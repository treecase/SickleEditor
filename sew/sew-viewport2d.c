#include "sew/sew-viewport2d.h"

#include "rmf/rmf.h"

#include <cairo.h>
#include <glib-object.h>
#include <gtk/gtk.h>

struct translate {
    float x, y;
};

struct point {
    double x, y;
};

G_DEFINE_ENUM_TYPE(
    SewViewAxis,
    sew_view_axis,
    G_DEFINE_ENUM_VALUE(SEW_VIEW_AXIS_TOP, "top"),
    G_DEFINE_ENUM_VALUE(SEW_VIEW_AXIS_FRONT, "front"),
    G_DEFINE_ENUM_VALUE(SEW_VIEW_AXIS_RIGHT, "right")
)

struct _SewViewport2d {
    GtkWidget parent_instance;
    struct translate translation, _trans_temp;
    float zoom;
    // Properties
    RmfRoot *map;
    SewViewAxis axis;
    // Template children
    GtkDrawingArea *area_grid;
    GtkDrawingArea *area_brushes;
};

G_DEFINE_FINAL_TYPE(SewViewport2d, sew_viewport_2d, GTK_TYPE_WIDGET)

enum Property {
    PROP_MAP = 1,
    PROP_AXIS,
    N_PROPERTIES,
};

static GParamSpec *obj_properties[N_PROPERTIES];

// Private /////////////////////////////////////////////////////////////////////

// The number of discrete scrolls needed to reach min/max zoom from 1:1.
constexpr double ZOOM_LIMIT = 100.0;

static double clamp(double x, double min, double max)
{
    x = x > max ? max : x;
    x = x < min ? min : x;
    return x;
}

static double zoom(double x)
{
    // Zoom level gets normalized such that:
    //  -ZOOM_LIMIT => 0.0
    //            0 => 0.5
    //  +ZOOM_LIMIT => 1.0
    auto xn = (x + ZOOM_LIMIT) / (2 * ZOOM_LIMIT);
    // This expression maps the zoom level to a zoom factor:
    //  0.0 => 1/16
    //  0.5 => 1
    //  1.0 => 16
    return pow(2, 8 * xn - 4);
}

static struct point map_vertex(SewViewport2d *self, RmfVector const *from)
{
    switch (self->axis) {
    case SEW_VIEW_AXIS_RIGHT:
        return (struct point){
            .x = from->y,
            .y = -from->z,
        };
    case SEW_VIEW_AXIS_FRONT:
        return (struct point){
            .x = from->x,
            .y = -from->z,
        };
    case SEW_VIEW_AXIS_TOP:
        return (struct point){
            .x = from->x,
            .y = from->y,
        };
    }
    g_return_val_if_reached((struct point){});
}

static void queue_draw(SewViewport2d *self)
{
    gtk_widget_queue_draw(GTK_WIDGET(self->area_grid));
    gtk_widget_queue_draw(GTK_WIDGET(self->area_brushes));
}

static void on_scroll(SewViewport2d *self, double, double dy, GtkGestureDrag *)
{
    g_return_if_fail(SEW_IS_VIEWPORT_2D(self));
    self->zoom = clamp(self->zoom - dy, -ZOOM_LIMIT, ZOOM_LIMIT);
    queue_draw(self);
}

static void on_drag_begin(SewViewport2d *self, double, double, GtkGestureDrag *)
{
    g_return_if_fail(SEW_IS_VIEWPORT_2D(self));
    self->_trans_temp = self->translation;
}

static void on_drag_update(
    SewViewport2d *self,
    double offset_x,
    double offset_y,
    GtkGestureDrag *
)
{
    g_return_if_fail(SEW_IS_VIEWPORT_2D(self));
    self->translation.x = self->_trans_temp.x + offset_x / zoom(self->zoom);
    self->translation.y = self->_trans_temp.y + offset_y / zoom(self->zoom);
    queue_draw(self);
}

