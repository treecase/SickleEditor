#ifndef RMF_SOLID_H
#define RMF_SOLID_H

#if !defined(__RMF_H_INSIDE__) && !defined(RMF_COMPILATION)
#  error "Only <rmf.h> can be included directly."
#endif

#include "rmf/rmf-iterator.h"
#include "rmf/rmf-mapobject.h"
#include "rmf/rmf-types.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define RMF_TYPE_SOLID rmf_solid_get_type()
G_DECLARE_FINAL_TYPE(RmfSolid, rmf_solid, RMF, SOLID, RmfMapObject);

rmf_int rmf_solid_get_n_faces(RmfSolid *self);
RmfIterator *rmf_solid_get_faces(RmfSolid *self);

G_END_DECLS

#endif
