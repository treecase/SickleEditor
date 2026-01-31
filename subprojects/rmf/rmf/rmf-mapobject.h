#ifndef RMF_MAP_OBJECT_H
#define RMF_MAP_OBJECT_H

#include "rmf/rmf-iterator.h"
#include "rmf/rmf-types.h"

#include <glib-object.h>

G_BEGIN_DECLS

typedef enum {
    RMF_OBJECT_TYPE_UNKNOWN,
    RMF_OBJECT_TYPE_WORLD,
    RMF_OBJECT_TYPE_SOLID,
    RMF_OBJECT_TYPE_ENTITY,
    RMF_OBJECT_TYPE_GROUP,
} RmfObjectType;

static char const *const RMF_OBJECT_TYPE_NAMES[] = {
    "unknown",
    "world",
    "solid",
    "entity",
    "group",
};

#define RMF_TYPE_OBJECT_TYPE rmf_object_type_get_type()

//

#define RMF_TYPE_MAP_OBJECT rmf_map_object_get_type()
G_DECLARE_DERIVABLE_TYPE(
    RmfMapObject,
    rmf_map_object,
    RMF,
    MAP_OBJECT,
    GObject
);

struct _RmfMapObjectClass {
    GObjectClass parent_class;
};

RmfObjectType rmf_map_object_get_object_type(RmfMapObject *self);
rmf_int rmf_map_object_get_visgroup_id(RmfMapObject *self);
rmf_color rmf_map_object_get_color(RmfMapObject *self);
rmf_int rmf_map_object_get_n_children(RmfMapObject *self);
RmfIterator *rmf_map_object_get_children(RmfMapObject *self);

G_END_DECLS

#endif
