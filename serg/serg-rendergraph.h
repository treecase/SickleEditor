#pragma once

#include <epoxy/gl.h>
#include <glib-object.h>

G_BEGIN_DECLS

typedef struct {
    GLfloat position[3];
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
