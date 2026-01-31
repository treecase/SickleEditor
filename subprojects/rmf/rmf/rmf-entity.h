#ifndef RMF_ENTITY_H
#define RMF_ENTITY_H

#if !defined(__RMF_H_INSIDE__) && !defined(RMF_COMPILATION)
#  error "Only <rmf.h> can be included directly."
#endif

#include "rmf/rmf-entitydata.h"
#include "rmf/rmf-types.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define RMF_TYPE_ENTITY rmf_entity_get_type()
G_DECLARE_FINAL_TYPE(RmfEntity, rmf_entity, RMF, ENTITY, RmfEntityData);

rmf_vector const *rmf_entity_get_origin(RmfEntity *self);

G_END_DECLS

#endif
