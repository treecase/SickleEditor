#ifndef RMF_GROUP_H
#define RMF_GROUP_H

#include "rmf/rmf-mapobject.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define RMF_TYPE_GROUP rmf_group_get_type()
G_DECLARE_FINAL_TYPE(RmfGroup, rmf_group, RMF, GROUP, RmfMapObject);

G_END_DECLS

#endif
