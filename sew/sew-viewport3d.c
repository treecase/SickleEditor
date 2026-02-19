#include "sew/sew-viewport3d.h"

#include "rmf/rmf.h"
#include "serg/serg-rendergraph.h"

#include <epoxy/gl.h>

struct _SewViewport3d {
    GtkGLArea parent_instance;
    // Properties
    RmfRoot *map;
    // Private
    SergRenderGraph *graph;
    struct {
        float forward, backward;
        float right, left;
        float up, down;
        float rotation_up, rotation_down;
        float rotation_left, rotation_right;
    } input;
};

G_DEFINE_FINAL_TYPE(SewViewport3d, sew_viewport_3d, GTK_TYPE_GL_AREA)

enum Property {
    PROP_MAP = 1,
    N_PROPERTIES,
};

static GParamSpec *obj_properties[N_PROPERTIES];

// Private /////////////////////////////////////////////////////////////////////

static void log_gl_debug_message(
    GLenum,
    GLenum type,
    GLuint,
    GLenum,
    GLsizei,
    GLchar const *message,
    void const *
)
{
    g_printerr(
        "%s :: %s\n",
        type == GL_DEBUG_TYPE_ERROR ? "GL ERROR" : "GL",
        message
    );
}

static void init_gl_stuff(void)
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
    glDebugMessageCallback(log_gl_debug_message, nullptr);
}

