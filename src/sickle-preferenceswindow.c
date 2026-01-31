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
}

static void sickle_preferences_window_init(SicklePreferencesWindow *self)
{
    gtk_widget_init_template(GTK_WIDGET(self));
    self->settings = g_settings_new(SE_APPLICATION_ID);

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
