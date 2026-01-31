#pragma once

#include <glib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SICKLE_TYPE_APPLICATION sickle_application_get_type()
G_DECLARE_FINAL_TYPE(
    SickleApplication,
    sickle_application,
    SICKLE,
    APPLICATION,
    GtkApplication
)

SickleApplication *sickle_application_new(void);

G_END_DECLS
