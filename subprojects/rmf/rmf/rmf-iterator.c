#include "rmf-iterator.h"

#include <glib-object.h>
#include <stdint.h>

#include "rmf-loader.h"

struct _RmfIterator {
    GObject parent_instance;
    RmfLoader *loader;
    rmf_int count;
    RmfIteratorReadFunc read_func;
};

enum RmfIteratorProperty {
    PROP_LOADER = 1,
    PROP_COUNT,
    PROP_READ_FUNC,
    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {nullptr};

G_DEFINE_FINAL_TYPE(RmfIterator, rmf_iterator, G_TYPE_OBJECT)

// GObject /////////////////////////////////////////////////////////////////////

static void rmf_iterator_dispose(GObject *object)
{
    auto const self = RMF_ITERATOR(object);
    g_clear_object(&self->loader);
    self->count = 0;
    G_OBJECT_CLASS(rmf_iterator_parent_class)->dispose(object);
}

static void rmf_iterator_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec)
{
    auto const self = RMF_ITERATOR(object);
    switch ((enum RmfIteratorProperty)property_id) {
    case PROP_COUNT:
        g_value_set_uint(value, self->count);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void rmf_iterator_set_property(
    GObject *object,
    guint property_id,
    GValue const *value,
    GParamSpec *pspec)
{
    auto const self = RMF_ITERATOR(object);
    switch ((enum RmfIteratorProperty)property_id) {
    case PROP_LOADER:
        g_clear_object(&self->loader);
        self->loader = rmf_loader_dup(RMF_LOADER(g_value_get_object(value)));
        break;
    case PROP_COUNT:
        self->count = g_value_get_uint(value);
        break;
    case PROP_READ_FUNC:
        self->read_func = (RmfIteratorReadFunc)g_value_get_pointer(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

// Boilerplate /////////////////////////////////////////////////////////////////

static void rmf_iterator_class_init(RmfIteratorClass *klass)
{
    auto const oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = rmf_iterator_dispose;
    oclass->get_property = rmf_iterator_get_property;
    oclass->set_property = rmf_iterator_set_property;

    obj_properties[PROP_LOADER] = g_param_spec_object(
        "loader",
        "Loader",
        "RMF file loader.",
        RMF_TYPE_LOADER,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);

    obj_properties[PROP_COUNT] = g_param_spec_uint(
        "count",
        "Count",
        "Count.",
        0,
        UINT32_MAX,
        0,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

    obj_properties[PROP_READ_FUNC] = g_param_spec_pointer(
        "read-func",
        "Read function",
        "Data reading function.",
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);
}

static void rmf_iterator_init(RmfIterator *)
{
}

// Public //////////////////////////////////////////////////////////////////////

RmfIterator *rmf_iterator_new(
    RmfLoader *loader,
    rmf_int count,
    RmfIteratorReadFunc read_func)
{
    g_return_val_if_fail(loader, nullptr);
    g_return_val_if_fail(read_func, nullptr);
    return g_object_new(
        RMF_TYPE_ITERATOR,
        "loader",
        loader,
        "count",
        count,
        "read-func",
        read_func,
        nullptr);
}

void *rmf_iterator_next(RmfIterator *self)
{
    if (self && self->count > 0 && self->read_func) {
        self->count--;
        return self->read_func(self->loader);
    }
    return nullptr;
}
