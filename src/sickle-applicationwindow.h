#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SICKLE_TYPE_APPLICATION_WINDOW sickle_application_window_get_type()
G_DECLARE_FINAL_TYPE(
    SickleApplicationWindow,
    sickle_application_window,
    SICKLE,
    APPLICATION_WINDOW,
    GtkApplicationWindow
)

SickleApplicationWindow *sickle_application_window_new(void);

/** Open a file. */
void
sickle_application_window_open(SickleApplicationWindow *window, GFile *file);
/** Save to a file. */
void
sickle_application_window_save(SickleApplicationWindow *window, GFile *file);

G_END_DECLS
