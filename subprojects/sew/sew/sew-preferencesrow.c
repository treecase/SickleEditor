#include "sew/sew-preferencesrow.h"

#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>

struct _SewPreferencesRow {
    GtkListBoxRow parent_instance;
    gchar *title;
    gchar *subtitle;
    GFile *file;
    gboolean as_folder;
    GtkFileDialog *file_dialog;
    // Template widgets
    GtkButton *openButton;
    GtkLabel *pathLabel;
};

G_DEFINE_FINAL_TYPE(
    SewPreferencesRow,
    sew_preferences_row,
    GTK_TYPE_LIST_BOX_ROW
)

typedef enum {
    PROP_TITLE = 1,
    PROP_SUBTITLE,
    PROP_FILE,
    PROP_AS_FOLDER,
    PROP_FILTER,
    N_PROPERITES,
} SewPreferencesRowProperty;

static GParamSpec *obj_properties[N_PROPERITES];

// Signal Handlers /////////////////////////////////////////////////////////////

static gboolean is_file_nondefault(SewPreferencesRow *self)
{
    GFile *file = sew_preferences_row_get_file(self);
    return file ? TRUE : FALSE;
}

static char *make_path_displayable(SewPreferencesRow *self, GFile *file)
{
    g_return_val_if_fail(SEW_IS_PREFERENCES_ROW(self), g_strdup("???"));
    if (!file) {
        return g_strdup("(Default)");
    } else {
        g_return_val_if_fail(G_IS_FILE(file), g_strdup("???"));
        return g_strdup(g_file_get_basename(file));
    }
}

struct filedialog_finish_data {
    SewPreferencesRow *self;
    gboolean as_folder;
};

static void on_filedialog_open_finished(
    GObject *source_object,
    GAsyncResult *res,
    gpointer data
)
{
    g_autofree struct filedialog_finish_data *cbdata = data;
    SewPreferencesRow *self = SEW_PREFERENCES_ROW(cbdata->self);
    gboolean as_folder = cbdata->as_folder;

    auto finish = as_folder ? gtk_file_dialog_select_folder_finish
                            : gtk_file_dialog_open_finish;

    g_autoptr(GError) error = nullptr;
    GFile *file = finish(GTK_FILE_DIALOG(source_object), res, &error);
    if (error && error->code != GTK_DIALOG_ERROR_DISMISSED) {
        g_printerr("%s: %s\n", __FUNCTION__, error->message);
    } else if (file) {
        sew_preferences_row_set_file(self, file);
    }
}

static void on_activated(SewPreferencesRow *self, void *)
{
    gboolean as_folder = sew_preferences_row_get_as_folder(self);
    auto f = as_folder ? gtk_file_dialog_select_folder : gtk_file_dialog_open;

    struct filedialog_finish_data *data
        = g_new(struct filedialog_finish_data, 1);
    data->self = self;
    data->as_folder = as_folder;

    f(self->file_dialog, nullptr, nullptr, on_filedialog_open_finished, data);
}

static void on_reset_clicked(SewPreferencesRow *self, GtkButton *)
{
    g_return_if_fail(SEW_IS_PREFERENCES_ROW(self));
    sew_preferences_row_set_file(self, nullptr);
}

// GObject /////////////////////////////////////////////////////////////////////

static void sew_preferences_row_dispose(GObject *object)
{
    SewPreferencesRow *self = SEW_PREFERENCES_ROW(object);
    g_clear_object(&self->file);
    g_clear_object(&self->file_dialog);
    gtk_widget_dispose_template(GTK_WIDGET(object), SEW_TYPE_PREFERENCES_ROW);
    G_OBJECT_CLASS(sew_preferences_row_parent_class)->dispose(object);
}

static void sew_preferences_row_finalize(GObject *object)
{
    SewPreferencesRow *self = SEW_PREFERENCES_ROW(object);
    g_free(self->title);
    g_free(self->subtitle);
    G_OBJECT_CLASS(sew_preferences_row_parent_class)->finalize(object);
}

