#include "rmf-structs.h"

#include <glib.h>

#include "rmf-loader.h"
#include "rmf-private.h"

// Visgroup ////////////////////////////////////////////////////////////////////

G_DEFINE_BOXED_TYPE(
    RmfVisgroup,
    rmf_visgroup,
    rmf_visgroup_copy,
    rmf_visgroup_free)

void rmf_read_visgroup(RmfLoader *self, RmfVisgroup *visgroup)
{
    rmf_loader_read(self, sizeof(RmfVisgroup), visgroup);
}

RmfVisgroup *rmf_visgroup_new(RmfLoader *loader)
{
    auto const self = g_new(RmfVisgroup, 1);
    rmf_read_visgroup(loader, self);
    return self;
}

RmfVisgroup *rmf_visgroup_copy(RmfVisgroup const *self)
{
    auto const copy = g_new(RmfVisgroup, 1);
    memcpy(copy, self, sizeof(RmfVisgroup));
    return copy;
}

void rmf_visgroup_free(RmfVisgroup *self)
{
    g_free(self);
}

// Face ////////////////////////////////////////////////////////////////////////

G_DEFINE_BOXED_TYPE(RmfFace, rmf_face, rmf_face_copy, rmf_face_free)

void rmf_read_face(RmfLoader *self, RmfFace *face)
{
    rmf_loader_log(self, "Face");
    auto const RMF_VERSION = rmf_loader_get_version(self);

    rmf_loader_read(self, RMF_VERSION > 1.6f ? 256 : 40, face->texture_name);
    rmf_loader_seek(self, 4);
    if (RMF_VERSION >= 2.2f) {
        rmf_read_vector(self, &face->right_axis);
    } else {
        // TODO: Compute right_axis. See
        // https://github.com/id-Software/Quake-Tools/blob/master/qutils/QBSP/MAP.C
        // for details.
    }
    rmf_read_float(self, &face->shift_x);
    if (RMF_VERSION >= 2.2f) {
        rmf_read_vector(self, &face->down_axis);
    } else {
        // TODO: Compute down_axis. See
        // https://github.com/id-Software/Quake-Tools/blob/master/qutils/QBSP/MAP.C
        // for details.
    }
    rmf_read_float(self, &face->shift_y);
    rmf_read_float(self, &face->angle);
    rmf_read_float(self, &face->scale_x);
    rmf_read_float(self, &face->scale_y);
    rmf_loader_seek(self, RMF_VERSION > 1.6f ? 16 : 4);
    rmf_int n_vertices = 0;
    rmf_read_int(self, &n_vertices);

    rmf_loader_log(self, " %u vertices", n_vertices);
    auto const vertices = g_new(rmf_vector, n_vertices);
    rmf_loader_read(self, n_vertices * sizeof(rmf_vector), vertices);
    face->vertices =
        g_array_new_take(vertices, n_vertices, FALSE, sizeof(rmf_vector));
    rmf_loader_read(self, 3 * sizeof(rmf_vector), face->plane_points);
}

RmfFace *rmf_face_new(RmfLoader *loader)
{
    auto const self = g_new(RmfFace, 1);
    rmf_read_face(loader, self);
    return self;
}

RmfFace *rmf_face_copy(RmfFace const *self)
{
    auto const copy = g_new(RmfFace, 1);
    memcpy(copy, self, sizeof(RmfFace));
    copy->vertices = g_array_ref(self->vertices);
    return copy;
}

void rmf_face_free(RmfFace *self)
{
    g_array_unref(self->vertices);
    g_free(self);
}

// KeyValue ////////////////////////////////////////////////////////////////////

G_DEFINE_BOXED_TYPE(
    RmfKeyvalue,
    rmf_keyvalue,
    rmf_keyvalue_copy,
    rmf_keyvalue_free)

void rmf_read_keyvalue(RmfLoader *self, RmfKeyvalue *keyvalue)
{
    rmf_read_nstring(self, &keyvalue->key);
    rmf_read_nstring(self, &keyvalue->value);
}

RmfKeyvalue *rmf_keyvalue_new(RmfLoader *loader)
{
    auto const self = g_new(RmfKeyvalue, 1);
    rmf_read_keyvalue(loader, self);
    return self;
}

RmfKeyvalue *rmf_keyvalue_copy(RmfKeyvalue const *self)
{
    auto const copy = g_new(RmfKeyvalue, 1);
    memcpy(copy, self, sizeof(RmfKeyvalue));
    return copy;
}

void rmf_keyvalue_free(RmfKeyvalue *self)
{
    g_free(self);
}

