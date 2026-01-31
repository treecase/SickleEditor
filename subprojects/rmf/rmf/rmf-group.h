#ifndef RMF_GROUP_H
#define RMF_GROUP_H

#if !defined(__RMF_H_INSIDE__) && !defined(RMF_COMPILATION)
#  error "Only <rmf.h> can be included directly."
#endif

#include "rmf/rmf-mapobject.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define RMF_TYPE_GROUP rmf_group_get_type()
G_DECLARE_FINAL_TYPE(RmfGroup, rmf_group, RMF, GROUP, RmfMapObject);

G_END_DECLS

#endif