static void on_drag_end(SewViewport2d *self, double, double, GtkGestureDrag *)
{
    g_return_if_fail(SEW_IS_VIEWPORT_2D(self));
    gtk_widget_queue_draw(GTK_WIDGET(self->area_brushes));
}

static void draw_func_grid(
    GtkDrawingArea *drawing_area,
    cairo_t *cr,
    int width,
    int height,
    SewViewport2d *self
)
{
    // TODO: zoom
    g_return_if_fail(SEW_IS_VIEWPORT_2D(self));

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

static void
draw_func_brushes(GtkDrawingArea *, cairo_t *cr, int, int, SewViewport2d *self)
{
    g_return_if_fail(SEW_IS_VIEWPORT_2D(self));
    if (!self->map) {
        return;
    }

    auto worldspawn = rmf_root_get_worldspawn(self->map);

    g_autoptr(RmfMapObjectIterator) it
        = rmf_map_object_get_children(RMF_MAP_OBJECT(worldspawn));
    RMF_ITERATOR_FOREACH(RmfMapObject, child, it)
    {
        if (RMF_IS_SOLID(child)) {
            auto color = rmf_map_object_get_color(child);
            cairo_set_source_rgb(
                cr,
                color.r / 255.0,
                color.g / 255.0,
                color.b / 255.0
            );
            cairo_set_line_width(cr, 1.0);
            g_autoptr(RmfFaceIterator) faces
                = rmf_solid_get_faces(RMF_SOLID(child));
            RMF_ITERATOR_FOREACH(RmfFace, face, faces)
            {
                for (size_t i = 0; i < face->vertices->len; ++i) {
                    auto vertex = &((RmfVector *)face->vertices->data)[i];
                    auto point = map_vertex(self, vertex);
                    if (i == 0) {
                        cairo_move_to(
                            cr,
                            (self->translation.x + point.x) * zoom(self->zoom),
                            (self->translation.y + point.y) * zoom(self->zoom)
                        );
                    } else {
                        cairo_line_to(
                            cr,
                            (self->translation.x + point.x) * zoom(self->zoom),
                            (self->translation.y + point.y) * zoom(self->zoom)
                        );
                    }
                }
            }
            cairo_stroke(cr);
        }
    }
}

// GtkWidget ///////////////////////////////////////////////////////////////////

static void sew_viewport_2d_dispose(GObject *object)
{
    auto self = SEW_VIEWPORT_2D(object);
    g_clear_object(&self->map);
    gtk_widget_dispose_template(GTK_WIDGET(object), SEW_TYPE_VIEWPORT_2D);

    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(GTK_WIDGET(object)))) {
        gtk_widget_unparent(child);
    }

    G_OBJECT_CLASS(sew_viewport_2d_parent_class)->dispose(object);
}

