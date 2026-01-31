#include "rmf-worldspawn.h"

#include <glib-object.h>

#include "rmf-entitydata.h"
#include "rmf-mapobject.h"
#include "rmf-private.h"
#include "rmf-types.h"

struct _RmfWorldspawn {
    RmfEntityData parent_instance;
    rmf_int n_paths;
    goffset addr_paths;
};

enum Property {
    PROP_N_PATHS = 1,
    PROP_PATHS,
    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {nullptr};

G_DEFINE_FINAL_TYPE(RmfWorldspawn, rmf_worldspawn, RMF_TYPE_ENTITY_DATA)

// GObject /////////////////////////////////////////////////////////////////////

static void rmf_worldspawn_constructed(GObject *object)
{
    auto const self = RMF_WORLDSPAWN(object);
    g_autoptr(RmfLoader) loader =
        rmf_map_object_get_loader(RMF_MAP_OBJECT(self));

    rmf_loader_log(loader, "Worldspawn");
    G_OBJECT_CLASS(rmf_worldspawn_parent_class)->constructed(object);

    g_assert(
        rmf_map_object_get_object_type(RMF_MAP_OBJECT(self))
        == RMF_OBJECT_TYPE_WORLD);

    rmf_read_int(loader, &self->n_paths);
    self->addr_paths = rmf_loader_get_offset(loader);
    rmf_loader_log(loader, "%u paths", self->n_paths);
    for (rmf_int i = 0; i < self->n_paths; ++i) {
        RmfPath path;
        rmf_read_path(loader, &path);
    }
}

static void rmf_worldspawn_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec)
{
    auto const self = RMF_WORLDSPAWN(object);
    switch ((enum Property)property_id) {
    case PROP_N_PATHS:
        g_value_set_uint(value, self->n_paths);
        break;
    case PROP_PATHS: {
        g_autoptr(RmfLoader) loader_ =
            rmf_map_object_get_loader(RMF_MAP_OBJECT(self));
        g_autoptr(RmfLoader) loader = rmf_loader_dup(loader_);
        rmf_loader_set_offset(loader, self->addr_paths);
        auto const iterator = rmf_iterator_new(
            loader,
            self->n_paths,
            (RmfIteratorReadFunc)rmf_path_new);
        g_value_take_object(value, iterator);
    } break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

// Boilerplate /////////////////////////////////////////////////////////////////

static void rmf_worldspawn_class_init(RmfWorldspawnClass *klass)
{
    auto const oclass = G_OBJECT_CLASS(klass);
    oclass->constructed = rmf_worldspawn_constructed;
    oclass->get_property = rmf_worldspawn_get_property;

    obj_properties[PROP_N_PATHS] = g_param_spec_uint(
        "n-paths",
        "Path count",
        "Number of paths.",
        0,
        UINT32_MAX,
        0,
        G_PARAM_READABLE);

    obj_properties[PROP_PATHS] = g_param_spec_object(
        "paths",
        "Paths",
        "Paths.",
        RMF_TYPE_ITERATOR,
        G_PARAM_READABLE);

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);
}

static void rmf_worldspawn_init(RmfWorldspawn *)
{
}

// Public //////////////////////////////////////////////////////////////////////

RmfWorldspawn *rmf_worldspawn_new(RmfLoader *loader)
{
    return g_object_new(RMF_TYPE_WORLDSPAWN, "loader", loader, nullptr);
}

rmf_int rmf_worldspawn_get_n_paths(RmfWorldspawn *self)
{
    rmf_int value = 0;
    g_object_get(self, "n-paths", &value, nullptr);
    return value;
}

RmfIterator *rmf_worldspawn_get_paths(RmfWorldspawn *self)
{
    RmfIterator *value = nullptr;
    g_object_get(self, "paths", &value, nullptr);
    return value;
}
