#include "sew/sew-viewport2d.h"

#include <cairo.h>
#include <glib-object.h>
#include <gtk/gtk.h>

struct _SewViewport2d {
    GtkGrid parent_instance;
    // Template children
    GtkDrawingArea *area_grid;
};

G_DEFINE_FINAL_TYPE(SewViewport2d, sew_viewport_2d, GTK_TYPE_GRID)

// Private /////////////////////////////////////////////////////////////////////

static void draw_func_grid(
    GtkDrawingArea *drawing_area,
    cairo_t *cr,
    int width,
    int height,
    void *
)
{
    constexpr int GRID_SIZE = 16;

    GdkRGBA color;
    gtk_widget_get_color(GTK_WIDGET(drawing_area), &color);
    gdk_cairo_set_source_rgba(cr, &color);
    cairo_set_line_width(cr, 1.0);
    for (int x = GRID_SIZE; x < width; x += GRID_SIZE) {
        cairo_move_to(cr, x, 0.0);
        cairo_line_to(cr, x, height);
    }
    for (int y = GRID_SIZE; y < height; y += GRID_SIZE) {
        cairo_move_to(cr, 0.0, y);
        cairo_line_to(cr, width, y);
    }
    cairo_stroke(cr);
}

// GtkWidget ///////////////////////////////////////////////////////////////////

static void sew_viewport_2d_dispose(GObject *object)
{
    gtk_widget_dispose_template(GTK_WIDGET(object), SEW_TYPE_VIEWPORT_2D);
    G_OBJECT_CLASS(sew_viewport_2d_parent_class)->dispose(object);
}

// SewViewport2d ////////////////////////////////////////////////////////////

static void sew_viewport_2d_class_init(SewViewport2dClass *klass)
{
    GObjectClass *oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = sew_viewport_2d_dispose;

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    gtk_widget_class_set_css_name(widget_class, "viewport2d");
    gtk_widget_class_set_template_from_resource(
        widget_class,
        "/com/github/treecase/sew/ui/sew-viewport2d.ui"
    );
    gtk_widget_class_bind_template_child(
        widget_class,
        SewViewport2d,
        area_grid
    );
}

static void sew_viewport_2d_init(SewViewport2d *self)
{
    gtk_widget_init_template(GTK_WIDGET(self));

    gtk_drawing_area_set_draw_func(
        self->area_grid,
        draw_func_grid,
        self,
        nullptr
    );
}
