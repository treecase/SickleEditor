#pragma once

#include <epoxy/gl.h>
#include <glib-object.h>
#include <graphene-gobject.h>

G_BEGIN_DECLS

typedef struct {
    graphene_matrix_t model, view, projection;
    GLuint sampler;
    graphene_vec3_t modulate;
} SergProgramUniforms;

// SergProgram

typedef struct _SergProgram SergProgram;

#define SERG_TYPE_PROGRAM serg_program_get_type()

G_DECLARE_FINAL_TYPE(
    SergProgram,
    serg_program,
    SERG,
    PROGRAM,
    GObject
)

SergProgram *serg_program_new(void);

void serg_program_use(SergProgram *program, SergProgramUniforms const *uniforms);

G_END_DECLS