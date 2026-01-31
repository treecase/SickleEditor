#include "rmf-mapobject.h"

#include <glib-object.h>

#include "rmf-entity.h"
#include "rmf-group.h"
#include "rmf-iterator.h"
#include "rmf-loader.h"
#include "rmf-private.h"
#include "rmf-solid.h"
#include "rmf-types.h"
#include "rmf-worldspawn.h"

#define ENUM_VALUE(value, nick) {value, RMF_OBJECT_TYPE_NAMES[value], nick}
G_DEFINE_ENUM_TYPE(
    RmfObjectType,
    rmf_object_type,
    ENUM_VALUE(RMF_OBJECT_TYPE_UNKNOWN, "Unknown"),
    ENUM_VALUE(RMF_OBJECT_TYPE_WORLD, "World"),
    ENUM_VALUE(RMF_OBJECT_TYPE_SOLID, "Solid"),
    ENUM_VALUE(RMF_OBJECT_TYPE_ENTITY, "Entity"),
    ENUM_VALUE(RMF_OBJECT_TYPE_GROUP, "Group"))
#undef ENUM_VALUE

typedef struct {
    RmfLoader *loader;
    RmfObjectType object_type;
    rmf_int visgroup_id;
    rmf_color color;
    rmf_int n_children;
    goffset addr_children;
} RmfMapObjectPrivate;

