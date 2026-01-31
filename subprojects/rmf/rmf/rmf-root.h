#ifndef RMF_ROOT_H
#define RMF_ROOT_H

#include "rmf/rmf-iterator.h"
#include "rmf/rmf-loader.h"
#include "rmf/rmf-structs.h"
#include "rmf/rmf-types.h"
#include "rmf/rmf-worldspawn.h"

#include <gio/gio.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define RMF_TYPE_ROOT rmf_root_get_type()
G_DECLARE_FINAL_TYPE(RmfRoot, rmf_root, RMF, ROOT, RmfLoader)

[[nodiscard("Returned object must be unref'ed!")]]
RmfRoot *rmf_root_new(GFile *file);

rmf_int rmf_root_get_n_visgroups(RmfRoot *self);
RmfIterator *rmf_root_get_visgroups(RmfRoot *self);
RmfWorldspawn *rmf_root_get_worldspawn(RmfRoot *self);
RmfDocinfo const *rmf_root_get_docinfo(RmfRoot *self);

G_END_DECLS

#endif
