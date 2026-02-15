#include "serg-rendergraph.h"

#include <gio/gio.h>
#include <glib.h>

enum SergRenderGraphBindingIndex {
    SERG_BINDING_INDEX_0,
};

enum SergRenderGraphVertexAttribute {
    SERG_VERTEX_ATTRIBUTE_POSITION,
    SERG_VERTEX_ATTRIBUTE_TEXCOORD,
};

struct SergRenderGraphBuffers {
    GLuint vertex, element;
};

//

struct _SergRenderGraph {
    GObject parent_instance;
    GLuint program;
    GLuint vertex_array_object;
    struct SergRenderGraphBuffers buffers;
    GLsizei count;
};

G_DEFINE_FINAL_TYPE(SergRenderGraph, serg_render_graph, G_TYPE_OBJECT)

// Private /////////////////////////////////////////////////////////////////////

static GLuint make_shader(GLenum type, char const *name)
{
    g_autofree char const *path
        = g_strdup_printf("/com/github/treecase/serg/glsl/%s", name);

    g_autoptr(GError) error = nullptr;
    GBytes *bytes
        = g_resources_lookup_data(path, G_RESOURCE_LOOKUP_FLAGS_NONE, &error);
    if (error) {
        g_printerr("%s: %s", __FUNCTION__, error->message);
        return 0;
    }

    gsize size = 0;
    g_autofree char const *source = g_bytes_unref_to_data(bytes, &size);

    GLuint shader = glCreateShader(type);
    GLint length = size;
    glShaderSource(shader, 1, &source, &length);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logSize = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
        g_autofree GLchar *log = g_malloc0(logSize);
        glGetShaderInfoLog(shader, logSize, nullptr, log);
        g_printerr("failed to compile %s: %s\n", name, log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static GLuint make_program(void)
{
    GLuint vshader = make_shader(GL_VERTEX_SHADER, "map.vert");
    GLuint fshader = make_shader(GL_FRAGMENT_SHADER, "map.frag");

    GLuint program = glCreateProgram();

    glAttachShader(program, vshader);
    glAttachShader(program, fshader);

    glLinkProgram(program);
    glDetachShader(program, vshader);
    glDetachShader(program, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logSize = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
        g_autofree GLchar *log = g_malloc0(logSize);
        glGetProgramInfoLog(program, logSize, nullptr, log);
        g_printerr("failed to compile program: %s\n", log);
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

// GObject /////////////////////////////////////////////////////////////////////

static void serg_render_graph_dispose(GObject *object)
{
    auto self = SERG_RENDER_GRAPH(object);
    glDeleteProgram(self->program);
    glDeleteVertexArrays(1, &self->vertex_array_object);
    if (self->buffers.vertex) {
        glDeleteBuffers(1, &self->buffers.vertex);
    }
    if (self->buffers.element) {
        glDeleteBuffers(1, &self->buffers.element);
    }
    G_OBJECT_CLASS(serg_render_graph_parent_class)->dispose(object);
}

// SergRenderGraph /////////////////////////////////////////////////////////////

static void serg_render_graph_class_init(SergRenderGraphClass *klass)
{
    auto oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = serg_render_graph_dispose;
}

static void serg_render_graph_init(SergRenderGraph *self)
{
    self->program = make_program();

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
    glBindVertexArray(self->vertex_array_object);
    glUseProgram(self->program);
    glDrawElements(GL_TRIANGLE_STRIP, self->count, GL_UNSIGNED_INT, (void *)0);
}
