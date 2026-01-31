#ifndef RMF_ITERATOR_H
#define RMF_ITERATOR_H

#include "rmf/rmf-loader.h"

#include <glib-object.h>

G_BEGIN_DECLS

#define RMF_TYPE_ITERATOR rmf_iterator_get_type()
G_DECLARE_FINAL_TYPE(RmfIterator, rmf_iterator, RMF, ITERATOR, GObject)

typedef void *(*RmfIteratorReadFunc)(RmfLoader *self);

[[nodiscard("Returned object must be unref'ed!")]]
RmfIterator *rmf_iterator_new(
    RmfLoader *loader,
    rmf_int count,
    RmfIteratorReadFunc read_func
);

/**
 * Returns null on error or if the iterator is exhausted.
 */
[[nodiscard]]
void *rmf_iterator_next(RmfIterator *self);

G_END_DECLS

#endif
