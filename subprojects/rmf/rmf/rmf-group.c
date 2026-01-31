#include "rmf-group.h"

#include <glib-object.h>

#include "rmf-loader.h"
#include "rmf-mapobject.h"
#include "rmf-private.h"

struct _RmfGroup {
    RmfMapObject parent_instance;
};

G_DEFINE_FINAL_TYPE(RmfGroup, rmf_group, RMF_TYPE_MAP_OBJECT)

// GObject /////////////////////////////////////////////////////////////////////

static void rmf_group_constructed(GObject *object)
{
    auto const self = RMF_GROUP(object);
    g_autoptr(RmfLoader) loader =
        rmf_map_object_get_loader(RMF_MAP_OBJECT(self));

    rmf_loader_log(loader, "Group");
    G_OBJECT_CLASS(rmf_group_parent_class)->constructed(object);

    g_assert(
        rmf_map_object_get_object_type(RMF_MAP_OBJECT(self))
        == RMF_OBJECT_TYPE_GROUP);
}

// Boilerplate /////////////////////////////////////////////////////////////////

static void rmf_group_class_init(RmfGroupClass *klass)
{
    auto const oclass = G_OBJECT_CLASS(klass);
    oclass->constructed = rmf_group_constructed;
}

static void rmf_group_init(RmfGroup *)
{
}

// Internal ////////////////////////////////////////////////////////////////////

RmfGroup *rmf_group_new(RmfLoader *loader)
{
    return g_object_new(RMF_TYPE_GROUP, "loader", loader, nullptr);
}
