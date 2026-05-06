#include "sickle-applicationwindow.h"

#include "config.h"
#include "rmf/rmf.h"
#include "sew/sew.h"

#include <glib-object.h>
#include <gtk/gtk.h>

struct _SickleApplicationWindow {
    GtkApplicationWindow parent_instance;
    // Private
    GBinding *binding_application_view3d_textures;
    // Properties
    RmfRoot *map;
    unsigned int grid_size;
    // Template widgets
    SewViewport3d *view3D;
    GtkListBox *outlinerListBox;
};

G_DEFINE_FINAL_TYPE(
    SickleApplicationWindow,
    sickle_application_window,
    GTK_TYPE_APPLICATION_WINDOW
)

enum {
    PROP_MAP = 1,
    PROP_GRID_SIZE,
    N_PROPERTIES,
};

static GParamSpec *obj_properties[N_PROPERTIES];

// Signal Handlers /////////////////////////////////////////////////////////////

static void on_notify_application(GObject *object, GParamSpec *, gpointer)
{
    SickleApplicationWindow *self = SICKLE_APPLICATION_WINDOW(object);
    if (self->binding_application_view3d_textures) {
        g_binding_unbind(self->binding_application_view3d_textures);
    }
    self->binding_application_view3d_textures = g_object_bind_property(
        gtk_window_get_application(GTK_WINDOW(self)),
        "texture-archives",
        self->view3D,
        "textures",
        G_BINDING_SYNC_CREATE
    );
}

static GtkWidget *create_solid_widget(gpointer item, gpointer store)
{
    guint position = 0;
    g_list_store_find(G_LIST_STORE(store), item, &position);
    g_autofree char *name = g_strdup_printf("solid.%03u", position + 1);
    return gtk_label_new(name);
}

static GtkWidget *create_entity_widget(gpointer item, gpointer)
{
    char const *classname
        = rmf_entity_data_get_classname(RMF_ENTITY_DATA(item));

    GtkWidget *solids = gtk_list_box_new();

    g_autoptr(GListStore) model = g_list_store_new(RMF_TYPE_MAP_OBJECT);
    GListStore *map_objects = rmf_map_object_get_children(RMF_MAP_OBJECT(item));
    RmfMapObject *map_object = nullptr;
    for (guint i = 0; map_objects
         && (map_object = g_list_model_get_item(G_LIST_MODEL(map_objects), i));
         ++i)
    {
        if (RMF_IS_SOLID(map_object)) {
            g_list_store_append(model, map_object);
        }
    }
    gtk_list_box_bind_model(
        GTK_LIST_BOX(solids),
        G_LIST_MODEL(model),
        create_solid_widget,
        model,
        nullptr
    );

    if (g_list_model_get_n_items(G_LIST_MODEL(model)) > 0) {
        GtkWidget *expander = gtk_expander_new(classname);
        gtk_expander_set_child(GTK_EXPANDER(expander), solids);
        return expander;
    } else {
        GtkWidget *label = gtk_label_new(classname);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        // FIXME: Arbitrary margin to line up expanderless and expander-having
        // entity names.
        gtk_widget_set_margin_start(label, 17);
        return label;
    }
}

static int section_sort_func(void const *a, void const *b, void *)
{
    int worldspawn_sort
        = RMF_IS_WORLDSPAWN((void *)b) - RMF_IS_WORLDSPAWN((void *)a);
    if (worldspawn_sort != 0) {
        return worldspawn_sort;
    }
    GListStore *a_children
        = rmf_map_object_get_children(RMF_MAP_OBJECT((void *)a));
    GListStore *b_children
        = rmf_map_object_get_children(RMF_MAP_OBJECT((void *)b));
    guint a_count
        = a_children ? g_list_model_get_n_items(G_LIST_MODEL(a_children)) : 0;
    guint b_count
        = b_children ? g_list_model_get_n_items(G_LIST_MODEL(b_children)) : 0;
    return (b_count > 0) - (a_count > 0);
}

static void on_notify_map(GObject *object, GParamSpec *, gpointer)
{
    SickleApplicationWindow *self = SICKLE_APPLICATION_WINDOW(object);
    g_autoptr(GListStore) model = g_list_store_new(RMF_TYPE_MAP_OBJECT);

    if (self->map) {
        RmfWorldspawn *worldspawn = rmf_root_get_worldspawn(self->map);
        GListStore *map_objects
            = rmf_map_object_get_children(RMF_MAP_OBJECT(worldspawn));
        RmfMapObject *map_object = nullptr;
        g_list_store_append(model, worldspawn);
        for (guint i = 0; map_objects
             && (map_object
                 = g_list_model_get_item(G_LIST_MODEL(map_objects), i));
             ++i)
        {
            if (RMF_IS_ENTITY(map_object)) {
                g_list_store_append(model, map_object);
            }
        }
    }

    GtkExpression *expression = gtk_property_expression_new(
        RMF_TYPE_ENTITY_DATA,
        nullptr,
        "classname"
    );
    GtkStringSorter *sorter = gtk_string_sorter_new(expression);
    GtkCustomSorter *section_sorter
        = gtk_custom_sorter_new(section_sort_func, nullptr, nullptr);
    GtkSortListModel *sort_model
        = gtk_sort_list_model_new(G_LIST_MODEL(model), GTK_SORTER(sorter));
    gtk_sort_list_model_set_section_sorter(
        sort_model,
        GTK_SORTER(section_sorter)
    );

    gtk_list_box_bind_model(
        self->outlinerListBox,
        G_LIST_MODEL(sort_model),
        create_entity_widget,
        nullptr,
        nullptr
    );
}

