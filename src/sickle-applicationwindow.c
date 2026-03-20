#include "sickle-applicationwindow.h"

#include "config.h"
#include "rmf/rmf.h"
#include "sew/sew.h"
#include "sickle-application.h"

#include <glib-object.h>
#include <gtk/gtk.h>

struct _SickleApplicationWindow {
    GtkApplicationWindow parent_instance;
    // Private
    GBinding *binding_application_view3d_textures;
    // Properties
    RmfRoot *map;
    unsigned int grid_size;
    // Template widgets
    SewViewport3d *view3D;
};

G_DEFINE_FINAL_TYPE(
    SickleApplicationWindow,
    sickle_application_window,
    GTK_TYPE_APPLICATION_WINDOW
)

enum {
    PROP_MAP = 1,
    PROP_GRID_SIZE,
    N_PROPERTIES,
};

static GParamSpec *obj_properties[N_PROPERTIES];

// Signal Handlers /////////////////////////////////////////////////////////////

static void on_notify_application(GObject *object, GParamSpec *, gpointer)
{
    SickleApplicationWindow *self = SICKLE_APPLICATION_WINDOW(object);
    if (self->binding_application_view3d_textures) {
        g_binding_unbind(self->binding_application_view3d_textures);
    }
    self->binding_application_view3d_textures = g_object_bind_property(
        gtk_window_get_application(GTK_WINDOW(self)),
        "texture-archives",
        self->view3D,
        "textures",
        G_BINDING_SYNC_CREATE
    );
}

// GObject /////////////////////////////////////////////////////////////////////

static void sickle_application_window_dispose(GObject *object)
{
    auto self = SICKLE_APPLICATION_WINDOW(object);
    g_clear_object(&self->map);
    g_clear_object(&self->binding_application_view3d_textures);
    gtk_widget_dispose_template(
        GTK_WIDGET(object),
        SICKLE_TYPE_APPLICATION_WINDOW
    );
    G_OBJECT_CLASS(sickle_application_window_parent_class)->dispose(object);
}

static void sickle_application_window_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec
)
{
    SickleApplicationWindow *self = SICKLE_APPLICATION_WINDOW(object);
    switch (property_id) {
    case PROP_MAP:
        g_value_set_object(value, self->map);
        break;
    case PROP_GRID_SIZE:
        g_value_set_uint(value, self->grid_size);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

static void sickle_application_window_set_property(
    GObject *object,
    guint property_id,
    GValue const *value,
    GParamSpec *pspec
)
{
    SickleApplicationWindow *self = SICKLE_APPLICATION_WINDOW(object);
    switch (property_id) {
    case PROP_MAP:
        g_clear_object(&self->map);
        self->map = RMF_ROOT(g_value_dup_object(value));
        break;
    case PROP_GRID_SIZE:
        self->grid_size = g_value_get_uint(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

// SickleApplicationWindow /////////////////////////////////////////////////////

static void
sickle_application_window_class_init(SickleApplicationWindowClass *klass)
{
    GObjectClass *oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = sickle_application_window_dispose;
    oclass->get_property = sickle_application_window_get_property;
    oclass->set_property = sickle_application_window_set_property;

    obj_properties[PROP_MAP] = g_param_spec_object(
        "map",
        nullptr,
        nullptr,
        RMF_TYPE_ROOT,
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
    );

    obj_properties[PROP_GRID_SIZE] = g_param_spec_uint(
        "grid-size",
        nullptr,
        nullptr,
        1,
        512,
        32,
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
    );

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    gtk_widget_class_set_template_from_resource(
        widget_class,
        SE_GRESOURCE_PREFIX "ui/sickle-applicationwindow.ui"
    );
    gtk_widget_class_bind_template_child(
        widget_class,
        SickleApplicationWindow,
        view3D
    );
}

static void sickle_application_window_init(SickleApplicationWindow *self)
{
    gtk_widget_init_template(GTK_WIDGET(self));
    g_signal_connect(
        self,
        "notify::application",
        G_CALLBACK(on_notify_application),
        nullptr
    );
}

// Public //////////////////////////////////////////////////////////////////////

SickleApplicationWindow *sickle_application_window_new(void)
{
    return g_object_new(
        SICKLE_TYPE_APPLICATION_WINDOW,
        // FIXME: temp
        "show-menubar",
        TRUE,
        nullptr
    );
}

void sickle_application_window_open(SickleApplicationWindow *self, GFile *file)
{
    if (!file) {
        g_object_set(self, "map", nullptr, nullptr);
        return;
    }
    g_print("Request to open '%s'\n", g_file_get_parse_name(file));

    auto loader = rmf_loader_new();

    g_autoptr(GError) error = nullptr;
    rmf_loader_load_from_file(loader, file, &error);
    if (error) {
        g_error("%s -- %s", __FUNCTION__, error->message);
    }

    RmfRoot *root = rmf_loader_get_root(loader);

    g_object_set(self, "map", root, nullptr);
}

void sickle_application_window_save(SickleApplicationWindow *, GFile *file)
{
    if (!file) {
        return;
    }
    g_print("Request to save '%s'\n", g_file_get_parse_name(file));
    // TODO
}
