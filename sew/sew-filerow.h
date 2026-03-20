#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>

#if !defined(__SEW_H_INSIDE__) && !defined(SEW_COMPILATION)
#  error "Only <sew/sew.h> can be included directly."
#endif

G_BEGIN_DECLS

#define SEW_TYPE_FILE_ROW sew_file_row_get_type()

G_DECLARE_FINAL_TYPE(SewFileRow, sew_file_row, SEW, FILE_ROW, GtkListBoxRow)

SewFileRow *sew_file_row_new(GFile *file);

GFile *sew_file_row_get_file(SewFileRow *self);
void sew_file_row_set_file(SewFileRow *self, GFile *file);

G_END_DECLS
