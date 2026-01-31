#include "rmf-solid.h"

#include <glib-object.h>

#include "rmf-iterator.h"
#include "rmf-loader.h"
#include "rmf-mapobject.h"
#include "rmf-private.h"

struct _RmfSolid {
    RmfMapObject parent_instance;
    rmf_int n_faces;
    goffset addr_faces;
};

enum Property {
    PROP_N_FACES = 1,
    PROP_FACES,
    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {nullptr};

G_DEFINE_FINAL_TYPE(RmfSolid, rmf_solid, RMF_TYPE_MAP_OBJECT)

// GObject /////////////////////////////////////////////////////////////////////

static void rmf_solid_constructed(GObject *object)
{
    auto const self = RMF_SOLID(object);
    g_autoptr(RmfLoader) loader =
        rmf_map_object_get_loader(RMF_MAP_OBJECT(self));

    rmf_loader_log(loader, "Solid");
    G_OBJECT_CLASS(rmf_solid_parent_class)->constructed(object);

    g_assert(
        rmf_map_object_get_object_type(RMF_MAP_OBJECT(self))
        == RMF_OBJECT_TYPE_SOLID);

    rmf_read_int(loader, &self->n_faces);
    rmf_loader_log(loader, " %d faces", self->n_faces);
    self->addr_faces = rmf_loader_get_offset(loader);
    for (rmf_int i = 0; i < self->n_faces; ++i) {
        RmfFace face;
        rmf_read_face(loader, &face);
    }
}

static void rmf_solid_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec)
{
    auto const self = RMF_SOLID(object);
    switch ((enum Property)property_id) {
    case PROP_N_FACES:
        g_value_set_uint(value, self->n_faces);
        break;
    case PROP_FACES: {
        g_autoptr(RmfLoader) loader_ =
            rmf_map_object_get_loader(RMF_MAP_OBJECT(self));
        g_autoptr(RmfLoader) loader = rmf_loader_dup(loader_);
        rmf_loader_set_offset(loader, self->addr_faces);
        auto const iterator = rmf_iterator_new(
            loader,
            self->n_faces,
            (RmfIteratorReadFunc)rmf_face_new);
        g_value_take_object(value, iterator);
    } break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

// Boilerplate /////////////////////////////////////////////////////////////////

static void rmf_solid_class_init(RmfSolidClass *klass)
{
    auto const oclass = G_OBJECT_CLASS(klass);
    oclass->constructed = rmf_solid_constructed;
    oclass->get_property = rmf_solid_get_property;

    obj_properties[PROP_N_FACES] = g_param_spec_uint(
        "n-faces",
        "Face count",
        "Number of faces.",
        0,
        UINT32_MAX,
        0,
        G_PARAM_READABLE);

    obj_properties[PROP_FACES] = g_param_spec_object(
        "faces",
        "Faces",
        "Faces.",
        RMF_TYPE_ITERATOR,
        G_PARAM_READABLE);

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);
}

static void rmf_solid_init(RmfSolid *)
{
}

// Public //////////////////////////////////////////////////////////////////////

rmf_int rmf_solid_get_n_faces(RmfSolid *self)
{
    rmf_int value = 0;
    g_object_get(self, "n-faces", &value, nullptr);
    return value;
}

RmfIterator *rmf_solid_get_faces(RmfSolid *self)
{
    RmfIterator *value = nullptr;
    g_object_get(self, "faces", &value, nullptr);
    return value;
}

// Internal ////////////////////////////////////////////////////////////////////

RmfSolid *rmf_solid_new(RmfLoader *loader)
{
    return g_object_new(RMF_TYPE_SOLID, "loader", loader, nullptr);
}
