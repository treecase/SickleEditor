#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>

#if !defined(__SEW_H_INSIDE__) && !defined(SEW_COMPILATION)
#  error "Only <sew/sew.h> can be included directly."
#endif

G_BEGIN_DECLS

#define SEW_TYPE_BUTTON_ROW sew_button_row_get_type()
G_DECLARE_FINAL_TYPE(
    SewButtonRow,
    sew_button_row,
    SEW,
    BUTTON_ROW,
    GtkListBoxRow
)

char const *sew_button_row_get_title(SewButtonRow *self);
void sew_button_row_set_title(SewButtonRow *self, char const *title);

char const *sew_button_row_get_end_icon_name(SewButtonRow *self);
void sew_button_row_set_end_icon_name(SewButtonRow *self, char const *icon_name);

char const *sew_button_row_get_start_icon_name(SewButtonRow *self);
void sew_button_row_set_start_icon_name(SewButtonRow *self, char const *icon_name);

G_END_DECLS