#pragma once

#include <glib-object.h>
#include <gtk/gtk.h>

#if !defined(__SEW_H_INSIDE__) && !defined(SEW_COMPILATION)
#  error "Only <sew/sew.h> can be included directly."
#endif

G_BEGIN_DECLS

#define SEW_TYPE_PREFERENCES_ROW sew_preferences_row_get_type()
G_DECLARE_FINAL_TYPE(
    SewPreferencesRow,
    sew_preferences_row,
    SEW,
    PREFERENCES_ROW,
    GtkListBoxRow
)

SewPreferencesRow *sew_preferences_row_new(void);

char *sew_preferences_row_get_title(SewPreferencesRow *self);
char *sew_preferences_row_get_subtitle(SewPreferencesRow *self);
GFile *sew_preferences_row_get_file(SewPreferencesRow *self);
gboolean sew_preferences_row_get_as_folder(SewPreferencesRow *self);
GtkFileFilter *sew_preferences_row_get_filter(SewPreferencesRow *self);

void sew_preferences_row_set_title(SewPreferencesRow *self, char *title);
void sew_preferences_row_set_subtitle(SewPreferencesRow *self, char *subtitle);
void sew_preferences_row_set_file(SewPreferencesRow *self, GFile *file);
void
sew_preferences_row_set_as_folder(SewPreferencesRow *self, gboolean as_folder);
void
sew_preferences_row_set_filter(SewPreferencesRow *self, GtkFileFilter *filter);

G_END_DECLS
