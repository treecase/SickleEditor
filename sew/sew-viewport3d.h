#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>

#if !defined(__SEW_H_INSIDE__) && !defined(SEW_COMPILATION)
#  error "Only <sew/sew.h> can be included directly."
#endif

G_BEGIN_DECLS

#define SEW_TYPE_VIEWPORT_3D sew_viewport_3d_get_type()

G_DECLARE_FINAL_TYPE(
    SewViewport3d,
    sew_viewport_3d,
    SEW,
    VIEWPORT_3D,
    GtkGLArea
)

G_END_DECLS
