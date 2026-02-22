#include "sew/sew-buttonrow.h"

#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>

struct _SewButtonRow {
    GtkListBoxRow parent_instance;
    // Properties
    gchar *title;
    gchar *start_icon_name;
    gchar *end_icon_name;
};

G_DEFINE_FINAL_TYPE(
    SewButtonRow,
    sew_button_row,
    GTK_TYPE_LIST_BOX_ROW
)

enum Property {
    PROP_TITLE = 1,
    PROP_START_ICON_NAME,
    PROP_END_ICON_NAME,
    N_PROPERITES,
};

static GParamSpec *obj_properties[N_PROPERITES];

// GObject /////////////////////////////////////////////////////////////////////

static void sew_button_row_dispose(GObject *object)
{
    gtk_widget_dispose_template(GTK_WIDGET(object), SEW_TYPE_BUTTON_ROW);
    G_OBJECT_CLASS(sew_button_row_parent_class)->dispose(object);
}

static void sew_button_row_finalize(GObject *object)
{
    SewButtonRow *self = SEW_BUTTON_ROW(object);
    g_free(self->title);
    g_free(self->start_icon_name);
    g_free(self->end_icon_name);
    G_OBJECT_CLASS(sew_button_row_parent_class)->finalize(object);
}

static void sew_button_row_set_property(
    GObject *object,
    guint property_id,
    GValue const *value,
    GParamSpec *pspec
)
{
    SewButtonRow *self = SEW_BUTTON_ROW(object);
    switch ((enum Property)property_id) {
    case PROP_TITLE:
        g_free(self->title);
        self->title = g_value_dup_string(value);
        break;
    case PROP_START_ICON_NAME:
        g_free(self->start_icon_name);
        self->start_icon_name = g_value_dup_string(value);
        break;
    case PROP_END_ICON_NAME:
        g_free(self->end_icon_name);
        self->end_icon_name = g_value_dup_string(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

static void sew_button_row_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec
)
{
    SewButtonRow *self = SEW_BUTTON_ROW(object);
    switch ((enum Property)property_id) {
    case PROP_TITLE:
        g_value_set_string(value, self->title);
        break;
    case PROP_START_ICON_NAME:
        g_value_set_string(value, self->start_icon_name);
        break;
    case PROP_END_ICON_NAME:
        g_value_set_string(value, self->end_icon_name);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

// SewButtonRow ///////////////////////////////////////////////////////////

static void sew_button_row_class_init(SewButtonRowClass *klass)
{
    GObjectClass *oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = sew_button_row_dispose;
    oclass->finalize = sew_button_row_finalize;
    oclass->get_property = sew_button_row_get_property;
    oclass->set_property = sew_button_row_set_property;

    obj_properties[PROP_TITLE] = g_param_spec_string(
        "title",
        nullptr,
        nullptr,
        "",
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
    );

    obj_properties[PROP_START_ICON_NAME] = g_param_spec_string(
        "start-icon-name",
        nullptr,
        nullptr,
        "",
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
    );

    obj_properties[PROP_END_ICON_NAME] = g_param_spec_string(
        "end-icon-name",
        nullptr,
        nullptr,
        "",
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
    );

    g_object_class_install_properties(oclass, N_PROPERITES, obj_properties);

    GtkWidgetClass *wclass = GTK_WIDGET_CLASS(klass);
    gtk_widget_class_set_template_from_resource(
        wclass,
        "/com/github/treecase/sew/ui/sew-buttonrow.ui"
    );
}

static void sew_button_row_init(SewButtonRow *self)
{
    gtk_widget_init_template(GTK_WIDGET(self));
}

// Public //////////////////////////////////////////////////////////////////////

char const *sew_button_row_get_title(SewButtonRow *self)
{
    char *title = nullptr;
    g_object_get(self, "title", &title, nullptr);
    return title;
}

void sew_button_row_set_title(SewButtonRow *self, char const *title)
{
    g_object_set(self, "title", title, nullptr);
}

char const *sew_button_row_get_start_icon_name(SewButtonRow *self)
{
    char *start_icon_name = nullptr;
    g_object_get(self, "start-icon-name", &start_icon_name, nullptr);
    return start_icon_name;
}

void sew_button_row_set_start_icon_name(SewButtonRow *self, char const *start_icon_name)
{
    g_object_set(self, "start-icon-name", start_icon_name, nullptr);
}

char const *sew_button_row_get_end_icon_name(SewButtonRow *self)
{
    char *end_icon_name = nullptr;
    g_object_get(self, "end-icon-name", &end_icon_name, nullptr);
    return end_icon_name;
}

void sew_button_row_set_end_icon_name(SewButtonRow *self, char const *end_icon_name)
{
    g_object_set(self, "end-icon-name", end_icon_name, nullptr);
}