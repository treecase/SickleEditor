// Format info from:
// https://twhl.info/wiki/page/Specification:_RMF
// https://developer.valvesoftware.com/wiki/RMF_(Rich_Map_Format)

#include "rmf-root.h"

#include <gio/gio.h>
#include <glib-object.h>

#include "glib.h"
#include "rmf-iterator.h"
#include "rmf-loader.h"
#include "rmf-private.h"
#include "rmf-structs.h"
#include "rmf-types.h"
#include "rmf-worldspawn.h"

struct _RmfRoot {
    RmfLoader parent_instance;

    rmf_int n_visgroups;
    goffset addr_visgroups;
    RmfWorldspawn *worldspawn;
    RmfDocinfo docinfo;
};

enum RmfRootProperty {
    PROP_N_VISGROUPS = 1,
    PROP_VISGROUPS,
    PROP_WORLDSPAWN,
    PROP_DOCINFO,
    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {nullptr};

G_DEFINE_FINAL_TYPE(RmfRoot, rmf_root, RMF_TYPE_LOADER)

// GObject /////////////////////////////////////////////////////////////////////

static void rmf_root_constructed(GObject *object)
{
    G_OBJECT_CLASS(rmf_root_parent_class)->constructed(object);
    auto const self = RMF_ROOT(object);
    auto const loader = RMF_LOADER(self);

    rmf_read_int(loader, &self->n_visgroups);
    self->addr_visgroups = rmf_loader_get_offset(loader);
    rmf_loader_log(loader, "%d visgroups", self->n_visgroups);
    rmf_loader_seek(loader, self->n_visgroups * sizeof(RmfVisgroup));

    g_clear_object(&self->worldspawn);
    self->worldspawn = rmf_worldspawn_new(loader);
    rmf_read_docinfo(loader, &self->docinfo);
}

static void rmf_root_dispose(GObject *object)
{
    auto const self = RMF_ROOT(object);
    g_clear_object(&self->worldspawn);
    G_OBJECT_CLASS(rmf_root_parent_class)->dispose(object);
}

static void rmf_root_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec)
{
    auto const self = RMF_ROOT(object);
    switch ((enum RmfRootProperty)property_id) {
    case PROP_N_VISGROUPS:
        g_value_set_uint(value, self->n_visgroups);
        break;
    case PROP_VISGROUPS: {
        auto const loader = RMF_LOADER(self);
        rmf_loader_set_offset(loader, self->addr_visgroups);
        auto const iterator = rmf_iterator_new(
            loader,
            self->n_visgroups,
            (RmfIteratorReadFunc)rmf_visgroup_new);
        g_value_take_object(value, iterator);
    } break;
    case PROP_WORLDSPAWN: {
        g_value_set_object(value, self->worldspawn);
    } break;
    case PROP_DOCINFO:
        g_value_set_pointer(value, &self->docinfo);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

// Boilerplate /////////////////////////////////////////////////////////////////

static void rmf_root_class_init(RmfRootClass *klass)
{
    auto const oclass = G_OBJECT_CLASS(klass);
    oclass->constructed = rmf_root_constructed;
    oclass->dispose = rmf_root_dispose;
    oclass->get_property = rmf_root_get_property;

    obj_properties[PROP_N_VISGROUPS] = g_param_spec_uint(
        "n-visgroups",
        "Visgroup Count",
        "Number of visgroups in the RMF.",
        0,
        UINT32_MAX,
        0,
        G_PARAM_READABLE);

    obj_properties[PROP_VISGROUPS] = g_param_spec_object(
        "visgroups",
        "Visgroups",
        "List of visgroups in the RMF.",
        RMF_TYPE_ITERATOR,
        G_PARAM_READABLE);

    obj_properties[PROP_WORLDSPAWN] = g_param_spec_object(
        "worldspawn",
        "Worldspawn",
        "Worldspawn object.",
        RMF_TYPE_WORLDSPAWN,
        G_PARAM_READABLE);

    obj_properties[PROP_DOCINFO] = g_param_spec_pointer(
        "docinfo",
        "DOCINFO",
        "Pointer to DOCINFO object.",
        G_PARAM_READABLE);

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);
}

static void rmf_root_init(RmfRoot *)
{
}

// Public //////////////////////////////////////////////////////////////////////

RmfRoot *rmf_root_new(GFile *file)
{
    g_return_val_if_fail(file, nullptr);
    g_autoptr(GBytes) data = g_file_load_bytes(file, nullptr, nullptr, nullptr);
    g_autofree auto filename = g_file_get_path(file);
    g_autofree auto source = g_filename_display_basename(filename);
    return g_object_new(RMF_TYPE_ROOT, "source", source, "data", data, nullptr);
}

rmf_int rmf_root_get_n_visgroups(RmfRoot *self)
{
    rmf_int value = 0;
    g_object_get(self, "n-visgroups", &value, nullptr);
    return value;
}

RmfIterator *rmf_root_get_visgroups(RmfRoot *self)
{
    RmfIterator *value = nullptr;
    g_object_get(self, "visgroups", &value, nullptr);
    return value;
}

RmfWorldspawn *rmf_root_get_worldspawn(RmfRoot *self)
{
    RmfWorldspawn *value = nullptr;
    g_object_get(self, "worldspawn", &value, nullptr);
    return value;
}

RmfDocinfo const *rmf_root_get_docinfo(RmfRoot *self)
{
    RmfDocinfo const *value = nullptr;
    g_object_get(self, "docinfo", &value, nullptr);
    return value;
}
