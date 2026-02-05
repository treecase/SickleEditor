#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>

#if !defined(__SEW_H_INSIDE__) && !defined(SEW_COMPILATION)
#  error "Only <sew/sew.h> can be included directly."
#endif

G_BEGIN_DECLS

#define SEW_TYPE_VIEWPORT_2D sew_viewport_2d_get_type()
G_DECLARE_FINAL_TYPE(SewViewport2d, sew_viewport_2d, SEW, VIEWPORT_2D, GtkGrid)

G_END_DECLS