static void sew_preferences_row_set_property(
    GObject *object,
    guint property_id,
    GValue const *value,
    GParamSpec *pspec
)
{
    SewPreferencesRow *self = SEW_PREFERENCES_ROW(object);
    switch (property_id) {
    case PROP_TITLE:
        g_free(self->title);
        self->title = g_value_dup_string(value);
        break;
    case PROP_SUBTITLE:
        g_free(self->subtitle);
        self->subtitle = g_value_dup_string(value);
        break;
    case PROP_FILE:
        g_clear_object(&self->file);
        self->file = g_value_dup_object(value);
        break;
    case PROP_AS_FOLDER:
        self->as_folder = g_value_get_boolean(value);
        break;
    case PROP_FILTER:
        gtk_file_dialog_set_default_filter(
            self->file_dialog,
            g_value_dup_object(value)
        );
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

static void sew_preferences_row_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec
)
{
    SewPreferencesRow *self = SEW_PREFERENCES_ROW(object);
    switch (property_id) {
    case PROP_TITLE:
        g_value_set_string(value, self->title);
        break;
    case PROP_SUBTITLE:
        g_value_set_string(value, self->subtitle);
        break;
    case PROP_FILE:
        g_value_set_object(value, self->file);
        break;
    case PROP_AS_FOLDER:
        g_value_set_boolean(value, self->as_folder);
        break;
    case PROP_FILTER:
        g_value_set_object(
            value,
            gtk_file_dialog_get_default_filter(self->file_dialog)
        );
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

// SewPreferencesRow ///////////////////////////////////////////////////////////

static void sew_preferences_row_class_init(SewPreferencesRowClass *klass)
{
    GObjectClass *oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = sew_preferences_row_dispose;
    oclass->finalize = sew_preferences_row_finalize;
    oclass->get_property = sew_preferences_row_get_property;
    oclass->set_property = sew_preferences_row_set_property;

    obj_properties[PROP_TITLE] = g_param_spec_string(
        "title",
        "Title",
        "The title for this row.",
        "",
        G_PARAM_READWRITE
    );

    obj_properties[PROP_SUBTITLE] = g_param_spec_string(
        "subtitle",
        "Subtitle",
        "The subtitle for this row.",
        "",
        G_PARAM_READWRITE
    );

    obj_properties[PROP_FILE] = g_param_spec_object(
        "file",
        "File",
        "File associated with this preference.",
        G_TYPE_FILE,
        G_PARAM_READWRITE
    );

    obj_properties[PROP_AS_FOLDER] = g_param_spec_boolean(
        "as-folder",
        "As Folder",
        "If true, the property is a folder rather than a file.",
        FALSE,
        G_PARAM_READWRITE
    );

    obj_properties[PROP_FILTER] = g_param_spec_object(
        "filter",
        "Filter",
        "File filter.",
        GTK_TYPE_FILE_FILTER,
        G_PARAM_READWRITE
    );

    g_object_class_install_properties(oclass, N_PROPERITES, obj_properties);

    GtkWidgetClass *wclass = GTK_WIDGET_CLASS(klass);
    gtk_widget_class_set_template_from_resource(
        wclass,
        "/com/github/treecase/sew/ui/sew-preferencesrow.ui"
    );
    gtk_widget_class_bind_template_callback(wclass, on_activated);
    gtk_widget_class_bind_template_callback(wclass, on_reset_clicked);
    gtk_widget_class_bind_template_callback(wclass, is_file_nondefault);
    gtk_widget_class_bind_template_callback(wclass, make_path_displayable);
    gtk_widget_class_bind_template_child(wclass, SewPreferencesRow, openButton);
    gtk_widget_class_bind_template_child(wclass, SewPreferencesRow, pathLabel);
}

static void sew_preferences_row_init(SewPreferencesRow *self)
{
    gtk_widget_init_template(GTK_WIDGET(self));
    self->file_dialog = gtk_file_dialog_new();
}

// Public //////////////////////////////////////////////////////////////////////

SewPreferencesRow *sew_preferences_row_new(void)
{
    return g_object_new(SEW_TYPE_PREFERENCES_ROW, nullptr);
}

char *sew_preferences_row_get_title(SewPreferencesRow *self)
{
    char *title = nullptr;
    g_object_get(self, "title", &title, nullptr);
    return title;
}

char *sew_preferences_row_get_subtitle(SewPreferencesRow *self)
{
    char *subtitle = nullptr;
    g_object_get(self, "subtitle", &subtitle, nullptr);
    return subtitle;
}

GFile *sew_preferences_row_get_file(SewPreferencesRow *self)
{
    GFile *file = nullptr;
    g_object_get(self, "file", &file, nullptr);
    return file;
}

gboolean sew_preferences_row_get_as_folder(SewPreferencesRow *self)
{
    gboolean as_folder = FALSE;
    g_object_get(self, "as-folder", &as_folder, nullptr);
    return as_folder;
}

GtkFileFilter *sew_preferences_row_get_filter(SewPreferencesRow *self)
{
    GtkFileFilter *filter = nullptr;
    g_object_get(self, "filter", &filter, nullptr);
    return filter;
}

void sew_preferences_row_set_title(SewPreferencesRow *self, char *title)
{
    g_object_set(self, "title", title, nullptr);
}

void sew_preferences_row_set_subtitle(SewPreferencesRow *self, char *subtitle)
{
    g_object_set(self, "subtitle", subtitle, nullptr);
}

void sew_preferences_row_set_file(SewPreferencesRow *self, GFile *file)
{
    g_object_set(self, "file", file, nullptr);
}

void
sew_preferences_row_set_as_folder(SewPreferencesRow *self, gboolean as_folder)
{
    g_object_set(self, "as-folder", as_folder, nullptr);
}

void
sew_preferences_row_set_filter(SewPreferencesRow *self, GtkFileFilter *filter)
{
    g_object_set(self, "filter", filter, nullptr);
}
