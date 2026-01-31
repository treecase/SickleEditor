// Format info from:
// https://twhl.info/wiki/page/Specification:_RMF
// https://developer.valvesoftware.com/wiki/RMF_(Rich_Map_Format)

#include "rmf-loader.h"

#include <glib-object.h>
#include <stddef.h>

#include "rmf-private.h"

static constexpr rmf_float RMF_MIN_SUPPORTED_VERSION = 1.6f;
static constexpr rmf_float RMF_MAX_SUPPORTED_VERSION = 2.2f;

typedef struct {
    char const *source;
    GBytes *data;
    goffset offset;
    rmf_float version;
} RmfLoaderPrivate;

enum RmfLoaderProperty {
    PROP_SOURCE = 1,
    PROP_DATA,
    PROP_OFFSET,
    PROP_VERSION,
    N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {
    nullptr,
};

G_DEFINE_TYPE_WITH_PRIVATE(RmfLoader, rmf_loader, G_TYPE_OBJECT)

// GObject /////////////////////////////////////////////////////////////////////

static void rmf_loader_constructed(GObject *object)
{
    G_OBJECT_CLASS(rmf_loader_parent_class)->constructed(object);
    auto const self = RMF_LOADER(object);
    RmfLoaderPrivate *const priv = rmf_loader_get_instance_private(self);

    rmf_read_float(self, &priv->version);
    if (priv->version < RMF_MIN_SUPPORTED_VERSION
        || priv->version > RMF_MAX_SUPPORTED_VERSION) {
        g_printerr(
            "Unsupported RMF version %g (only versions %g through %g are "
            "supported)",
            priv->version,
            RMF_MIN_SUPPORTED_VERSION,
            RMF_MAX_SUPPORTED_VERSION);
    }
    rmf_loader_log(self, "RMF version %g", priv->version);

    char magic[3];
    rmf_loader_read(self, 3, magic);
    if (memcmp(magic, "RMF", 3) != 0) {
        g_printerr("Invalid RMF magic number \"%.3s\"\n", magic);
    }
}

static void rmf_loader_dispose(GObject *object)
{
    auto const self = RMF_LOADER(object);
    RmfLoaderPrivate *const priv = rmf_loader_get_instance_private(self);
    g_bytes_unref(priv->data);
    priv->data = nullptr;
    G_OBJECT_CLASS(rmf_loader_parent_class)->dispose(object);
}

static void rmf_loader_finalize(GObject *object)
{
    auto const self = RMF_LOADER(object);
    RmfLoaderPrivate *const priv = rmf_loader_get_instance_private(self);
    g_free((gpointer)priv->source);
    G_OBJECT_CLASS(rmf_loader_parent_class)->finalize(object);
}

static void rmf_loader_get_property(
    GObject *object,
    guint property_id,
    GValue *value,
    GParamSpec *pspec)
{
    auto const self = RMF_LOADER(object);
    RmfLoaderPrivate *const priv = rmf_loader_get_instance_private(self);
    switch ((enum RmfLoaderProperty)property_id) {
    case PROP_OFFSET:
        g_value_set_int64(value, priv->offset);
        break;
    case PROP_VERSION:
        g_value_set_float(value, priv->version);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void rmf_loader_set_property(
    GObject *object,
    guint property_id,
    GValue const *value,
    GParamSpec *pspec)
{
    auto const self = RMF_LOADER(object);
    RmfLoaderPrivate *const priv = rmf_loader_get_instance_private(self);
    switch ((enum RmfLoaderProperty)property_id) {
    case PROP_SOURCE:
        g_assert(G_VALUE_HOLDS_STRING(value));
        g_free((gpointer)priv->source);
        priv->source = g_value_dup_string(value);
        break;
    case PROP_DATA:
        g_assert(G_VALUE_HOLDS_BOXED(value));
        g_assert(G_VALUE_HOLDS(value, G_TYPE_BYTES));
        g_bytes_unref(priv->data);
        priv->data = g_value_dup_boxed(value);
        g_return_if_fail(priv->data);
        break;
    case PROP_OFFSET:
        g_assert(G_VALUE_HOLDS_INT64(value));
        priv->offset = g_value_get_int64(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

// Boilerplate /////////////////////////////////////////////////////////////////

static void rmf_loader_class_init(RmfLoaderClass *klass)
{
    auto const oclass = G_OBJECT_CLASS(klass);
    oclass->constructed = rmf_loader_constructed;
    oclass->dispose = rmf_loader_dispose;
    oclass->finalize = rmf_loader_finalize;
    oclass->get_property = rmf_loader_get_property;
    oclass->set_property = rmf_loader_set_property;

    obj_properties[PROP_SOURCE] = g_param_spec_string(
        "source",
        "Source",
        "String identifying the source of the data (eg. filename).",
        "",
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);

    obj_properties[PROP_DATA] = g_param_spec_boxed(
        "data",
        "Data",
        "GBytes object containing the RMF data.",
        G_TYPE_BYTES,
        G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);

    obj_properties[PROP_OFFSET] = g_param_spec_int64(
        "offset",
        "Offset",
        "Offset into the file.",
        G_MINOFFSET,
        G_MAXOFFSET,
        0,
        G_PARAM_READWRITE);

    obj_properties[PROP_VERSION] = g_param_spec_float(
        "version",
        "Version",
        "RMF version.",
        RMF_MIN_SUPPORTED_VERSION,
        RMF_MAX_SUPPORTED_VERSION,
        RMF_MAX_SUPPORTED_VERSION,
        G_PARAM_READABLE);

    g_object_class_install_properties(oclass, N_PROPERTIES, obj_properties);
}

static void rmf_loader_init(RmfLoader *)
{
}

// Public //////////////////////////////////////////////////////////////////////

RmfLoader *rmf_loader_new(char const *source, GBytes *data)
{
    g_return_val_if_fail(data, nullptr);
    return g_object_new(
        RMF_TYPE_LOADER,
        "source",
        source,
        "data",
        data,
        nullptr);
}

RmfLoader *rmf_loader_dup(RmfLoader *self)
{
    g_return_val_if_fail(self, nullptr);
    RmfLoaderPrivate const *const priv = rmf_loader_get_instance_private(self);
    return g_object_new(
        RMF_TYPE_LOADER,
        "source",
        priv->source,
        "data",
        priv->data,
        "offset",
        priv->offset,
        nullptr);
}

rmf_float rmf_loader_get_version(RmfLoader *self)
{
    rmf_float version = 0.f;
    g_object_get(self, "version", &version, nullptr);
    return version;
}

void rmf_loader_log(RmfLoader *self, char const *format, ...)
{
    va_list ap;
    va_start(ap, format);

    RmfLoaderPrivate const *const priv = rmf_loader_get_instance_private(self);
    g_autofree auto fmt = g_strdup_printf(
        "%s:%08zx:%s",
        priv->source,
        rmf_loader_get_offset(self),
        format);
    g_logv("rmf", G_LOG_LEVEL_INFO, fmt, ap);

    va_end(ap);
}

// Internal ////////////////////////////////////////////////////////////////////

goffset rmf_loader_get_offset(RmfLoader *self)
{
    g_return_val_if_fail(self, 0);
    goffset offset = 0;
    g_object_get(self, "offset", &offset, nullptr);
    return offset;
}

void rmf_loader_set_offset(RmfLoader *self, size_t offset)
{
    g_return_if_fail(self);
    g_object_set(self, "offset", offset, nullptr);
}

void rmf_loader_seek(RmfLoader *self, goffset n)
{
    rmf_loader_set_offset(self, rmf_loader_get_offset(self) + n);
}

void rmf_loader_read(RmfLoader *self, size_t n, void *dest)
{
    g_return_if_fail(self);
    g_return_if_fail(dest);
    RmfLoaderPrivate *priv = rmf_loader_get_instance_private(self);
    auto const src = g_bytes_get_region(priv->data, 1, priv->offset, n);
    memcpy(dest, src, n);
    priv->offset += n;
}