enum Property {
    PROP_LOADER = 1,
    PROP_OBJECT_TYPE,
    PROP_VISGROUP_ID,
    PROP_COLOR,
    PROP_N_CHILDREN,
    PROP_CHILDREN,
    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {nullptr};

G_DEFINE_TYPE_WITH_PRIVATE(RmfMapObject, rmf_map_object, G_TYPE_OBJECT)

static RmfObjectType object_type_from_nstring(rmf_nstring const *nstring);

// GObject /////////////////////////////////////////////////////////////////////

static void rmf_map_object_constructed(GObject *object)
{
    auto const self = RMF_MAP_OBJECT(object);
    RmfMapObjectPrivate *const priv = rmf_map_object_get_instance_private(self);
    auto const loader = priv->loader;

    rmf_loader_log(loader, "MapObject");
    G_OBJECT_CLASS(rmf_map_object_parent_class)->constructed(object);

    rmf_nstring type;
    rmf_read_nstring(loader, &type);
    rmf_loader_log(loader, " type %.*s", type.length, type.data);
    priv->object_type = object_type_from_nstring(&type);

    rmf_read_int(loader, &priv->visgroup_id);
    rmf_read_color(loader, &priv->color);
    rmf_read_int(loader, &priv->n_children);
    priv->addr_children = rmf_loader_get_offset(loader);

    rmf_loader_log(loader, " %d children", priv->n_children);
    for (rmf_int i = 0; i < priv->n_children; ++i) {
        rmf_loader_log(loader, "Child %d ===", i);
        g_autoptr(RmfMapObject) child = rmf_map_object_new(loader);
    }

    auto const loader_dup = rmf_loader_dup(priv->loader);
    g_object_unref(priv->loader);
    priv->loader = loader_dup;
}

static void rmf_map_object_dispose(GObject *object)
{
    auto const self = RMF_MAP_OBJECT(object);
    RmfMapObjectPrivate *const priv = rmf_map_object_get_instance_private(self);
    g_clear_object(&priv->loader);
    G_OBJECT_CLASS(rmf_map_object_parent_class)->dispose(object);
}

static void rmf_map_object_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec)
{
    auto const self = RMF_MAP_OBJECT(object);
    RmfMapObjectPrivate *const priv = rmf_map_object_get_instance_private(self);
    switch ((enum Property)property_id) {
    case PROP_LOADER:
        g_value_set_object(value, priv->loader);
        break;
    case PROP_OBJECT_TYPE:
        g_value_set_enum(value, priv->object_type);
        break;
    case PROP_VISGROUP_ID:
        g_value_set_uint(value, priv->visgroup_id);
        break;
    case PROP_COLOR:
        g_value_set_pointer(value, &priv->color);
        break;
    case PROP_N_CHILDREN:
        g_value_set_uint(value, priv->n_children);
        break;
    case PROP_CHILDREN: {
        g_autoptr(RmfLoader) loader = rmf_loader_dup(priv->loader);
        rmf_loader_set_offset(loader, priv->addr_children);
        auto const iterator = rmf_iterator_new(
            loader,
            priv->n_children,
            (RmfIteratorReadFunc)rmf_map_object_new);
        g_value_take_object(value, iterator);
    } break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void rmf_map_object_set_property(
    GObject *object,
    guint property_id,
    GValue const *value,
    GParamSpec *pspec)
{
    auto const self = RMF_MAP_OBJECT(object);
    RmfMapObjectPrivate *const priv = rmf_map_object_get_instance_private(self);
    switch ((enum Property)property_id) {
    case PROP_LOADER:
        g_assert(G_VALUE_HOLDS_OBJECT(value));
        g_assert(G_VALUE_HOLDS(value, RMF_TYPE_LOADER));
        g_clear_object(&priv->loader);
        priv->loader = g_value_dup_object(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

// Boilerplate /////////////////////////////////////////////////////////////////

static void rmf_map_object_class_init(RmfMapObjectClass *klass)
{
    auto const oclass = G_OBJECT_CLASS(klass);
    oclass->constructed = rmf_map_object_constructed;
    oclass->dispose = rmf_map_object_dispose;
    oclass->get_property = rmf_map_object_get_property;
    oclass->set_property = rmf_map_object_set_property;

    obj_properties[PROP_LOADER] = g_param_spec_object(
        "loader",
        "Loader",
        "Loader to read from.",
        RMF_TYPE_LOADER,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE);

    obj_properties[PROP_OBJECT_TYPE] = g_param_spec_enum(
        "object-type",
        "Object type",
        "Object type.",
        RMF_TYPE_OBJECT_TYPE,
        RMF_OBJECT_TYPE_UNKNOWN,
        G_PARAM_READABLE);

    obj_properties[PROP_VISGROUP_ID] = g_param_spec_uint(
        "visgroup-id",
        "Visgroup ID",
        "Visgroup ID.",
        0,
        UINT32_MAX,
        0,
        G_PARAM_READABLE);

    obj_properties[PROP_COLOR] =
        g_param_spec_pointer("color", "Color", "Color.", G_PARAM_READABLE);

    obj_properties[PROP_N_CHILDREN] = g_param_spec_uint(
        "n-children",
        "Child count",
        "Number of children.",
        0,
        UINT32_MAX,
        0,
        G_PARAM_READABLE);

    obj_properties[PROP_CHILDREN] = g_param_spec_object(
        "children",
        "Children",
        "Children.",
        RMF_TYPE_ITERATOR,
        G_PARAM_READABLE);

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);
}

static void rmf_map_object_init(RmfMapObject *)
{
}

// Public //////////////////////////////////////////////////////////////////////

RmfLoader *rmf_map_object_get_loader(RmfMapObject *self)
{
    RmfLoader *value = nullptr;
    g_object_get(self, "loader", &value, nullptr);
    return value;
}

RmfObjectType rmf_map_object_get_object_type(RmfMapObject *self)
{
    RmfObjectType value = RMF_OBJECT_TYPE_UNKNOWN;
    g_object_get(self, "object-type", &value, nullptr);
    return value;
}

rmf_int rmf_map_object_get_visgroup_id(RmfMapObject *self)
{
    rmf_int value = 0;
    g_object_get(self, "visgroup-id", &value, nullptr);
    return value;
}

rmf_color rmf_map_object_get_color(RmfMapObject *self)
{
    rmf_color *value = nullptr;
    g_object_get(self, "color", &value, nullptr);
    return *value;
}

rmf_int rmf_map_object_get_n_children(RmfMapObject *self)
{
    rmf_int value = 0;
    g_object_get(self, "n-children", &value, nullptr);
    return value;
}

RmfIterator *rmf_map_object_get_children(RmfMapObject *self)
{
    RmfIterator *value = nullptr;
    g_object_get(self, "children", &value, nullptr);
    return value;
}

// Internal ////////////////////////////////////////////////////////////////////

RmfMapObject *rmf_map_object_new(RmfLoader *loader)
{
    // Peek the object type.
    rmf_nstring type_str;
    rmf_read_nstring(loader, &type_str);
    rmf_loader_seek(loader, -(1 + type_str.length));
    auto const object_type = object_type_from_nstring(&type_str);

    // Construct the proper subclass according to the object type.
    switch (object_type) {
    case RMF_OBJECT_TYPE_WORLD:
        return RMF_MAP_OBJECT(rmf_worldspawn_new(loader));
    case RMF_OBJECT_TYPE_SOLID:
        return RMF_MAP_OBJECT(rmf_solid_new(loader));
    case RMF_OBJECT_TYPE_ENTITY:
        return RMF_MAP_OBJECT(rmf_entity_new(loader));
    case RMF_OBJECT_TYPE_GROUP:
        return RMF_MAP_OBJECT(rmf_group_new(loader));
    case RMF_OBJECT_TYPE_UNKNOWN:
        break;
    }
    g_return_val_if_reached(nullptr);
}

static RmfObjectType object_type_from_nstring(rmf_nstring const *nstring)
{
    if (strncmp(nstring->data, "CMapWorld", nstring->length) == 0) {
        return RMF_OBJECT_TYPE_WORLD;
    } else if (strncmp(nstring->data, "CMapSolid", nstring->length) == 0) {
        return RMF_OBJECT_TYPE_SOLID;
    } else if (strncmp(nstring->data, "CMapEntity", nstring->length) == 0) {
        return RMF_OBJECT_TYPE_ENTITY;
    } else if (strncmp(nstring->data, "CMapGroup", nstring->length) == 0) {
        return RMF_OBJECT_TYPE_GROUP;
    } else {
        g_warning("unknown object type '%.*s'", nstring->length, nstring->data);
        return RMF_OBJECT_TYPE_UNKNOWN;
    }
}
