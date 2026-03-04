#include "sickle-textureswindow.h"

#include "config.h"
#include "sew/sew.h"

#include <gtk/gtk.h>
#include <wad/wad.h>

struct _SickleTexturesWindow {
    GtkWindow parent_instance;
    // Properties
    GPtrArray *textures;
    // Template widgets
    GtkSearchEntry *search;
    GtkFlowBox *flowbox;
};

G_DEFINE_FINAL_TYPE(
    SickleTexturesWindow,
    sickle_textures_window,
    GTK_TYPE_WINDOW
)

enum Property {
    PROP_TEXTURES = 1,
    N_PROPERTIES,
};

static GParamSpec *obj_properties[N_PROPERTIES];

// Private /////////////////////////////////////////////////////////////////////

static gboolean
flow_box_filter_func(GtkFlowBoxChild *child, gpointer search_term)
{
    GtkWidget *box = gtk_flow_box_child_get_child(child);
    GtkWidget *label = gtk_widget_get_last_child(box);
    char const *text = gtk_label_get_label(GTK_LABEL(label));
    g_autofree char *haystack = g_ascii_strup(text, -1);
    g_autofree char *needle = g_ascii_strup(search_term, -1);
    return g_strstr_len(haystack, -1, needle) != nullptr;
}

static int
flow_box_sort_func(GtkFlowBoxChild *child1, GtkFlowBoxChild *child2, gpointer)
{
    GtkWidget *box1 = gtk_flow_box_child_get_child(child1);
    GtkWidget *box2 = gtk_flow_box_child_get_child(child2);
    GtkWidget *label1 = gtk_widget_get_last_child(box1);
    GtkWidget *label2 = gtk_widget_get_last_child(box2);
    char const *text1 = gtk_label_get_label(GTK_LABEL(label1));
    char const *text2 = gtk_label_get_label(GTK_LABEL(label2));
    return g_strcmp0(text1, text2);
}

static void add_textures_from_archive(
    SickleTexturesWindow *self,
    WadTextureArchive *archive
)
{
    g_autofree char const **names = wad_texture_archive_get_names(archive);
    for (char const **name = names; *name != nullptr; ++name) {
        GValue const *value = wad_texture_archive_get_texture(archive, *name);

        g_autoptr(GdkPaintable) paintable = nullptr;
        if (G_VALUE_TYPE(value) == WAD_TYPE_MIPTEX_FILE) {
            WadMiptexFile const *miptex = g_value_get_boxed(value);
            paintable = sew_make_paintable_from_miptex(miptex);
        } else if (G_VALUE_TYPE(value) == WAD_TYPE_QPIC_FILE) {
            WadQpicFile const *qpic = g_value_get_boxed(value);
            paintable = sew_make_paintable_from_qpic(qpic);
        }

        if (paintable) {
            GtkWidget *picture = gtk_picture_new_for_paintable(paintable);
            gtk_picture_set_can_shrink(GTK_PICTURE(picture), FALSE);
            gtk_widget_set_halign(picture, GTK_ALIGN_CENTER);
            gtk_widget_set_valign(picture, GTK_ALIGN_CENTER);

            GtkWidget *label = gtk_label_new(*name);

            GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
            gtk_box_append(GTK_BOX(box), picture);
            gtk_box_append(GTK_BOX(box), label);

            gtk_flow_box_append(self->flowbox, box);
        }
    }
}

// Signal Handlers /////////////////////////////////////////////////////////////

static void on_notify_textures(GObject *object, GParamSpec *, gpointer)
{
    SickleTexturesWindow *self = SICKLE_TEXTURES_WINDOW(object);
    for (size_t i = 0; i < self->textures->len; ++i) {
        WadTextureArchive *archive = self->textures->pdata[i];
        add_textures_from_archive(self, archive);
    }
}

void on_search_search_changed(
    SickleTexturesWindow *self,
    GtkSearchEntry *search_entry
)
{
    char *search_text
        = g_strdup(gtk_editable_get_text(GTK_EDITABLE(search_entry)));
    if (search_text != nullptr) {
        gtk_flow_box_set_filter_func(
            self->flowbox,
            flow_box_filter_func,
            search_text,
            g_free
        );
    } else {
        gtk_flow_box_set_filter_func(self->flowbox, nullptr, nullptr, nullptr);
    }
}

// GObject /////////////////////////////////////////////////////////////////////

static void sickle_textures_window_dispose(GObject *object)
{
    SickleTexturesWindow *self = SICKLE_TEXTURES_WINDOW(object);
    if (self->textures) {
        g_ptr_array_unref(self->textures);
    }
    gtk_widget_dispose_template(
        GTK_WIDGET(object),
        SICKLE_TYPE_TEXTURES_WINDOW
    );
    G_OBJECT_CLASS(sickle_textures_window_parent_class)->dispose(object);
}

static void sickle_textures_window_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec
)
{
    SickleTexturesWindow *self = SICKLE_TEXTURES_WINDOW(object);
    switch ((enum Property)property_id) {
    case PROP_TEXTURES:
        g_value_set_boxed(value, self->textures);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void sickle_textures_window_set_property(
    GObject *object,
    guint property_id,
    GValue const *value,
    GParamSpec *pspec
)
{
    SickleTexturesWindow *self = SICKLE_TEXTURES_WINDOW(object);
    switch ((enum Property)property_id) {
    case PROP_TEXTURES:
        if (self->textures) {
            g_ptr_array_unref(self->textures);
        }
        self->textures = g_value_dup_boxed(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

// SickleTexturesWindow ////////////////////////////////////////////////////////

static void sickle_textures_window_class_init(SickleTexturesWindowClass *klass)
{
    GObjectClass *oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = sickle_textures_window_dispose;
    oclass->get_property = sickle_textures_window_get_property;
    oclass->set_property = sickle_textures_window_set_property;

    obj_properties[PROP_TEXTURES] = g_param_spec_boxed(
        "textures",
        nullptr,
        nullptr,
        G_TYPE_PTR_ARRAY,
        G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS
    );

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    gtk_widget_class_set_template_from_resource(
        widget_class,
        SE_GRESOURCE_PREFIX "ui/sickle-textureswindow.ui"
    );
    gtk_widget_class_bind_template_child(
        widget_class,
        SickleTexturesWindow,
        search
    );
    gtk_widget_class_bind_template_child(
        widget_class,
        SickleTexturesWindow,
        flowbox
    );
    gtk_widget_class_bind_template_callback(
        widget_class,
        on_search_search_changed
    );
}

static void sickle_textures_window_init(SickleTexturesWindow *self)
{
    g_signal_connect(
        self,
        "notify::textures",
        G_CALLBACK(on_notify_textures),
        nullptr
    );
    gtk_widget_init_template(GTK_WIDGET(self));
    gtk_flow_box_set_sort_func(
        self->flowbox,
        flow_box_sort_func,
        nullptr,
        nullptr
    );
}

// Public //////////////////////////////////////////////////////////////////////

SickleTexturesWindow *sickle_textures_window_new(void)
{
    return g_object_new(SICKLE_TYPE_TEXTURES_WINDOW, nullptr);
}

GPtrArray *sickle_textures_window_get_textures(SickleTexturesWindow *texwin)
{
    GPtrArray *textures = nullptr;
    g_object_get(texwin, "textures", &textures, nullptr);
    return textures;
}

void sickle_textures_window_set_textures(
    SickleTexturesWindow *texwin,
    GPtrArray *textures
)
{
    g_object_set(texwin, "textures", textures, nullptr);
}
