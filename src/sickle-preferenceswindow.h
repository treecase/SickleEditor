#pragma once

#include <glib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SICKLE_TYPE_PREFERENCES_WINDOW sickle_preferences_window_get_type()
G_DECLARE_FINAL_TYPE(
    SicklePreferencesWindow,
    sickle_preferences_window,
    SICKLE,
    PREFERENCES_WINDOW,
    GtkWindow
)

SicklePreferencesWindow *sickle_preferences_window_new(void);

G_END_DECLS