static void sew_viewport_2d_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec
)
{
    auto self = SEW_VIEWPORT_2D(object);
    switch ((enum Property)property_id) {
    case PROP_MAP:
        g_value_set_object(value, self->map);
        break;
    case PROP_AXIS:
        g_value_set_enum(value, self->axis);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void sew_viewport_2d_set_property(
    GObject *object,
    guint property_id,
    GValue const *value,
    GParamSpec *pspec
)
{
    auto self = SEW_VIEWPORT_2D(object);
    switch ((enum Property)property_id) {
    case PROP_MAP:
        g_clear_object(&self->map);
        self->map = RMF_ROOT(g_value_dup_object(value));
        break;
    case PROP_AXIS:
        self->axis = g_value_get_enum(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

// GtkWidget ///////////////////////////////////////////////////////////////////

static void sew_viewport_2d_compute_expand(
    GtkWidget *widget,
    gboolean *hexpand_p,
    gboolean *vexpand_p
)
{
    GtkWidget *w;
    gboolean hexpand = FALSE;
    gboolean vexpand = FALSE;

    for (w = gtk_widget_get_first_child(widget); w != NULL;
         w = gtk_widget_get_next_sibling(w))
    {
        hexpand = hexpand
            || gtk_widget_compute_expand(w, GTK_ORIENTATION_HORIZONTAL);
        vexpand
            = vexpand || gtk_widget_compute_expand(w, GTK_ORIENTATION_VERTICAL);
    }

    *hexpand_p = hexpand;
    *vexpand_p = vexpand;
}

GtkSizeRequestMode sew_viewport_2d_get_request_mode(GtkWidget *widget)
{
    GtkWidget *w;
    int wfh = 0, hfw = 0;

    for (w = gtk_widget_get_first_child(widget); w != NULL;
         w = gtk_widget_get_next_sibling(w))
    {
        GtkSizeRequestMode mode = gtk_widget_get_request_mode(w);

        switch (mode) {
        case GTK_SIZE_REQUEST_HEIGHT_FOR_WIDTH:
            hfw++;
            break;
        case GTK_SIZE_REQUEST_WIDTH_FOR_HEIGHT:
            wfh++;
            break;
        case GTK_SIZE_REQUEST_CONSTANT_SIZE:
        default:
            break;
        }
    }

    if (hfw == 0 && wfh == 0) {
        return GTK_SIZE_REQUEST_CONSTANT_SIZE;
    } else {
        return wfh > hfw ? GTK_SIZE_REQUEST_WIDTH_FOR_HEIGHT
                         : GTK_SIZE_REQUEST_HEIGHT_FOR_WIDTH;
    }
}

// SewViewport2d ////////////////////////////////////////////////////////////

static void sew_viewport_2d_class_init(SewViewport2dClass *klass)
{
    GObjectClass *oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = sew_viewport_2d_dispose;
    oclass->get_property = sew_viewport_2d_get_property;
    oclass->set_property = sew_viewport_2d_set_property;

    obj_properties[PROP_MAP] = g_param_spec_object(
        "map",
        nullptr,
        nullptr,
        RMF_TYPE_ROOT,
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
    );

    obj_properties[PROP_AXIS] = g_param_spec_enum(
        "axis",
        nullptr,
        nullptr,
        SEW_TYPE_VIEW_AXIS,
        SEW_VIEW_AXIS_TOP,
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
    );

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    // widget_class->focus = gtk_widget_focus_child;
    widget_class->compute_expand = sew_viewport_2d_compute_expand;
    widget_class->get_request_mode = sew_viewport_2d_get_request_mode;

    gtk_widget_class_set_css_name(widget_class, "viewport2d");
    gtk_widget_class_set_layout_manager_type(widget_class, GTK_TYPE_BIN_LAYOUT);
    gtk_widget_class_set_template_from_resource(
        widget_class,
        "/com/github/treecase/sew/ui/sew-viewport2d.ui"
    );
    gtk_widget_class_bind_template_callback(widget_class, on_scroll);
    gtk_widget_class_bind_template_callback(widget_class, on_drag_begin);
    gtk_widget_class_bind_template_callback(widget_class, on_drag_update);
    gtk_widget_class_bind_template_callback(widget_class, on_drag_end);
    gtk_widget_class_bind_template_child(
        widget_class,
        SewViewport2d,
        area_grid
    );
    gtk_widget_class_bind_template_child(
        widget_class,
        SewViewport2d,
        area_brushes
    );
}

static void sew_viewport_2d_init(SewViewport2d *self)
{
    gtk_widget_init_template(GTK_WIDGET(self));

    gtk_drawing_area_set_draw_func(
        self->area_grid,
        (GtkDrawingAreaDrawFunc)draw_func_grid,
        self,
        nullptr
    );

    gtk_drawing_area_set_draw_func(
        self->area_brushes,
        (GtkDrawingAreaDrawFunc)draw_func_brushes,
        self,
        nullptr
    );
}
