#include "sickle-preferenceswindow.h"

#include "config.h"
#include "sew/sew.h"

#include <gtk/gtk.h>

struct _SicklePreferencesWindow {
    GtkWindow parent_instance;
    GSettings *settings;
    // Template widgets
    SewPreferencesRow *row_game_def;
    SewPreferencesRow *row_game_root;
    SewPreferencesRow *row_sprite_root;
    GtkListBox *wad_list;
    SewButtonRow *row_add_wad;
    SewButtonRow *row_remove_wad;
};

G_DEFINE_FINAL_TYPE(
    SicklePreferencesWindow,
    sickle_preferences_window,
    GTK_TYPE_WINDOW
)

// Private /////////////////////////////////////////////////////////////////////

static gboolean convert_uri_to_file(GValue *value, GVariant *variant, void *)
{
    char const *uri = g_variant_get_string(variant, nullptr);
    if (g_strcmp0(uri, "") == 0) {
        g_value_set_object(value, nullptr);
        return TRUE;
    }
    GFile *file = g_file_new_for_uri(uri);
    g_value_take_object(value, file);
    return TRUE;
}

static GVariant *convert_file_to_uri(
    GValue const *value,
    GVariantType const *expected_type,
    void *
)
{
    g_return_val_if_fail(G_VALUE_HOLDS_OBJECT(value), nullptr);
    void *object = g_value_get_object(value);
    if (!object) {
        return g_variant_new_string("");
    }
    g_return_val_if_fail(G_IS_FILE(object), nullptr);
    GFile *file = G_FILE(object);
    g_variant_type_equal(expected_type, G_VARIANT_TYPE_STRING);
    char *uri = g_file_get_uri(file);
    return g_variant_new_take_string(uri);
}

// Signal Handlers /////////////////////////////////////////////////////////////

static void
on_wad_paths_changed(GSettings *settings, gchar *key, gpointer user_data)
{
    SicklePreferencesWindow *self = SICKLE_PREFERENCES_WINDOW(user_data);

    GtkListBoxRow *row = nullptr;
    while ((row = gtk_list_box_get_row_at_index(self->wad_list, 2))) {
        gtk_list_box_remove(self->wad_list, GTK_WIDGET(row));
    }

    g_auto(GStrv) paths = g_settings_get_strv(settings, key);
    for (char **path = paths; *path != nullptr; ++path) {
        GFile *file = g_file_new_for_uri(*path);
        SewFileRow *row = sew_file_row_new(file);
        gtk_list_box_append(self->wad_list, GTK_WIDGET(row));
    }
}

static void
on_wad_opened(GObject *source_object, GAsyncResult *res, gpointer data)
{
    GtkFileDialog *file_dialog = GTK_FILE_DIALOG(source_object);
    SicklePreferencesWindow *self = SICKLE_PREFERENCES_WINDOW(data);

    g_autoptr(GError) error = nullptr;
    GFile *file = gtk_file_dialog_open_finish(file_dialog, res, &error);
    if (error) {
        if (error->code != GTK_DIALOG_ERROR_DISMISSED) {
            g_printerr("%s :: %s\n", __FUNCTION__, error->message);
        }
        return;
    }

    g_auto(GStrv) paths = g_settings_get_strv(self->settings, "wad-paths");
    GStrvBuilder *builder = g_strv_builder_new();
    g_strv_builder_addv(builder, (char const **)paths);
    // Don't add duplicates.
    char *uri = g_file_get_uri(file);
    for (char **path = paths; *path != nullptr; ++path) {
        if (g_str_equal(uri, *path)) {
            g_free(uri);
            g_strv_builder_unref(builder);
            return;
        }
    }
    g_strv_builder_take(builder, uri);
    g_auto(GStrv) new_paths = g_strv_builder_unref_to_strv(builder);
    g_settings_set_strv(
        self->settings,
        "wad-paths",
        (char const *const *)new_paths
    );
}

