#pragma once

#include <epoxy/gl.h>
#include <glib-object.h>
#include <graphene-gobject.h>

G_BEGIN_DECLS

typedef struct {
    GLfloat position[3];
    GLfloat normal[3];
    GLfloat texcoord[2];
} SergRenderGraphVertex;

/**
 * SergTextureData:
 * @width: Width of the texture in pixels.
 * @height: Height of the texture in pixels.
 * @pixels: The texture's pixel data in RGBA8 format. (Must be at least
 * `width`×`height`×4 bytes in size.)
 */
typedef struct {
    GLsizei width, height;
    GLubyte *pixels;
} SergTextureData;

/**
 * SergPrimitiveData:
 * @texture_z: Index of the primitive's texture in the texture array.
 */
typedef struct {
    GLint texture_z;
} SergPrimitiveData;

// SergRenderGraph

typedef struct _SergRenderGraph SergRenderGraph;

#define SERG_TYPE_RENDER_GRAPH serg_render_graph_get_type()
G_DECLARE_FINAL_TYPE(
    SergRenderGraph,
    serg_render_graph,
    SERG,
    RENDER_GRAPH,
    GObject
)

SergRenderGraph *serg_render_graph_new(void);

graphene_point3d_t const *
serg_render_graph_get_camera_position(SergRenderGraph *graph);
void serg_render_graph_set_camera_position(
    SergRenderGraph *graph,
    graphene_point3d_t const *camera_position
);

graphene_euler_t const *
serg_render_graph_get_camera_direction(SergRenderGraph *graph);
void serg_render_graph_set_camera_direction(
    SergRenderGraph *graph,
    graphene_euler_t const *camera_direction
);

void serg_render_graph_set_camera_aspect(SergRenderGraph *graph, float aspect);

graphene_matrix_t *serg_render_graph_camera_matrix(SergRenderGraph *graph);

void serg_render_graph_set_vertices(
    SergRenderGraph *self,
    size_t length,
    SergRenderGraphVertex const vertices[length]
);

void serg_render_graph_set_elements(
    SergRenderGraph *self,
    size_t length,
    GLuint const indices[length]
);

/**
 * serg_render_graph_set_textures:
 * @n_textures: Number of textures in `textures`.
 * @textures: List of [struct@SergTextureData]s.
 */
void serg_render_graph_set_textures(
    SergRenderGraph *self,
    size_t n_textures,
    SergTextureData textures[n_textures]
);

void serg_render_graph_set_primitive_data(
    SergRenderGraph *self,
    size_t n_primitives,
    SergPrimitiveData primitive_data[n_primitives]
);

void serg_render_graph_render(SergRenderGraph *self);

G_END_DECLS
