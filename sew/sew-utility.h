#pragma once

#include <gdk/gdk.h>
#include <wad/wad.h>

#if !defined(__SEW_H_INSIDE__) && !defined(SEW_COMPILATION)
#  error "Only <sew/sew.h> can be included directly."
#endif

GdkPaintable *sew_make_paintable_from_miptex(WadMiptexFile const *miptex);

GdkPaintable *sew_make_paintable_from_qpic(WadQpicFile const *qpic);
