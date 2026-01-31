#include "rmf-entity.h"

#include <glib-object.h>

#include "rmf-entitydata.h"
#include "rmf-loader.h"
#include "rmf-mapobject.h"
#include "rmf-private.h"

struct _RmfEntity {
    RmfEntityData parent_instance;
    rmf_vector origin;
};

enum Property {
    PROP_ORIGIN = 1,
    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {nullptr};

G_DEFINE_FINAL_TYPE(RmfEntity, rmf_entity, RMF_TYPE_ENTITY_DATA)

// GObject /////////////////////////////////////////////////////////////////////

static void rmf_entity_constructed(GObject *object)
{
    auto const self = RMF_ENTITY(object);
    g_autoptr(RmfLoader) loader =
        rmf_map_object_get_loader(RMF_MAP_OBJECT(self));

    rmf_loader_log(loader, "Entity");
    G_OBJECT_CLASS(rmf_entity_parent_class)->constructed(object);

    g_assert(
        rmf_map_object_get_object_type(RMF_MAP_OBJECT(self))
        == RMF_OBJECT_TYPE_ENTITY);

    rmf_loader_seek(loader, 2);
    rmf_read_vector(loader, &self->origin);
    rmf_loader_seek(loader, 4);
}

static void rmf_entity_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec)
{
    auto const self = RMF_ENTITY(object);
    switch ((enum Property)property_id) {
    case PROP_ORIGIN:
        g_value_set_pointer(value, &self->origin);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

// Boilerplate /////////////////////////////////////////////////////////////////

static void rmf_entity_class_init(RmfEntityClass *klass)
{
    auto const oclass = G_OBJECT_CLASS(klass);
    oclass->constructed = rmf_entity_constructed;
    oclass->get_property = rmf_entity_get_property;

    obj_properties[PROP_ORIGIN] =
        g_param_spec_pointer("origin", "Origin", "Origin.", G_PARAM_READABLE);

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);
}

static void rmf_entity_init(RmfEntity *)
{
}

// Public //////////////////////////////////////////////////////////////////////

rmf_vector const *rmf_entity_get_origin(RmfEntity *self)
{
    rmf_vector const *value = 0;
    g_object_get(self, "origin", &value, nullptr);
    return value;
}

// Internal ////////////////////////////////////////////////////////////////////

RmfEntity *rmf_entity_new(RmfLoader *loader)
{
    return g_object_new(RMF_TYPE_ENTITY, "loader", loader, nullptr);
}