// PathNode ////////////////////////////////////////////////////////////////////

G_DEFINE_BOXED_TYPE(
    RmfPathNode,
    rmf_path_node,
    rmf_path_node_copy,
    rmf_path_node_free)

void rmf_read_pathnode(RmfLoader *self, RmfPathNode *pathnode)
{
    rmf_loader_read(self, sizeof(struct rmf_pathnode_inner), &pathnode->inner);
    pathnode->addr_keyvalues = rmf_loader_get_offset(self);
    RmfKeyvalue keyvalue;
    for (rmf_int i = 0; i < pathnode->inner.n_keyvalues; ++i) {
        rmf_read_keyvalue(self, &keyvalue);
    }
}

RmfPathNode *rmf_path_node_new(RmfLoader *loader)
{
    auto const self = g_new(RmfPathNode, 1);
    rmf_read_pathnode(loader, self);
    return self;
}

RmfPathNode *rmf_path_node_copy(RmfPathNode const *self)
{
    auto const copy = g_new(RmfPathNode, 1);
    memcpy(copy, self, sizeof(RmfPathNode));
    return copy;
}

void rmf_path_node_free(RmfPathNode *self)
{
    g_free(self);
}

// Path ////////////////////////////////////////////////////////////////////////

G_DEFINE_BOXED_TYPE(RmfPath, rmf_path, rmf_path_copy, rmf_path_free)

void rmf_read_path(RmfLoader *self, RmfPath *path)
{
    rmf_loader_read(self, sizeof(struct rmf_path_inner), &path->inner);
    path->addr_nodes = rmf_loader_get_offset(self);
    RmfPathNode pathnode;
    for (rmf_int i = 0; i < path->inner.n_nodes; ++i) {
        rmf_read_pathnode(self, &pathnode);
    }
}

RmfPath *rmf_path_new(RmfLoader *loader)
{
    auto const self = g_new(RmfPath, 1);
    rmf_read_path(loader, self);
    return self;
}

RmfPath *rmf_path_copy(RmfPath const *self)
{
    auto const copy = g_new(RmfPath, 1);
    memcpy(copy, self, sizeof(RmfPath));
    return copy;
}

void rmf_path_free(RmfPath *self)
{
    g_free(self);
}

// Camera //////////////////////////////////////////////////////////////////////

G_DEFINE_BOXED_TYPE(RmfCamera, rmf_camera, rmf_camera_copy, rmf_camera_free)

void rmf_read_camera(RmfLoader *self, RmfCamera *camera)
{
    rmf_loader_read(self, sizeof(RmfCamera), camera);
}

RmfCamera *rmf_camera_new(RmfLoader *loader)
{
    auto const self = g_new(RmfCamera, 1);
    rmf_read_camera(loader, self);
    return self;
}

RmfCamera *rmf_camera_copy(RmfCamera const *self)
{
    auto const copy = g_new(RmfCamera, 1);
    memcpy(copy, self, sizeof(RmfCamera));
    return copy;
}

void rmf_camera_free(RmfCamera *self)
{
    g_free(self);
}

// DOCINFO /////////////////////////////////////////////////////////////////////

G_DEFINE_BOXED_TYPE(RmfDocinfo, rmf_docinfo, rmf_docinfo_copy, rmf_docinfo_free)

void rmf_read_docinfo(RmfLoader *self, RmfDocinfo *docinfo)
{
    rmf_loader_log(self, "DOCINFO");
    rmf_loader_read(self, sizeof(struct rmf_docinfo_inner), &docinfo->inner);
    docinfo->addr_cameras = rmf_loader_get_offset(self);
    g_assert(memcmp(docinfo->inner.docinfo, "DOCINFO", 8) == 0);
    rmf_loader_log(self, " %u cameras", docinfo->inner.n_cameras);
    RmfCamera camera;
    for (rmf_int i = 0; i < docinfo->inner.n_cameras; ++i) {
        rmf_loader_log(self, " Camera %u", i);
        rmf_read_camera(self, &camera);
    }
}

RmfDocinfo *rmf_docinfo_new(RmfLoader *loader)
{
    auto const self = g_new(RmfDocinfo, 1);
    rmf_read_docinfo(loader, self);
    return self;
}

RmfDocinfo *rmf_docinfo_copy(RmfDocinfo const *self)
{
    auto const copy = g_new(RmfDocinfo, 1);
    memcpy(copy, self, sizeof(RmfDocinfo));
    return copy;
}

void rmf_docinfo_free(RmfDocinfo *self)
{
    g_free(self);
}
