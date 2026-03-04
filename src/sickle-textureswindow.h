#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SICKLE_TYPE_TEXTURES_WINDOW sickle_textures_window_get_type()

G_DECLARE_FINAL_TYPE(
    SickleTexturesWindow,
    sickle_textures_window,
    SICKLE,
    TEXTURES_WINDOW,
    GtkWindow
)

SickleTexturesWindow *sickle_textures_window_new(void);

GPtrArray *sickle_textures_window_get_textures(SickleTexturesWindow *texwin);

void sickle_textures_window_set_textures(
    SickleTexturesWindow *texwin,
    GPtrArray *textures
);

G_END_DECLS
