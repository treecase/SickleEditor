#include "sickle-application.h"

#include "config.h"
#include "sew/sew.h"
#include "sickle-applicationwindow.h"
#include "sickle-preferenceswindow.h"

#include <gio/gio.h>
#include <glib-object.h>
#include <glib.h>
#include <glibconfig.h>
#include <gtk/gtk.h>
#include <stddef.h>

struct _SickleApplication {
    GtkApplication parent_instance;
    char *game_definition;
    char *game_root;
    char *sprite_root;
    GStrv wads;
};

G_DEFINE_FINAL_TYPE(SickleApplication, sickle_application, GTK_TYPE_APPLICATION)

enum {
    PROP_GAME_DEFINITION = 1,
    PROP_GAME_ROOT,
    PROP_SPRITE_ROOT,
    PROP_WADS,
    N_PROPERTIES,
};

static GParamSpec *obj_properties[N_PROPERTIES] = {};

// Signal Handlers /////////////////////////////////////////////////////////////

// helper for `action_open`
static void on_file_dialog_open_finished(
    GObject *source_object,
    GAsyncResult *res,
    gpointer data
)
{
    g_autoptr(GError) error = nullptr;
    g_autoptr(GFile) file = gtk_file_dialog_open_finish(
        GTK_FILE_DIALOG(source_object),
        res,
        &error
    );
    if (error && error->code != GTK_DIALOG_ERROR_DISMISSED) {
        g_error("%s: %s\n", __FUNCTION__, error->message);
    }
    GtkApplication *application = GTK_APPLICATION(data);
    GtkWindow *window = gtk_application_get_active_window(application);
    sickle_application_window_open(SICKLE_APPLICATION_WINDOW(window), file);
}

// helper for `action_save`
static void on_file_dialog_save_finished(
    GObject *source_object,
    GAsyncResult *res,
    gpointer data
)
{
    g_autoptr(GError) error = nullptr;
    g_autoptr(GFile) file = gtk_file_dialog_save_finish(
        GTK_FILE_DIALOG(source_object),
        res,
        &error
    );
    if (error && error->code != GTK_DIALOG_ERROR_DISMISSED) {
        g_error("%s: %s\n", __FUNCTION__, error->message);
    }
    GtkApplication *application = GTK_APPLICATION(data);
    GtkWindow *window = gtk_application_get_active_window(application);
    sickle_application_window_save(SICKLE_APPLICATION_WINDOW(window), file);
}

static void on_notify_wad_paths(GObject *object, GParamSpec *, gpointer)
{
    SickleApplication *self = SICKLE_APPLICATION(object);
    for (char **wad = self->wads; *wad != nullptr; ++wad) {
        // TODO: Load the WADs
    }
}

// Actions /////////////////////////////////////////////////////////////////////

static void action_new(GSimpleAction *, GVariant *, gpointer user_data)
{
    GtkWindow *window
        = gtk_application_get_active_window(GTK_APPLICATION(user_data));
    sickle_application_window_open(SICKLE_APPLICATION_WINDOW(window), nullptr);
}

static void action_open(GSimpleAction *, GVariant *, gpointer user_data)
{
    SickleApplication *self = SICKLE_APPLICATION(user_data);

    GtkWindow *window
        = gtk_application_get_active_window(GTK_APPLICATION(self));

    g_autoptr(GtkFileDialog) chooser = gtk_file_dialog_new();

    g_autoptr(GtkFileFilter) all_filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(all_filter, "*.*");
    gtk_file_filter_set_name(all_filter, "All Files");

    g_autoptr(GtkFileFilter) map_filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(map_filter, "*.map");
    gtk_file_filter_set_name(map_filter, "Game Maps");

    g_autoptr(GtkFileFilter) rmf_filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(rmf_filter, "*.rmf");
    gtk_file_filter_set_name(rmf_filter, "Hammer/Worldcraft Maps");

    g_autoptr(GListStore) filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    g_list_store_append(filters, all_filter);
    g_list_store_append(filters, map_filter);
    g_list_store_append(filters, rmf_filter);

    gtk_file_dialog_set_filters(chooser, G_LIST_MODEL(filters));
    gtk_file_dialog_set_default_filter(chooser, rmf_filter);

    gtk_file_dialog_open(
        chooser,
        window,
        nullptr,
        on_file_dialog_open_finished,
        self
    );
}

