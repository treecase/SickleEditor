#include "sew/sew-viewport3d.h"

#include "rmf/rmf.h"
#include "serg/serg-rendergraph.h"

#include <epoxy/gl.h>

struct _SewViewport3d {
    GtkGLArea parent_instance;
    // Properties
    RmfRoot *map;
    // GL
    SergRenderGraph *graph;
};

G_DEFINE_FINAL_TYPE(SewViewport3d, sew_viewport_3d, GTK_TYPE_GL_AREA)

enum Property {
    PROP_MAP = 1,
    N_PROPERTIES,
};

static GParamSpec *obj_properties[N_PROPERTIES];

// Private /////////////////////////////////////////////////////////////////////

static SergRenderGraphVertex const VERTICES[] = {
    // left top
    {{-0.5f, +0.5f, 1.0f}, {0.0f, 0.0f}},
    // left bottom
    {{-0.5f, -0.5f, 1.0f}, {0.0f, 0.0f}},
    // right top
    {{+0.5f, +0.5f, 1.0f}, {0.0f, 0.0f}},
    // right bottom
    {{+0.5f, -0.5f, 1.0f}, {0.0f, 0.0f}},
};

// A quad in GL_TRIANGLE_STRIP format
static GLuint const INDICES[] = {0, 1, 2, 3};

static void log_gl_debug_message(
    GLenum /*source*/,
    GLenum type,
    GLuint /*id*/,
    GLenum /*severity*/,
    GLsizei /*length*/,
    GLchar const *message,
    void const */*userParam*/
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
    glDebugMessageCallback(log_gl_debug_message, nullptr);
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
    case N_PROPERTIES:
    }
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
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
    case N_PROPERTIES:
    }
    G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
}

// GtkWidget ///////////////////////////////////////////////////////////////////

static void sew_viewport_3d_realize(GtkWidget *widget)
{
    GTK_WIDGET_CLASS(sew_viewport_3d_parent_class)->realize(widget);
    GtkGLArea *area = GTK_GL_AREA(widget);
    SewViewport3d *self = SEW_VIEWPORT_3D(widget);
    gtk_gl_area_make_current(area);

    init_gl_stuff();

    self->graph = serg_render_graph_new();
    serg_render_graph_set_vertices(
        self->graph,
        sizeof(VERTICES) / sizeof(*VERTICES),
        VERTICES
    );
    serg_render_graph_set_elements(
        self->graph,
        sizeof(INDICES) / sizeof(*INDICES),
        INDICES
    );
}

static void sew_viewport_3d_unrealize(GtkWidget *widget)
{
    gtk_gl_area_make_current(GTK_GL_AREA(widget));
    SewViewport3d *self = SEW_VIEWPORT_3D(widget);

    g_clear_object(&self->graph);

    GTK_WIDGET_CLASS(sew_viewport_3d_parent_class)->unrealize(widget);
}

// GtkGLArea ///////////////////////////////////////////////////////////////////

static gboolean sew_viewport_3d_render(GtkGLArea *area, GdkGLContext *)
{
    SewViewport3d *self = SEW_VIEWPORT_3D(area);

    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

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
    GtkGLArea *area = GTK_GL_AREA(self);
    gtk_gl_area_set_allowed_apis(area, GDK_GL_API_GL);
    gtk_gl_area_set_required_version(area, 4, 5);
    gtk_gl_area_set_auto_render(area, FALSE);
}
