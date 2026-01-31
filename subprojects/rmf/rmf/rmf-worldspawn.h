#ifndef RMF_WORLDSPAWN_H
#define RMF_WORLDSPAWN_H

#include "rmf/rmf-entitydata.h"
#include "rmf/rmf-iterator.h"
#include "rmf/rmf-types.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define RMF_TYPE_WORLDSPAWN rmf_worldspawn_get_type()
G_DECLARE_FINAL_TYPE(
    RmfWorldspawn,
    rmf_worldspawn,
    RMF,
    WORLDSPAWN,
    RmfEntityData
);

rmf_int rmf_worldspawn_get_n_paths(RmfWorldspawn *self);
RmfIterator *rmf_worldspawn_get_paths(RmfWorldspawn *self);

G_END_DECLS

#endif
