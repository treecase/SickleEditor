#ifndef RMF_PRIVATE_H
#define RMF_PRIVATE_H

#if !defined(__RMF_H_INSIDE__) && !defined(RMF_COMPILATION)
#  error "Only <rmf.h> can be included directly."
#endif

#include "rmf/rmf-entity.h"
#include "rmf/rmf-entitydata.h"
#include "rmf/rmf-group.h"
#include "rmf/rmf-loader.h"
#include "rmf/rmf-mapobject.h"
#include "rmf/rmf-solid.h"
#include "rmf/rmf-structs.h"
#include "rmf/rmf-types.h"
#include "rmf/rmf-worldspawn.h"

#include <glib.h>
#include <stddef.h>

// rmf-loader
goffset rmf_loader_get_offset(RmfLoader *self);
void rmf_loader_set_offset(RmfLoader *self, size_t offset);
void rmf_loader_seek(RmfLoader *self, goffset n);
void rmf_loader_read(RmfLoader *restrict self, size_t n, void *restrict dest);

// rmf-types
void rmf_read_byte(RmfLoader *restrict self, rmf_byte *restrict b);
void rmf_read_int(RmfLoader *restrict self, rmf_int *restrict i);
void rmf_read_float(RmfLoader *restrict self, rmf_float *restrict f);
void rmf_read_nstring(RmfLoader *restrict self, rmf_nstring *restrict nstring);
void rmf_read_color(RmfLoader *restrict self, rmf_color *restrict color);
void rmf_read_vector(RmfLoader *restrict self, rmf_vector *restrict vector);

// rmf-structs
void
rmf_read_visgroup(RmfLoader *restrict self, RmfVisgroup *restrict visgroup);
RmfVisgroup *rmf_visgroup_new(RmfLoader *loader);

void rmf_read_face(RmfLoader *restrict self, RmfFace *restrict face);
RmfFace *rmf_face_new(RmfLoader *self);

void
rmf_read_keyvalue(RmfLoader *restrict self, RmfKeyvalue *restrict keyvalue);
RmfKeyvalue *rmf_keyvalue_new(RmfLoader *self);

void
rmf_read_pathnode(RmfLoader *restrict self, RmfPathNode *restrict pathnode);
RmfPathNode *rmf_pathnode_new(RmfLoader *self);

void rmf_read_path(RmfLoader *restrict self, RmfPath *restrict path);
RmfPath *rmf_path_new(RmfLoader *self);

void rmf_read_camera(RmfLoader *restrict self, RmfCamera *restrict camera);
RmfCamera *rmf_camera_new(RmfLoader *self);

void rmf_read_docinfo(RmfLoader *restrict self, RmfDocinfo *restrict docinfo);
RmfDocinfo *rmf_docinfo_new(RmfLoader *self);

// rmf-mapobject
RmfMapObject *rmf_map_object_new(RmfLoader *loader);
RmfLoader *rmf_map_object_get_loader(RmfMapObject *self);

// rmf-entitydata
RmfEntityData *rmf_entity_data_new(RmfLoader *loader);

// rmf-worldspawn
RmfWorldspawn *rmf_worldspawn_new(RmfLoader *loader);

// rmf-solid
RmfSolid *rmf_solid_new(RmfLoader *loader);

// rmf-entity
RmfEntity *rmf_entity_new(RmfLoader *loader);

// rmf-group
RmfGroup *rmf_group_new(RmfLoader *loader);

#endif
