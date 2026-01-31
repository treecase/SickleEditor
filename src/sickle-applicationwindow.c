#include "sickle-applicationwindow.h"

#include "config.h"
#include "rmf/rmf.h"

#include <glib-object.h>
#include <gtk/gtk.h>

struct _SickleApplicationWindow {
    GtkApplicationWindow parent_instance;
    unsigned int grid_size;
};

G_DEFINE_FINAL_TYPE(
    SickleApplicationWindow,
    sickle_application_window,
    GTK_TYPE_APPLICATION_WINDOW
)

enum {
    PROP_GRID_SIZE = 1,
    N_PROPERTIES,
};

static GParamSpec *obj_properties[N_PROPERTIES];

// GObject /////////////////////////////////////////////////////////////////////

static void sickle_application_window_dispose(GObject *object)
{
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

    obj_properties[PROP_GRID_SIZE] = g_param_spec_uint(
        "grid-size",
        nullptr,
        nullptr,
        1,
        512,
        32,
        G_PARAM_READWRITE
    );

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    gtk_widget_class_set_template_from_resource(
        widget_class,
        SE_GRESOURCE_PREFIX "ui/sickle-applicationwindow.ui"
    );
}

static void sickle_application_window_init(SickleApplicationWindow *self)
{
    gtk_widget_init_template(GTK_WIDGET(self));
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
        return;
    }
    g_print("Request to open '%s'\n", g_file_get_parse_name(file));

    g_autoptr(RmfRoot) root = rmf_root_new(file);
    RmfWorldspawn *x = rmf_root_get_worldspawn(root);
    char *classname = rmf_entity_data_get_classname(RMF_ENTITY_DATA(x));
    g_print("worlspawn.classname = %s\n", classname);
}

void sickle_application_window_save(SickleApplicationWindow *self, GFile *file)
{
    if (!file) {
        return;
    }
    g_print("Request to save '%s'\n", g_file_get_parse_name(file));
}
