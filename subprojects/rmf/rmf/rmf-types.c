#include "rmf-types.h"

#include <glib.h>

#include "rmf-loader.h"
#include "rmf-private.h"

void rmf_read_byte(RmfLoader *self, rmf_byte *b)
{
    rmf_loader_read(self, sizeof(rmf_byte), b);
}

void rmf_read_int(RmfLoader *self, rmf_int *i)
{
    rmf_loader_read(self, sizeof(rmf_int), i);
    *i = GUINT32_FROM_LE(*i);
}

void rmf_read_float(RmfLoader *self, rmf_float *f)
{
    rmf_loader_read(self, sizeof(rmf_float), f);
}

void rmf_read_nstring(RmfLoader *rmf, rmf_nstring *nstring)
{
    rmf_read_byte(rmf, &nstring->length);
    g_assert(nstring->length > 0);
    rmf_loader_read(rmf, nstring->length, nstring->data);
    bool const is_null_terminated = nstring->data[nstring->length - 1] == '\0';
    g_assert(is_null_terminated);
}

void rmf_read_color(RmfLoader *rmf, rmf_color *color)
{
    rmf_loader_read(rmf, sizeof(rmf_color), color);
}

void rmf_read_vector(RmfLoader *rmf, rmf_vector *color)
{
    rmf_loader_read(rmf, sizeof(rmf_vector), color);
}