static void on_wad_list_row_activated(
    SicklePreferencesWindow *self,
    GtkListBoxRow *row,
    GtkListBox *wad_list
)
{
    g_return_if_fail(SICKLE_IS_PREFERENCES_WINDOW(self));
    g_return_if_fail(GTK_IS_LIST_BOX_ROW(row));
    g_return_if_fail(GTK_IS_LIST_BOX(wad_list));
    if (row == GTK_LIST_BOX_ROW(self->row_add_wad)) {
        // Add a WAD to the list.
        GtkFileFilter *filter = gtk_file_filter_new();
        gtk_file_filter_set_name(filter, "WAD Archive");
        gtk_file_filter_add_pattern(filter, "*.wad");
        GtkFileDialog *file_dialog = gtk_file_dialog_new();
        gtk_file_dialog_set_default_filter(file_dialog, filter);
        gtk_file_dialog_open(
            file_dialog,
            nullptr,
            nullptr,
            on_wad_opened,
            self
        );
    } else if (row == GTK_LIST_BOX_ROW(self->row_remove_wad)) {
        // Remove a WAD from the list.
        GtkListBoxRow *selected_row = gtk_list_box_get_selected_row(wad_list);
        if (selected_row && SEW_IS_FILE_ROW(selected_row)) {
            g_autofree char *selected_uri = g_file_get_uri(
                sew_file_row_get_file(SEW_FILE_ROW(selected_row))
            );
            g_auto(GStrv) uris
                = g_settings_get_strv(self->settings, "wad-paths");
            GStrvBuilder *builder = g_strv_builder_new();
            for (char **uri = uris; *uri != nullptr; ++uri) {
                if (!g_str_equal(*uri, selected_uri)) {
                    g_strv_builder_add(builder, *uri);
                }
            }
            g_auto(GStrv) new_uris = g_strv_builder_unref_to_strv(builder);
            g_settings_set_strv(
                self->settings,
                "wad-paths",
                (char const *const *)new_uris
            );
        }
    }
}

// GObject /////////////////////////////////////////////////////////////////////

static void sickle_preferences_window_dispose(GObject *object)
{
    SicklePreferencesWindow *self = SICKLE_PREFERENCES_WINDOW(object);
    g_clear_object(&self->settings);
    gtk_widget_dispose_template(
        GTK_WIDGET(object),
        SICKLE_TYPE_PREFERENCES_WINDOW
    );
    G_OBJECT_CLASS(sickle_preferences_window_parent_class)->dispose(object);
}

// SicklePreferencesWindow /////////////////////////////////////////////////////

static void
sickle_preferences_window_class_init(SicklePreferencesWindowClass *klass)
{
    GObjectClass *oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = sickle_preferences_window_dispose;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    gtk_widget_class_set_template_from_resource(
        widget_class,
        SE_GRESOURCE_PREFIX "ui/sickle-preferenceswindow.ui"
    );
    gtk_widget_class_bind_template_child(
        widget_class,
        SicklePreferencesWindow,
        row_game_def
    );
    gtk_widget_class_bind_template_child(
        widget_class,
        SicklePreferencesWindow,
        row_game_root
    );
    gtk_widget_class_bind_template_child(
        widget_class,
        SicklePreferencesWindow,
        row_sprite_root
    );
    gtk_widget_class_bind_template_child(
        widget_class,
        SicklePreferencesWindow,
        wad_list
    );
    gtk_widget_class_bind_template_child(
        widget_class,
        SicklePreferencesWindow,
        row_add_wad
    );
    gtk_widget_class_bind_template_child(
        widget_class,
        SicklePreferencesWindow,
        row_remove_wad
    );
    gtk_widget_class_bind_template_callback(
        widget_class,
        on_wad_list_row_activated
    );
}

static void sickle_preferences_window_init(SicklePreferencesWindow *self)
{
    gtk_widget_init_template(GTK_WIDGET(self));
    self->settings = g_settings_new(SE_APPLICATION_ID);

    g_signal_connect(
        self->settings,
        "changed::wad-paths",
        G_CALLBACK(on_wad_paths_changed),
        self
    );
    on_wad_paths_changed(self->settings, "wad-paths", self);

    g_settings_bind_with_mapping(
        self->settings,
        "fgd-path",
        self->row_game_def,
        "file",
        G_SETTINGS_BIND_DEFAULT,
        convert_uri_to_file,
        convert_file_to_uri,
        nullptr,
        nullptr
    );
    g_settings_bind_with_mapping(
        self->settings,
        "game-root-path",
        self->row_game_root,
        "file",
        G_SETTINGS_BIND_DEFAULT,
        convert_uri_to_file,
        convert_file_to_uri,
        nullptr,
        nullptr
    );
    g_settings_bind_with_mapping(
        self->settings,
        "sprite-root-path",
        self->row_sprite_root,
        "file",
        G_SETTINGS_BIND_DEFAULT,
        convert_uri_to_file,
        convert_file_to_uri,
        nullptr,
        nullptr
    );
}

// Public //////////////////////////////////////////////////////////////////////

SicklePreferencesWindow *sickle_preferences_window_new(void)
{
    return g_object_new(SICKLE_TYPE_PREFERENCES_WINDOW, nullptr);
}
