#include "serg-rendergraph.h"

#include "serg-program.h"

#include <gio/gio.h>
#include <glib.h>
#include <graphene-gobject.h>

enum SergRenderGraphBindingIndex {
    SERG_BINDING_INDEX_0,
};

enum SergRenderGraphVertexAttribute {
    SERG_VERTEX_ATTRIBUTE_POSITION,
    SERG_VERTEX_ATTRIBUTE_NORMAL,
    SERG_VERTEX_ATTRIBUTE_TEXCOORD,
};

struct SergRenderGraphBuffers {
    GLuint vertex, element;
};

//

struct _SergRenderGraph {
    GObject parent_instance;
    // Private
    SergProgram *program;
    GLuint vertex_array_object;
    struct SergRenderGraphBuffers buffers;
    GLsizei count;
    // Properties
    graphene_point3d_t camera_position;
    graphene_euler_t camera_direction;
    float camera_aspect;
};

G_DEFINE_FINAL_TYPE(SergRenderGraph, serg_render_graph, G_TYPE_OBJECT)

enum Property {
    PROP_CAMERA_POSITION = 1,
    PROP_CAMERA_DIRECTION,
    PROP_CAMERA_ASPECT,
    N_PROPERTIES,
};

static GParamSpec *obj_properties[N_PROPERTIES];

// GObject /////////////////////////////////////////////////////////////////////

static void serg_render_graph_dispose(GObject *object)
{
    auto self = SERG_RENDER_GRAPH(object);
    g_clear_object(&self->program);
    glDeleteVertexArrays(1, &self->vertex_array_object);
    if (self->buffers.vertex) {
        glDeleteBuffers(1, &self->buffers.vertex);
    }
    if (self->buffers.element) {
        glDeleteBuffers(1, &self->buffers.element);
    }
    G_OBJECT_CLASS(serg_render_graph_parent_class)->dispose(object);
}

