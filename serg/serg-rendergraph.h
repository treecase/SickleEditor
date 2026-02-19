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

void serg_render_graph_render(SergRenderGraph *self);

G_END_DECLS
