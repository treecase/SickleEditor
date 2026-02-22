#include "sew/sew-filerow.h"

#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>

struct _SewFileRow {
    GtkListBoxRow parent_instance;
    // Properties
    GFile *file;
};

G_DEFINE_FINAL_TYPE(
    SewFileRow,
    sew_file_row,
    GTK_TYPE_LIST_BOX_ROW
)

enum Property {
    PROP_FILE = 1,
    N_PROPERITES,
};

static GParamSpec *obj_properties[N_PROPERITES];

// Private /////////////////////////////////////////////////////////////////////

static char const *make_label(SewFileRow *, GFile *file)
{
    return file? g_file_get_basename(file) : nullptr;
}

// GObject /////////////////////////////////////////////////////////////////////

static void sew_file_row_dispose(GObject *object)
{
    gtk_widget_dispose_template(GTK_WIDGET(object), SEW_TYPE_FILE_ROW);
    g_clear_object(&SEW_FILE_ROW(object)->file);
    G_OBJECT_CLASS(sew_file_row_parent_class)->dispose(object);
}

static void sew_file_row_set_property(
    GObject *object,
    guint property_id,
    GValue const *value,
    GParamSpec *pspec
)
{
    SewFileRow *self = SEW_FILE_ROW(object);
    switch ((enum Property)property_id) {
    case PROP_FILE:
        g_clear_object(&self->file);
        self->file = g_value_dup_object(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

static void sew_file_row_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec
)
{
    SewFileRow *self = SEW_FILE_ROW(object);
    switch ((enum Property)property_id) {
    case PROP_FILE:
        g_value_set_object(value, self->file);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

// SewFileRow //////////////////////////////////////////////////////////////////

static void sew_file_row_class_init(SewFileRowClass *klass)
{
    GObjectClass *oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = sew_file_row_dispose;
    oclass->get_property = sew_file_row_get_property;
    oclass->set_property = sew_file_row_set_property;

    obj_properties[PROP_FILE] = g_param_spec_object(
        "file",
        nullptr,
        nullptr,
        G_TYPE_FILE,
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
    );

    g_object_class_install_properties(oclass, N_PROPERITES, obj_properties);

    GtkWidgetClass *wclass = GTK_WIDGET_CLASS(klass);
    gtk_widget_class_set_template_from_resource(
        wclass,
        "/com/github/treecase/sew/ui/sew-filerow.ui"
    );
    gtk_widget_class_bind_template_callback(klass, make_label);
}

static void sew_file_row_init(SewFileRow *self)
{
    gtk_widget_init_template(GTK_WIDGET(self));
}

// Public //////////////////////////////////////////////////////////////////////

SewFileRow *sew_file_row_new(GFile *file)
{
    return g_object_new(SEW_TYPE_FILE_ROW, "file", file, nullptr);
}

GFile *sew_file_row_get_file(SewFileRow *self)
{
    GFile *file = nullptr;
    g_object_get(self, "file", &file, nullptr);
    return file;
}

void sew_file_row_set_file(SewFileRow *self, GFile *file)
{
    g_object_set(self, "file", file, nullptr);
}