static void serg_render_graph_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec
)
{
    SergRenderGraph const *self = SERG_RENDER_GRAPH(object);
    switch ((enum Property)property_id) {
    case PROP_CAMERA_POSITION:
        g_value_set_boxed(value, &self->camera_position);
        break;
    case PROP_CAMERA_DIRECTION:
        g_value_set_boxed(value, &self->camera_direction);
        break;
    case PROP_CAMERA_ASPECT:
        g_value_set_float(value, self->camera_aspect);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void serg_render_graph_set_property(
    GObject *object,
    guint property_id,
    GValue const *value,
    GParamSpec *pspec
)
{
    SergRenderGraph *self = SERG_RENDER_GRAPH(object);
    switch ((enum Property)property_id) {
    case PROP_CAMERA_POSITION:
        graphene_point3d_init_from_point(
            &self->camera_position,
            g_value_get_boxed(value)
        );
        break;
    case PROP_CAMERA_DIRECTION:
        graphene_euler_init_from_euler(
            &self->camera_direction,
            g_value_get_boxed(value)
        );
        break;
    case PROP_CAMERA_ASPECT:
        self->camera_aspect = g_value_get_float(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

// SergRenderGraph /////////////////////////////////////////////////////////////

static void serg_render_graph_class_init(SergRenderGraphClass *klass)
{
    auto oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = serg_render_graph_dispose;
    oclass->get_property = serg_render_graph_get_property;
    oclass->set_property = serg_render_graph_set_property;

    obj_properties[PROP_CAMERA_POSITION] = g_param_spec_boxed(
        "camera-position",
        nullptr,
        nullptr,
        GRAPHENE_TYPE_POINT3D,
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
    );

    obj_properties[PROP_CAMERA_DIRECTION] = g_param_spec_boxed(
        "camera-direction",
        nullptr,
        nullptr,
        GRAPHENE_TYPE_EULER,
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
    );

    obj_properties[PROP_CAMERA_ASPECT] = g_param_spec_float(
        "camera-aspect",
        nullptr,
        nullptr,
        FLT_MIN,
        FLT_MAX,
        1.f,
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
    );

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);
}

static void serg_render_graph_init(SergRenderGraph *self)
{
    self->program = serg_program_new();

    glCreateVertexArrays(1, &self->vertex_array_object);

    // vertex.position : GLfloat[3]
    glEnableVertexArrayAttrib(
        self->vertex_array_object,
        SERG_VERTEX_ATTRIBUTE_POSITION
    );
    glVertexArrayAttribFormat(
        self->vertex_array_object,
        SERG_VERTEX_ATTRIBUTE_POSITION,
        3,
        GL_FLOAT,
        GL_FALSE,
        offsetof(SergRenderGraphVertex, position)
    );
    glVertexArrayAttribBinding(
        self->vertex_array_object,
        SERG_VERTEX_ATTRIBUTE_POSITION,
        SERG_BINDING_INDEX_0
    );

    // vertex.normal : GLfloat[3]
    glEnableVertexArrayAttrib(
        self->vertex_array_object,
        SERG_VERTEX_ATTRIBUTE_NORMAL
    );
    glVertexArrayAttribFormat(
        self->vertex_array_object,
        SERG_VERTEX_ATTRIBUTE_NORMAL,
        3,
        GL_FLOAT,
        GL_FALSE,
        offsetof(SergRenderGraphVertex, normal)
    );
    glVertexArrayAttribBinding(
        self->vertex_array_object,
        SERG_VERTEX_ATTRIBUTE_NORMAL,
        SERG_BINDING_INDEX_0
    );

    // vertex.texcoord : GLfloat[2]
    glEnableVertexArrayAttrib(
        self->vertex_array_object,
        SERG_VERTEX_ATTRIBUTE_TEXCOORD
    );
    glVertexArrayAttribFormat(
        self->vertex_array_object,
        SERG_VERTEX_ATTRIBUTE_TEXCOORD,
        2,
        GL_FLOAT,
        GL_FALSE,
        offsetof(SergRenderGraphVertex, texcoord)
    );
    glVertexArrayAttribBinding(
        self->vertex_array_object,
        SERG_VERTEX_ATTRIBUTE_TEXCOORD,
        SERG_BINDING_INDEX_0
    );
}

// Public //////////////////////////////////////////////////////////////////////

SergRenderGraph *serg_render_graph_new(void)
{
    return g_object_new(SERG_TYPE_RENDER_GRAPH, nullptr);
}

/**
 * serg_render_graph_get_camera_position
 * Returns: (transfer none)
 */
graphene_point3d_t const *
serg_render_graph_get_camera_position(SergRenderGraph *self)
{
    graphene_point3d_t *value = nullptr;
    g_object_get(self, "camera-position", &value, nullptr);
    return value;
}

void serg_render_graph_set_camera_position(
    SergRenderGraph *self,
    graphene_point3d_t const *camera_position
)
{
    g_object_set(self, "camera-position", camera_position, nullptr);
}

/**
 * serg_render_graph_get_camera_direction
 * Returns: (transfer none)
 */
graphene_euler_t const *
serg_render_graph_get_camera_direction(SergRenderGraph *self)
{
    graphene_euler_t *value = nullptr;
    g_object_get(self, "camera-direction", &value, nullptr);
    return value;
}

void serg_render_graph_set_camera_direction(
    SergRenderGraph *self,
    graphene_euler_t const *camera_direction
)
{
    g_object_set(self, "camera-direction", camera_direction, nullptr);
}

void serg_render_graph_set_camera_aspect(SergRenderGraph *self, float aspect)
{
    g_object_set(self, "camera-aspect", aspect, nullptr);
}

/**
 * serg_render_graph_camera_matrix
 * Returns: (transfer full)
 */
graphene_matrix_t *serg_render_graph_camera_matrix(SergRenderGraph *self)
{
    graphene_vec3_t eye, center;

    graphene_point3d_to_vec3(&self->camera_position, &eye);

    graphene_vec3_t forward;
    graphene_vec3_negate(graphene_vec3_z_axis(), &forward);

    graphene_matrix_t m;
    graphene_euler_to_matrix(&self->camera_direction, &m);
    graphene_matrix_transform_vec3(&m, &forward, &forward);

    graphene_vec3_init_from_vec3(&center, &eye);
    graphene_vec3_add(&center, &forward, &center);

    graphene_matrix_t *matrix = graphene_matrix_alloc();
    graphene_matrix_init_look_at(matrix, &eye, &center, graphene_vec3_y_axis());
    graphene_matrix_inverse(matrix, matrix);
    return matrix;
}

void serg_render_graph_set_vertices(
    SergRenderGraph *self,
    size_t length,
    SergRenderGraphVertex const vertices[length]
)
{
    g_return_if_fail(SERG_IS_RENDER_GRAPH(self));
    if (self->buffers.vertex) {
        glDeleteBuffers(1, &self->buffers.vertex);
    }
    glCreateBuffers(1, &self->buffers.vertex);
    glNamedBufferStorage(
        self->buffers.vertex,
        length * sizeof(SergRenderGraphVertex),
        vertices,
        0
    );
    glVertexArrayVertexBuffer(
        self->vertex_array_object,
        SERG_BINDING_INDEX_0,
        self->buffers.vertex,
        0,
        sizeof(SergRenderGraphVertex)
    );
}

void serg_render_graph_set_elements(
    SergRenderGraph *self,
    size_t length,
    GLuint const indices[length]
)
{
    g_return_if_fail(SERG_IS_RENDER_GRAPH(self));
    if (self->buffers.element) {
        glDeleteBuffers(1, &self->buffers.element);
    }
    glCreateBuffers(1, &self->buffers.element);
    glNamedBufferStorage(
        self->buffers.element,
        length * sizeof(GLuint),
        indices,
        0
    );
    glVertexArrayElementBuffer(
        self->vertex_array_object,
        self->buffers.element
    );
    self->count = length;
}

void serg_render_graph_render(SergRenderGraph *self)
{
    g_return_if_fail(SERG_IS_RENDER_GRAPH(self));

    constexpr float FOV = 60.f;
    constexpr float NEAR_PLANE = 1.f;
    constexpr float FAR_PLANE = 8192.f;

    SergProgramUniforms uniforms = {};

    graphene_matrix_init_identity(&uniforms.model);
    graphene_matrix_rotate_x(&uniforms.model, -90.f);

    auto view = serg_render_graph_camera_matrix(self);
    graphene_matrix_init_from_matrix(&uniforms.view, view);
    graphene_matrix_free(view);

    graphene_matrix_init_perspective(
        &uniforms.projection,
        FOV,
        self->camera_aspect,
        NEAR_PLANE,
        FAR_PLANE
    );

    uniforms.sampler = 0;

    graphene_vec3_init(&uniforms.modulate, 0.2f, 0.4f, 0.4f);

    // TODO: Bind texture
    glBindVertexArray(self->vertex_array_object);
    serg_program_use(self->program, &uniforms);
    glDrawElements(GL_TRIANGLE_FAN, self->count, GL_UNSIGNED_INT, (void *)0);
}