static void action_save(GSimpleAction *, GVariant *, gpointer user_data)
{
    SickleApplication *self = SICKLE_APPLICATION(user_data);

    GtkWindow *window
        = gtk_application_get_active_window(GTK_APPLICATION(self));

    g_autoptr(GtkFileDialog) chooser = gtk_file_dialog_new();

    g_autoptr(GtkFileFilter) all_filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(all_filter, "*.*");
    gtk_file_filter_set_name(all_filter, "All Files");

    g_autoptr(GtkFileFilter) map_filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(map_filter, "*.map");
    gtk_file_filter_set_name(map_filter, "Game Maps");

    g_autoptr(GListStore) filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    g_list_store_append(filters, all_filter);
    g_list_store_append(filters, map_filter);

    gtk_file_dialog_set_filters(chooser, G_LIST_MODEL(filters));
    gtk_file_dialog_set_default_filter(chooser, map_filter);

    gtk_file_dialog_save(
        chooser,
        window,
        nullptr,
        on_file_dialog_save_finished,
        self
    );
}

// helper for `action_exit`
static void dothing(void *w, void *)
{
    gtk_window_close(GTK_WINDOW(w));
}

static void action_exit(GSimpleAction *, GVariant *, gpointer user_data)
{
    GList *windows = gtk_application_get_windows(GTK_APPLICATION(user_data));
    g_list_foreach(windows, dothing, nullptr);
}

static void action_preferences(GSimpleAction *, GVariant *, gpointer user_data)
{
    SicklePreferencesWindow *preferences = sickle_preferences_window_new();
    gtk_window_set_transient_for(
        GTK_WINDOW(preferences),
        gtk_application_get_active_window(GTK_APPLICATION(user_data))
    );
    gtk_window_present(GTK_WINDOW(preferences));
}

static void action_about(GSimpleAction *, GVariant *, gpointer user_data)
{
    g_autoptr(GtkBuilder) builder = gtk_builder_new_from_resource(
        SE_GRESOURCE_PREFIX "ui/sickle-about.ui"
    );
    GtkAboutDialog *about
        = GTK_ABOUT_DIALOG(gtk_builder_get_object(builder, "about"));
    gtk_about_dialog_set_version(about, SE_VERSION);
    gtk_window_set_modal(GTK_WINDOW(about), TRUE);
    gtk_window_set_transient_for(
        GTK_WINDOW(about),
        gtk_application_get_active_window(GTK_APPLICATION(user_data))
    );
    gtk_window_present(GTK_WINDOW(about));
}

// clang-format off
static GActionEntry APP_ENTRIES[] = {
    {.name = "new",         .activate = action_new},
    {.name = "open",        .activate = action_open},
    {.name = "save",        .activate = action_save},
    {.name = "exit",        .activate = action_exit},
    {.name = "preferences", .activate = action_preferences},
    {.name = "about",       .activate = action_about},
};

static struct {
    char const *action;
    char const *accel;
} ACCELS[] = {
    {.action = "app.new",         .accel = "<Primary>n"},
    {.action = "app.open",        .accel = "<Primary>o"},
    {.action = "app.save",        .accel = "<Primary>s"},
    {.action = "app.exit",        .accel = "<Primary>q"},
    {.action = "app.preferences", .accel = "<Primary>comma"},
}; // clang-format on

// GObject /////////////////////////////////////////////////////////////////////

static void sickle_application_finalize(GObject *object)
{
    SickleApplication *self = SICKLE_APPLICATION(object);
    g_free(self->game_definition);
    g_free(self->game_root);
    g_free(self->sprite_root);
    g_strfreev(self->wads);
    G_OBJECT_CLASS(sickle_application_parent_class)->finalize(object);
}