static gboolean on_key_pressed(
    GtkEventControllerKey *,
    guint keyval,
    guint,
    GdkModifierType,
    gpointer user_data
)
{
    SewViewport3d *self = SEW_VIEWPORT_3D(user_data);
    switch (keyval)
    {
    case GDK_KEY_q:
        self->input.up = 1.f;
        break;
    case GDK_KEY_w:
        self->input.forward = 1.f;
        break;
    case GDK_KEY_e:
        self->input.down = 1.f;
        break;
    case GDK_KEY_a:
        self->input.left = 1.f;
        break;
    case GDK_KEY_s:
        self->input.backward = 1.f;
        break;
    case GDK_KEY_d:
        self->input.right = 1.f;
        break;
    case GDK_KEY_Up:
        self->input.rotation_up = 1.f;
        break;
    case GDK_KEY_Left:
        self->input.rotation_left = 1.f;
        break;
    case GDK_KEY_Down:
        self->input.rotation_down = 1.f;
        break;
    case GDK_KEY_Right:
        self->input.rotation_right = 1.f;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

static gboolean on_key_released(
    GtkEventControllerKey *,
    guint keyval,
    guint,
    GdkModifierType,
    gpointer user_data
)
{
    SewViewport3d *self = SEW_VIEWPORT_3D(user_data);
    switch (keyval)
    {
    case GDK_KEY_q:
        self->input.up = 0.f;
        break;
    case GDK_KEY_w:
        self->input.forward = 0.f;
        break;
    case GDK_KEY_e:
        self->input.down = 0.f;
        break;
    case GDK_KEY_a:
        self->input.left = 0.f;
        break;
    case GDK_KEY_s:
        self->input.backward = 0.f;
        break;
    case GDK_KEY_d:
        self->input.right = 0.f;
        break;
    case GDK_KEY_Up:
        self->input.rotation_up = 0.f;
        break;
    case GDK_KEY_Left:
        self->input.rotation_left = 0.f;
        break;
    case GDK_KEY_Down:
        self->input.rotation_down = 0.f;
        break;
    case GDK_KEY_Right:
        self->input.rotation_right = 0.f;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

static float clamp(float x, float min, float max)
{
    if (x < min) {
        return min;
    } else if (x > max) {
        return max;
    } else {
        return x;
    }
}

static gboolean on_tick(GtkWidget *widget, GdkFrameClock *frame_clock, gpointer)
{
    SewViewport3d *self = SEW_VIEWPORT_3D(widget);

    constexpr float MOVE_SPEED = 192.f; // units/second
    constexpr float TURN_SPEED = 120.f; // degrees/second
    constexpr float PITCH_LIMIT = 89.9f; // degrees

    float fps = gdk_frame_clock_get_fps(frame_clock);
    fps = fps != 0.f? fps : 60.f;
    float delta = 1.f / fps;

    // Update the camera's direction:
    // Camera pitch/yaw inputs (normalized unitless)
    float pitch_input = self->input.rotation_up - self->input.rotation_down;
    float yaw_input = self->input.rotation_left - self->input.rotation_right;
    // Camera pitch/yaw/roll deltas (degrees)
    graphene_vec3_t direction_delta;
    graphene_vec3_init(
        &direction_delta,
        TURN_SPEED * pitch_input * delta,
        TURN_SPEED * yaw_input * delta,
        0.f
    );
    // Get current camera direction
    graphene_euler_t const *direction =
        serg_render_graph_get_camera_direction(self->graph);
    // Camera current pitch/yaw/roll (degrees)
    graphene_vec3_t vDirection;
    graphene_euler_to_vec3(direction, &vDirection);
    // Add pitch/yaw/roll deltas to current direction
    graphene_vec3_add(&vDirection, &direction_delta, &vDirection);
    // Clamp pitch
    graphene_vec3_init(
        &vDirection,
        clamp(graphene_vec3_get_x(&vDirection), -PITCH_LIMIT, PITCH_LIMIT),
        graphene_vec3_get_y(&vDirection),
        graphene_vec3_get_z(&vDirection)
    );
    // Create new direction object
    graphene_euler_t direction_new;
    graphene_euler_init_from_vec3(
        &direction_new,
        &vDirection,
        graphene_euler_get_order(direction)
    );

    // Update the camera's position:
    // Camera space basis vectors
    graphene_vec3_t right, up, forward;
    graphene_vec3_init(&right, 1.f, 0.f, 0.f);
    graphene_vec3_init(&up, 0.f, 1.f, 0.f);
    graphene_vec3_init(&forward, 0.f, 0.f, -1.f);
    // Camera space deltas
    graphene_vec3_t right_delta, up_delta, forward_delta;
    graphene_vec3_scale(
        &right,
        (self->input.right - self->input.left) * MOVE_SPEED * delta,
        &right_delta
    );
    graphene_vec3_scale(
        &up,
        (self->input.up - self->input.down) * MOVE_SPEED * delta,
        &up_delta
    );
    graphene_vec3_scale(
        &forward,
        (self->input.forward - self->input.backward) * MOVE_SPEED * delta,
        &forward_delta
    );
    // Transform deltas from camera space to world space
    graphene_matrix_t camera_to_world;
    graphene_euler_to_matrix(&direction_new, &camera_to_world);
    graphene_matrix_transform_vec3(&camera_to_world, &right_delta, &right_delta);
    graphene_matrix_transform_vec3(&camera_to_world, &up_delta, &up_delta);
    graphene_matrix_transform_vec3(&camera_to_world, &forward_delta, &forward_delta);
    // Compute final world space delta
    graphene_vec3_t position_delta;
    graphene_vec3_init(&position_delta, 0.f, 0.f, 0.f);
    graphene_vec3_subtract(&position_delta, &right_delta, &position_delta);
    graphene_vec3_subtract(&position_delta, &up_delta, &position_delta);
    graphene_vec3_subtract(&position_delta, &forward_delta, &position_delta);
    // Compute new position
    graphene_vec3_t position;
    graphene_point3d_to_vec3(
        serg_render_graph_get_camera_position(self->graph),
        &position
    );
    graphene_vec3_add(&position, &position_delta, &position);
    // Create new Point3D object
    graphene_point3d_t position_new;
    graphene_point3d_init_from_vec3(&position_new, &position);

    serg_render_graph_set_camera_direction(self->graph, &direction_new);
    serg_render_graph_set_camera_position(self->graph, &position_new);

    gtk_gl_area_queue_render(GTK_GL_AREA(self));
    return G_SOURCE_CONTINUE;
}

static void on_map_changed(GObject *object, GParamSpec *, gpointer)
{
    SewViewport3d *self = SEW_VIEWPORT_3D(object);
    if (self->map == nullptr) {
        return;
    }

    g_autoptr(GArray) VERTICES = g_array_new(FALSE, FALSE, sizeof(SergRenderGraphVertex));
    g_autoptr(GArray) INDICES = g_array_new(FALSE, FALSE, sizeof(GLuint));

    RmfWorldspawn *worldspawn = rmf_root_get_worldspawn(self->map);
    RmfMapObjectIterator *children = rmf_map_object_get_children(RMF_MAP_OBJECT(worldspawn));
    GLuint index = 0;
    RMF_ITERATOR_FOREACH(RmfMapObject, child, children) {
        if (RMF_IS_SOLID(child)) {
            RmfFaceIterator *faces = rmf_solid_get_faces(RMF_SOLID(child));
            RMF_ITERATOR_FOREACH(RmfFace, face, faces) {
                // Compute the face normal.
                graphene_vec3_t planeA, planeB, planeNormal;
                graphene_vec3_init(
                    &planeA,
                    face->plane_points[1].x - face->plane_points[0].x,
                    face->plane_points[1].y - face->plane_points[0].y,
                    face->plane_points[1].z - face->plane_points[0].z
                );
                graphene_vec3_init(
                    &planeB,
                    face->plane_points[2].x - face->plane_points[0].x,
                    face->plane_points[2].y - face->plane_points[0].y,
                    face->plane_points[2].z - face->plane_points[0].z
                );
                graphene_vec3_cross(&planeA, &planeB, &planeNormal);
                graphene_vec3_normalize(&planeNormal, &planeNormal);
                // RMF stores vertices in clockwise order, so they are added in
                // reverse since OpenGL uses counterclockwise winding.
                RmfVector *vertices = (RmfVector *)face->vertices->data;
                for (guint i = face->vertices->len; i > 0; --i) {
                    RmfVector *vertex = &vertices[i - 1];
                    SergRenderGraphVertex rVertex = {
                        .position = {vertex->x, vertex->y, vertex->z},
                        .normal = {
                            graphene_vec3_get_x(&planeNormal),
                            graphene_vec3_get_y(&planeNormal),
                            graphene_vec3_get_z(&planeNormal),
                        },
                        .texcoord = {0.f, 0.f}, // TODO
                    };
                    g_array_append_val(VERTICES, rVertex);
                    g_array_append_val(INDICES, index);
                    index++;
                }
                constexpr GLuint RESTART_IDX = 0xffffffff;
                g_array_append_val(INDICES, RESTART_IDX);
            }
        }
    }

    serg_render_graph_set_vertices(self->graph, VERTICES->len, (SergRenderGraphVertex *)VERTICES->data);
    serg_render_graph_set_elements(self->graph, INDICES->len, (GLuint *)INDICES->data);
}

static void on_resized(GtkGLArea *area, gint width, gint height, gpointer)
{
    float aspect = (float)width / (float)height;
    serg_render_graph_set_camera_aspect(SEW_VIEWPORT_3D(area)->graph, aspect);
}

// GObject /////////////////////////////////////////////////////////////////////

static void sew_viewport_3d_dispose(GObject *object)
{
    auto self = SEW_VIEWPORT_3D(object);
    g_clear_object(&self->map);
    G_OBJECT_CLASS(sew_viewport_3d_parent_class)->dispose(object);
}

static void sew_viewport_3d_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec
)
{
    auto self = SEW_VIEWPORT_3D(object);
    switch ((enum Property)property_id) {
    case PROP_MAP:
        g_value_set_object(value, self->map);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void sew_viewport_3d_set_property(
    GObject *object,
    guint property_id,
    GValue const *value,
    GParamSpec *pspec
)
{
    auto self = SEW_VIEWPORT_3D(object);
    switch ((enum Property)property_id) {
    case PROP_MAP:
        g_clear_object(&self->map);
        self->map = g_value_get_object(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

// GtkWidget ///////////////////////////////////////////////////////////////////

static void sew_viewport_3d_realize(GtkWidget *widget)
{
    GTK_WIDGET_CLASS(sew_viewport_3d_parent_class)->realize(widget);

    gtk_gl_area_make_current(GTK_GL_AREA(widget));
    init_gl_stuff();

    SewViewport3d *self = SEW_VIEWPORT_3D(widget);

    self->graph = serg_render_graph_new();
    on_map_changed(G_OBJECT(self), obj_properties[PROP_MAP], nullptr);
    g_signal_handlers_unblock_by_func(self, on_map_changed, nullptr);
}

static void sew_viewport_3d_unrealize(GtkWidget *widget)
{
    gtk_gl_area_make_current(GTK_GL_AREA(widget));
    SewViewport3d *self = SEW_VIEWPORT_3D(widget);

    g_signal_handlers_block_by_func(self, on_map_changed, nullptr);
    g_clear_object(&self->graph);

    GTK_WIDGET_CLASS(sew_viewport_3d_parent_class)->unrealize(widget);
}

// GtkGLArea ///////////////////////////////////////////////////////////////////

static gboolean sew_viewport_3d_render(GtkGLArea *area, GdkGLContext *)
{
    SewViewport3d *self = SEW_VIEWPORT_3D(area);

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    serg_render_graph_render(self->graph);

    return TRUE;
}

// SewViewport3d ///////////////////////////////////////////////////////////////

static void sew_viewport_3d_class_init(SewViewport3dClass *klass)
{
    GtkGLAreaClass *glarea_class = GTK_GL_AREA_CLASS(klass);
    glarea_class->render = sew_viewport_3d_render;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    widget_class->realize = sew_viewport_3d_realize;
    widget_class->unrealize = sew_viewport_3d_unrealize;
    gtk_widget_class_set_css_name(widget_class, "viewport3d");

    GObjectClass *oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = sew_viewport_3d_dispose;
    oclass->get_property = sew_viewport_3d_get_property;
    oclass->set_property = sew_viewport_3d_set_property;

    obj_properties[PROP_MAP] = g_param_spec_object(
        "map",
        nullptr,
        nullptr,
        RMF_TYPE_ROOT,
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
    );

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);
}

static void sew_viewport_3d_init(SewViewport3d *self)
{
    g_signal_connect(self, "notify::map", G_CALLBACK(on_map_changed), nullptr);
    g_signal_connect(self, "resize", G_CALLBACK(on_resized), nullptr);
    g_signal_handlers_block_by_func(self, on_map_changed, nullptr);

    GtkGLArea *area = GTK_GL_AREA(self);
    gtk_gl_area_set_allowed_apis(area, GDK_GL_API_GL);
    gtk_gl_area_set_required_version(area, 4, 5);
    gtk_gl_area_set_has_depth_buffer(area, TRUE);
    gtk_gl_area_set_auto_render(area, FALSE);

    auto controller = gtk_event_controller_key_new();
    g_signal_connect(controller, "key-pressed", G_CALLBACK(on_key_pressed), self);
    g_signal_connect(controller, "key-released", G_CALLBACK(on_key_released), self);

    gtk_widget_add_controller(GTK_WIDGET(self), controller);
    gtk_widget_add_tick_callback(GTK_WIDGET(self), on_tick, nullptr, nullptr);
    gtk_widget_set_focusable(GTK_WIDGET(self), TRUE);
}
