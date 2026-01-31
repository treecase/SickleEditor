#include "rmf-entitydata.h"

#include <glib-object.h>

#include "rmf-iterator.h"
#include "rmf-loader.h"
#include "rmf-mapobject.h"
#include "rmf-private.h"
#include "rmf-types.h"

typedef struct {
    rmf_nstring classname;
    rmf_int spawnflags;
    rmf_int n_keyvalues;
    goffset addr_keyvalues;
} RmfEntityDataPrivate;

enum Property {
    PROP_CLASSNAME = 1,
    PROP_SPAWNFLAGS,
    PROP_N_KEYVALUES,
    PROP_KEYVALUES,
    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {nullptr};

G_DEFINE_TYPE_WITH_PRIVATE(RmfEntityData, rmf_entity_data, RMF_TYPE_MAP_OBJECT)

// GObject /////////////////////////////////////////////////////////////////////

static void rmf_entity_data_constructed(GObject *object)
{
    auto const self = RMF_ENTITY_DATA(object);
    RmfEntityDataPrivate *priv = rmf_entity_data_get_instance_private(self);
    g_autoptr(RmfLoader) loader =
        rmf_map_object_get_loader(RMF_MAP_OBJECT(self));

    rmf_loader_log(loader, "EntityData");
    G_OBJECT_CLASS(rmf_entity_data_parent_class)->constructed(object);

    rmf_read_nstring(loader, &priv->classname);
    rmf_loader_log(
        loader,
        " classname: %.*s",
        priv->classname.length,
        priv->classname.data);
    rmf_loader_seek(loader, 4);
    rmf_read_int(loader, &priv->spawnflags);
    rmf_read_int(loader, &priv->n_keyvalues);
    priv->addr_keyvalues = rmf_loader_get_offset(loader);
    rmf_loader_log(loader, " %u keyvalues", priv->n_keyvalues);
    for (rmf_int i = 0; i < priv->n_keyvalues; ++i) {
        RmfKeyvalue keyvalue;
        rmf_read_keyvalue(loader, &keyvalue);
        rmf_loader_log(
            loader,
            " keyvalue %u: %.*s = %.*s",
            i,
            keyvalue.key.length,
            keyvalue.key.data,
            keyvalue.value.length,
            keyvalue.value.data);
    }
    rmf_loader_seek(loader, 12);
}

static void rmf_entity_data_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec)
{
    auto const self = RMF_ENTITY_DATA(object);
    RmfEntityDataPrivate *const priv =
        rmf_entity_data_get_instance_private(self);
    switch ((enum Property)property_id) {
    case PROP_CLASSNAME:
        g_value_set_string(value, priv->classname.data);
        break;
    case PROP_SPAWNFLAGS:
        g_value_set_uint(value, priv->spawnflags);
        break;
    case PROP_N_KEYVALUES:
        g_value_set_uint(value, priv->n_keyvalues);
        break;
    case PROP_KEYVALUES: {
        g_autoptr(RmfLoader) loader_ =
            rmf_map_object_get_loader(RMF_MAP_OBJECT(self));
        g_autoptr(RmfLoader) loader = rmf_loader_dup(loader_);
        rmf_loader_set_offset(loader, priv->addr_keyvalues);
        auto const iterator = rmf_iterator_new(
            loader,
            priv->n_keyvalues,
            (RmfIteratorReadFunc)rmf_keyvalue_new);
        g_value_take_object(value, iterator);
    } break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

// Boilerplate /////////////////////////////////////////////////////////////////

static void rmf_entity_data_class_init(RmfEntityDataClass *klass)
{
    auto const oclass = G_OBJECT_CLASS(klass);
    oclass->constructed = rmf_entity_data_constructed;
    oclass->get_property = rmf_entity_data_get_property;

    obj_properties[PROP_CLASSNAME] = g_param_spec_string(
        "classname",
        "Classname",
        "Classname.",
        "",
        G_PARAM_READABLE);

    obj_properties[PROP_SPAWNFLAGS] = g_param_spec_uint(
        "spawnflags",
        "Spawnflags",
        "Spawnflags.",
        0,
        UINT_MAX,
        0,
        G_PARAM_READABLE);

    obj_properties[PROP_N_KEYVALUES] = g_param_spec_uint(
        "n-keyvalues",
        "Keyvalue count",
        "Number of keyvalues.",
        0,
        UINT_MAX,
        0,
        G_PARAM_READABLE);

    obj_properties[PROP_KEYVALUES] = g_param_spec_object(
        "keyvalues",
        "Keyvalues",
        "Keyvalues.",
        RMF_TYPE_ITERATOR,
        G_PARAM_READABLE);

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);
}

static void rmf_entity_data_init(RmfEntityData *)
{
}

// Public //////////////////////////////////////////////////////////////////////

char *rmf_entity_data_get_classname(RmfEntityData *self)
{
    char *value = nullptr;
    g_object_get(self, "classname", &value, nullptr);
    return value;
}

rmf_int rmf_entity_data_get_spawnflags(RmfEntityData *self)
{
    rmf_int value = 0;
    g_object_get(self, "spawnflags", &value, nullptr);
    return value;
}

rmf_int rmf_entity_data_get_n_keyvalues(RmfEntityData *self)
{
    rmf_int value = 0;
    g_object_get(self, "n-keyvalues", &value, nullptr);
    return value;
}

RmfIterator *rmf_entity_data_get_keyvalues(RmfEntityData *self)
{
    RmfIterator *value = nullptr;
    g_object_get(self, "keyvalues", &value, nullptr);
    return value;
}

// Internal ////////////////////////////////////////////////////////////////////

RmfEntityData *rmf_entity_data_new(RmfLoader *loader)
{
    return g_object_new(RMF_TYPE_ENTITY_DATA, "loader", loader, nullptr);
}