static void sickle_application_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec
)
{
    SickleApplication *const self = SICKLE_APPLICATION(object);
    switch (property_id) {
    case PROP_GAME_DEFINITION:
        g_value_set_string(value, self->game_definition);
        break;
    case PROP_GAME_ROOT:
        g_value_set_string(value, self->game_root);
        break;
    case PROP_SPRITE_ROOT:
        g_value_set_string(value, self->sprite_root);
        break;
    case PROP_WADS:
        g_value_set_boxed(value, self->wads);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

static void sickle_application_set_property(
    GObject *object,
    guint property_id,
    GValue const *value,
    GParamSpec *pspec
)
{
    SickleApplication *const self = SICKLE_APPLICATION(object);
    switch (property_id) {
    case PROP_GAME_DEFINITION:
        g_free(self->game_definition);
        self->game_definition = g_value_dup_string(value);
        break;
    case PROP_GAME_ROOT:
        g_free(self->game_root);
        self->game_root = g_value_dup_string(value);
        break;
    case PROP_SPRITE_ROOT:
        g_free(self->sprite_root);
        self->sprite_root = g_value_dup_string(value);
        break;
    case PROP_WADS:
        g_strfreev(self->wads);
        self->wads = g_value_dup_boxed(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

// GApplication ////////////////////////////////////////////////////////////////

static void sickle_application_activate(GApplication *application)
{
    G_APPLICATION_CLASS(sickle_application_parent_class)->activate(application);

    SickleApplicationWindow *window = sickle_application_window_new();
    gtk_application_add_window(
        GTK_APPLICATION(application),
        GTK_WINDOW(window)
    );
    gtk_window_present(GTK_WINDOW(window));
}

static void sickle_application_open(
    GApplication *application,
    GFile **files,
    gint n_files,
    gchar const *hint
)
{
    G_APPLICATION_CLASS(sickle_application_parent_class)
        ->open(application, files, n_files, hint);

    GtkWindow *window
        = gtk_application_get_active_window(GTK_APPLICATION(application));

    SickleApplicationWindow *appwin = nullptr;
    if (window) {
        appwin = SICKLE_APPLICATION_WINDOW(window);
    } else {
        appwin = sickle_application_window_new();
        gtk_application_add_window(
            GTK_APPLICATION(application),
            GTK_WINDOW(appwin)
        );
    }
    sickle_application_window_open(appwin, files[0]);
    gtk_window_present(GTK_WINDOW(appwin));
}

static void sickle_application_startup(GApplication *application)
{
    G_APPLICATION_CLASS(sickle_application_parent_class)->startup(application);

    // Init SEW widgets.
    sew_init();

    // Add the global stylesheet.
    GdkDisplay *display = gdk_display_get_default();
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_resource(
        css_provider,
        SE_GRESOURCE_PREFIX "style.css"
    );
    gtk_style_context_add_provider_for_display(
        display,
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    g_action_map_add_action_entries(
        G_ACTION_MAP(application),
        APP_ENTRIES,
        G_N_ELEMENTS(APP_ENTRIES),
        application
    );

    for (size_t i = 0; i < G_N_ELEMENTS(ACCELS); ++i) {
        char const *const accels[] = {ACCELS[i].accel, nullptr};
        gtk_application_set_accels_for_action(
            GTK_APPLICATION(application),
            ACCELS[i].action,
            accels
        );
    }
}

// SickleApplication ///////////////////////////////////////////////////////////

static void sickle_application_class_init(SickleApplicationClass *klass)
{
    GObjectClass *oclass = G_OBJECT_CLASS(klass);
    oclass->finalize = sickle_application_finalize;
    oclass->get_property = sickle_application_get_property;
    oclass->set_property = sickle_application_set_property;

    obj_properties[PROP_GAME_DEFINITION] = g_param_spec_string(
        "fgd-path",
        nullptr,
        nullptr,
        "",
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
    );

    obj_properties[PROP_GAME_ROOT] = g_param_spec_string(
        "game-root-path",
        nullptr,
        nullptr,
        "",
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
    );

    obj_properties[PROP_SPRITE_ROOT] = g_param_spec_string(
        "sprite-root-path",
        nullptr,
        nullptr,
        "",
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
    );

    obj_properties[PROP_WADS] = g_param_spec_boxed(
        "wad-paths",
        nullptr,
        nullptr,
        G_TYPE_STRV,
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
    );

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);

    GApplicationClass *gappclass = G_APPLICATION_CLASS(klass);
    gappclass->activate = sickle_application_activate;
    gappclass->open = sickle_application_open;
    gappclass->startup = sickle_application_startup;
}

static void sickle_application_init(SickleApplication *self)
{
    g_signal_connect(
        self,
        "notify::wad-paths",
        G_CALLBACK(on_notify_wad_paths),
        nullptr
    );
    g_autoptr(GSettings) settings = g_settings_new(SE_APPLICATION_ID);
    g_settings_bind(
        settings,
        "wad-paths",
        self,
        "wad-paths",
        G_SETTINGS_BIND_GET
    );
}

// Public //////////////////////////////////////////////////////////////////////

SickleApplication *sickle_application_new(void)
{
    return g_object_new(
        SICKLE_TYPE_APPLICATION,
        "application-id",
        SE_APPLICATION_ID,
        "flags",
        G_APPLICATION_HANDLES_OPEN,
        nullptr
    );
}

#if 0
Application::Application()
: Glib::ObjectBase{typeid(Application)}
, Gtk::Application{SE_APPLICATION_ID, Gio::Application::Flags::HANDLES_OPEN}
, _settings{Gio::Settings::create(SE_APPLICATION_ID)}
, _prop_fgd_path{*this, "fgd-path"}
, _prop_game_root_path{*this, "game-root-path"}
, _prop_sprite_root_path{*this, "sprite-root-path"}
, _prop_wad_paths{*this, "wad-paths"}
{
    sew_init();
    sickle_register_resource();
    sickle_init_public_types();

    property_fgd_path().signal_changed().connect(
        sigc::mem_fun(*this, &Application::_on_fgd_path_changed)
    );
    property_game_root_path().signal_changed().connect(
        sigc::mem_fun(*this, &Application::_on_game_root_path_changed)
    );
    property_sprite_root_path().signal_changed().connect(
        sigc::mem_fun(*this, &Application::_on_sprite_root_path_changed)
    );
    property_wad_paths().signal_changed().connect(
        sigc::mem_fun(*this, &Application::_on_wad_paths_changed)
    );

    _settings->bind("fgd-path", property_fgd_path());
    _settings->bind("game-root-path", property_game_root_path());
    _settings->bind("sprite-root-path", property_sprite_root_path());
    _settings->bind("wad-paths", property_wad_paths());
}

// Actions /////////////////////////////////////////////////////////////////////

void Application::_sync_wadpaths()
{
#  if 0
    auto &texman = Sickle::Editor::Textures::TextureManager::get_reference();

    auto const utf8paths = property_wad_paths().get_value();
    std::unordered_set<std::filesystem::path> paths{};
    std::transform(
        utf8paths.cbegin(),
        utf8paths.cend(),
        std::inserter(paths, paths.begin()),
        Glib::filename_from_utf8);

    // Remove removed WADs.
    for (auto const &wad_path : texman.get_wad_paths())
    {
        if (!paths.count(wad_path))
        {
            texman.remove_wad(wad_path);
        }
    }

    // Add new WADs.
    for (auto const &path : paths)
    {
        texman.add_wad(path);
    }
#  endif
}

void Application::_on_fgd_path_changed()
{
    auto const path = property_fgd_path().get_value();
    if (!path.empty()) {
        _game_definition = FGD::from_file(Glib::filename_from_utf8(path));
        auto &games = Editor::GameDefinition::instance();
        games.add_game(_game_definition);
    }
}

void Application::_on_game_root_path_changed()
{
    auto const path = property_game_root_path().get_value();
    World3D::PointEntitySprite::game_root_path = Glib::filename_from_utf8(path);
}

void Application::_on_sprite_root_path_changed()
{
    auto const path = property_sprite_root_path().get_value();
    World3D::PointEntitySprite::sprite_root_path = Glib::filename_from_utf8(path);
}

void Application::_on_wad_paths_changed()
{
    _sync_wadpaths();
}
#endif