// GObject /////////////////////////////////////////////////////////////////////

static void sickle_application_window_dispose(GObject *object)
{
    auto self = SICKLE_APPLICATION_WINDOW(object);
    g_clear_object(&self->map);
    g_clear_object(&self->binding_application_view3d_textures);
    gtk_widget_dispose_template(
        GTK_WIDGET(object),
        SICKLE_TYPE_APPLICATION_WINDOW
    );
    G_OBJECT_CLASS(sickle_application_window_parent_class)->dispose(object);
}

static void sickle_application_window_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec
)
{
    SickleApplicationWindow *self = SICKLE_APPLICATION_WINDOW(object);
    switch (property_id) {
    case PROP_MAP:
        g_value_set_object(value, self->map);
        break;
    case PROP_GRID_SIZE:
        g_value_set_uint(value, self->grid_size);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

static void sickle_application_window_set_property(
    GObject *object,
    guint property_id,
    GValue const *value,
    GParamSpec *pspec
)
{
    SickleApplicationWindow *self = SICKLE_APPLICATION_WINDOW(object);
    switch (property_id) {
    case PROP_MAP:
        g_clear_object(&self->map);
        self->map = RMF_ROOT(g_value_dup_object(value));
        break;
    case PROP_GRID_SIZE:
        self->grid_size = g_value_get_uint(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
    }
}

// SickleApplicationWindow /////////////////////////////////////////////////////

static void
sickle_application_window_class_init(SickleApplicationWindowClass *klass)
{
    GObjectClass *oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = sickle_application_window_dispose;
    oclass->get_property = sickle_application_window_get_property;
    oclass->set_property = sickle_application_window_set_property;

    obj_properties[PROP_MAP] = g_param_spec_object(
        "map",
        nullptr,
        nullptr,
        RMF_TYPE_ROOT,
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
    );

    obj_properties[PROP_GRID_SIZE] = g_param_spec_uint(
        "grid-size",
        nullptr,
        nullptr,
        1,
        512,
        32,
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
    );

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    gtk_widget_class_set_template_from_resource(
        widget_class,
        SE_GRESOURCE_PREFIX "ui/sickle-applicationwindow.ui"
    );
    gtk_widget_class_bind_template_child(
        widget_class,
        SickleApplicationWindow,
        view3D
    );
    gtk_widget_class_bind_template_child(
        widget_class,
        SickleApplicationWindow,
        outlinerListBox
    );
}

static void sickle_application_window_init(SickleApplicationWindow *self)
{
    gtk_widget_init_template(GTK_WIDGET(self));
    g_signal_connect(
        self,
        "notify::application",
        G_CALLBACK(on_notify_application),
        nullptr
    );
    g_signal_connect(self, "notify::map", G_CALLBACK(on_notify_map), nullptr);
}

// Public //////////////////////////////////////////////////////////////////////

SickleApplicationWindow *sickle_application_window_new(void)
{
    return g_object_new(
        SICKLE_TYPE_APPLICATION_WINDOW,
        // FIXME: temp
        "show-menubar",
        TRUE,
        nullptr
    );
}

void sickle_application_window_open_blank(SickleApplicationWindow *self)
{
    g_object_set(self, "map", nullptr, nullptr);
}

void sickle_application_window_open(SickleApplicationWindow *self, GFile *file)
{
    if (!file) {
        return;
    }
    g_print("Request to open '%s'\n", g_file_get_parse_name(file));

    auto loader = rmf_loader_new();

    g_autoptr(GError) error = nullptr;
    rmf_loader_load_from_file(loader, file, &error);
    if (error) {
        g_error("%s -- %s", __FUNCTION__, error->message);
    }

    RmfRoot *root = rmf_loader_get_root(loader);

    g_object_set(self, "map", root, nullptr);
}

void sickle_application_window_save(SickleApplicationWindow *, GFile *file)
{
    if (!file) {
        return;
    }
    g_print("Request to save '%s'\n", g_file_get_parse_name(file));
    // TODO
}
