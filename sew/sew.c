#include "sew/sew.h"

#include "serg/serg.h"
#include "sew-resources.h"

#include <gdk/gdk.h>
#include <glib-object.h>
#include <gtk/gtkcssprovider.h>

void sew_init(void)
{
    serg_init();

    sew_register_resource();

    g_type_ensure(SEW_TYPE_BUTTON_ROW);
    g_type_ensure(SEW_TYPE_FILE_ROW);
    g_type_ensure(SEW_TYPE_PREFERENCES_ROW);
    g_type_ensure(SEW_TYPE_VIEWPORT_2D);
    g_type_ensure(SEW_TYPE_VIEWPORT_3D);

    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_resource(
        css_provider,
        "/com/github/treecase/sew/style.css"
    );
    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_FALLBACK
    );
}
