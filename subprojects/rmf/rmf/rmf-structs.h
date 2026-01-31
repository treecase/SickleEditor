#ifndef RMF_STRUCTS_H
#define RMF_STRUCTS_H

#if !defined(__RMF_H_INSIDE__) && !defined(RMF_COMPILATION)
#  error "Only <rmf.h> can be included directly."
#endif

#include "rmf/rmf-types.h"

#include <glib-object.h>
#include <stddef.h>

// Visgroup ////////////////////////////////////////////////////////////////////

typedef struct {
    char name[128];
    rmf_color color;
    // rmf_byte _pad1;
    rmf_int visgroup_id;
    rmf_byte visible;
    // rmf_byte _pad2[3];
} RmfVisgroup;

#define RMF_TYPE_VISGROUP rmf_visgroup_get_type()
RmfVisgroup *rmf_visgroup_copy(RmfVisgroup const *self);
void rmf_visgroup_free(RmfVisgroup *self);

// Face ////////////////////////////////////////////////////////////////////////

typedef struct {
    char texture_name[256]; // Until RMF v1.8: 36 bytes long
    // rmf_byte _pad1[4];
    rmf_vector right_axis; // Since RMF v2.2
    rmf_float shift_x;
    rmf_vector down_axis; // Since RMF v2.2
    rmf_float shift_y;
    rmf_float angle;
    rmf_float scale_x;
    rmf_float scale_y;
    // rmf_byte _pad2[16]; // Until RMF v1.8: 4 bytes long
    GArray *vertices;
    rmf_vector plane_points[3];
} RmfFace;

#define RMF_TYPE_FACE rmf_face_get_type()
RmfFace *rmf_face_copy(RmfFace const *self);
void rmf_face_free(RmfFace *self);

// KeyValue ////////////////////////////////////////////////////////////////////

typedef struct {
    rmf_nstring key;
    rmf_nstring value;
} RmfKeyvalue;

#define RMF_TYPE_KEYVALUE rmf_keyvalue_get_type()
RmfKeyvalue *rmf_keyvalue_copy(RmfKeyvalue const *self);
void rmf_keyvalue_free(RmfKeyvalue *self);

// PathNode ////////////////////////////////////////////////////////////////////

typedef struct {
    struct rmf_pathnode_inner {
        rmf_vector position;
        rmf_int index;
        char name_override[128];
        rmf_int n_keyvalues;
    } inner;

    goffset addr_keyvalues;
} RmfPathNode;

#define RMF_TYPE_PATH_NODE rmf_path_node_get_type()
RmfPathNode *rmf_path_node_copy(RmfPathNode const *self);
void rmf_path_node_free(RmfPathNode *self);

// Path ////////////////////////////////////////////////////////////////////////

typedef struct {
    struct rmf_path_inner {
        char path_name[128];
        char classname[128];
        rmf_int path_type;
        rmf_int n_nodes;
    } inner;

    goffset addr_nodes;
} RmfPath;

#define RMF_TYPE_PATH rmf_path_get_type()
RmfPath *rmf_path_copy(RmfPath const *self);
void rmf_path_free(RmfPath *self);

// Camera //////////////////////////////////////////////////////////////////////

typedef struct {
    rmf_vector eye_position;
    rmf_vector lookat_position;
} RmfCamera;

#define RMF_TYPE_CAMERA rmf_camera_get_type()
RmfCamera *rmf_camera_copy(RmfCamera const *self);
void rmf_camera_free(RmfCamera *self);

// DOCINFO /////////////////////////////////////////////////////////////////////

typedef struct {
    struct rmf_docinfo_inner {
        char docinfo[8];
        rmf_float docinfo_version;
        rmf_int active_camera;
        rmf_int n_cameras;
    } inner;

    goffset addr_cameras;
} RmfDocinfo;

#define RMF_TYPE_DOCINFO rmf_docinfo_get_type()
RmfDocinfo *rmf_docinfo_copy(RmfDocinfo const *self);
void rmf_docinfo_free(RmfDocinfo *self);

/* clang-format off */
static_assert(
    sizeof(RmfVisgroup) == 140
    && offsetof(RmfVisgroup, name) == 0
    && offsetof(RmfVisgroup, color) == 128
    && offsetof(RmfVisgroup, visgroup_id) == 132
    && offsetof(RmfVisgroup, visible) == 136);

static_assert(
    sizeof(struct rmf_pathnode_inner) == 148
    && offsetof(struct rmf_pathnode_inner, position) == 0
    && offsetof(struct rmf_pathnode_inner, index) == 12
    && offsetof(struct rmf_pathnode_inner, name_override) == 16
    && offsetof(struct rmf_pathnode_inner, n_keyvalues) == 144);

static_assert(
    sizeof(struct rmf_path_inner) == 264
    && offsetof(struct rmf_path_inner, path_name) == 0
    && offsetof(struct rmf_path_inner, classname) == 128
    && offsetof(struct rmf_path_inner, path_type) == 256
    && offsetof(struct rmf_path_inner, n_nodes) == 260);

static_assert(
    sizeof(RmfCamera) == 24
    && offsetof(RmfCamera, eye_position) == 0
    && offsetof(RmfCamera, lookat_position) == 12);

static_assert(
    sizeof(struct rmf_docinfo_inner) == 20
    && offsetof(struct rmf_docinfo_inner, docinfo) == 0
    && offsetof(struct rmf_docinfo_inner, docinfo_version) == 8
    && offsetof(struct rmf_docinfo_inner, active_camera) == 12
    && offsetof(struct rmf_docinfo_inner, n_cameras) == 16);
/* clang-format on */

#endif
