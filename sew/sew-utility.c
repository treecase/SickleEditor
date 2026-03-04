#include "sew-utility.h"

GdkPaintable *sew_make_paintable_from_miptex(WadMiptexFile const *miptex)
{
    guchar *rgba_buffer = g_new(guchar, miptex->width * miptex->height * 4);

    WadRgb const *palette = (WadRgb *)miptex->palette->data;
    for (size_t i = 0; i < miptex->mip_images[0]->len; ++i) {
        guchar pixel = miptex->mip_images[0]->data[i];
        WadRgb color = palette[pixel];
        bool transparent
            = (miptex->texture_name[0] == '{'
               && pixel == miptex->palette->len - 1);
        rgba_buffer[(i * 4) + 0] = color.rgb[0];
        rgba_buffer[(i * 4) + 1] = color.rgb[1];
        rgba_buffer[(i * 4) + 2] = color.rgb[2];
        rgba_buffer[(i * 4) + 3] = transparent ? 0x00 : 0xff;
    }

    g_autoptr(GBytes) bytes
        = g_bytes_new_take(rgba_buffer, miptex->width * miptex->height * 4);

    GdkTexture *texture = gdk_memory_texture_new(
        miptex->width,
        miptex->height,
        GDK_MEMORY_R8G8B8A8,
        bytes,
        miptex->width * 4
    );
    return GDK_PAINTABLE(texture);
}

GdkPaintable *sew_make_paintable_from_qpic(WadQpicFile const *qpic)
{
    guchar *rgba_buffer = g_new(guchar, qpic->width * qpic->height * 4);

    WadRgb const *palette = (WadRgb *)qpic->palette->data;
    for (size_t i = 0; i < qpic->data->len; ++i) {
        guchar pixel = qpic->data->data[i];
        WadRgb color = palette[pixel];
        rgba_buffer[(i * 4) + 0] = color.rgb[0];
        rgba_buffer[(i * 4) + 1] = color.rgb[1];
        rgba_buffer[(i * 4) + 2] = color.rgb[2];
        rgba_buffer[(i * 4) + 3] = 0xff;
    }

    g_autoptr(GBytes) bytes
        = g_bytes_new_take(rgba_buffer, qpic->width * qpic->height * 4);

    GdkTexture *texture = gdk_memory_texture_new(
        qpic->width,
        qpic->height,
        GDK_MEMORY_R8G8B8A8,
        bytes,
        qpic->width * 4
    );
    return GDK_PAINTABLE(texture);
}
