#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>

#if !defined(__SEW_H_INSIDE__) && !defined(SEW_COMPILATION)
#  error "Only <sew/sew.h> can be included directly."
#endif

G_BEGIN_DECLS

typedef enum {
    SEW_VIEW_AXIS_TOP,
    SEW_VIEW_AXIS_FRONT,
    SEW_VIEW_AXIS_RIGHT,
} SewViewAxis;

#define SEW_TYPE_VIEW_AXIS sew_view_axis_get_type()
GType sew_view_axis_get_type(void);

#define SEW_TYPE_VIEWPORT_2D sew_viewport_2d_get_type()
G_DECLARE_FINAL_TYPE(
    SewViewport2d,
    sew_viewport_2d,
    SEW,
    VIEWPORT_2D,
    GtkWidget
)

G_END_DECLS
