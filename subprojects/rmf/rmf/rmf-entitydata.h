#ifndef RMF_ENTITY_DATA_H
#define RMF_ENTITY_DATA_H

#include "rmf/rmf-iterator.h"
#include "rmf/rmf-mapobject.h"
#include "rmf/rmf-types.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define RMF_TYPE_ENTITY_DATA rmf_entity_data_get_type()
G_DECLARE_DERIVABLE_TYPE(
    RmfEntityData,
    rmf_entity_data,
    RMF,
    ENTITY_DATA,
    RmfMapObject
);

struct _RmfEntityDataClass {
    RmfMapObjectClass parent_class;
};

char *rmf_entity_data_get_classname(RmfEntityData *self);
rmf_int rmf_entity_data_get_spawnflags(RmfEntityData *self);
rmf_int rmf_entity_data_get_n_keyvalues(RmfEntityData *self);
RmfIterator *rmf_entity_data_get_keyvalues(RmfEntityData *self);

G_END_DECLS

#endif
