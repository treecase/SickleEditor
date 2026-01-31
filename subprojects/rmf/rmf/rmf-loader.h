#ifndef RMF_LOADER_H
#define RMF_LOADER_H

#if !defined(__RMF_H_INSIDE__) && !defined(RMF_COMPILATION)
#  error "Only <rmf.h> can be included directly."
#endif

#include "rmf/rmf-types.h"

#include <glib-object.h>
#include <glib.h>

G_BEGIN_DECLS

#define RMF_TYPE_LOADER rmf_loader_get_type()
G_DECLARE_DERIVABLE_TYPE(RmfLoader, rmf_loader, RMF, LOADER, GObject)

struct _RmfLoaderClass {
    GObjectClass parent_class;
};

[[nodiscard("Returned object must be unref'ed!")]]
RmfLoader *rmf_loader_new(char const *source, GBytes *bytes);

[[nodiscard("Returned object must be unref'ed!")]]
RmfLoader *rmf_loader_dup(RmfLoader *self);

rmf_float rmf_loader_get_version(RmfLoader *self);

void rmf_loader_log(RmfLoader *restrict self, char const *restrict format, ...);
void rmf_loader_logv(
    RmfLoader *restrict self,
    char const *restrict format,
    va_list ap
);

G_END_DECLS

#endif
