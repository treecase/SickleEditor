#include "serg-program.h"

#include <epoxy/gl.h>
#include <gio/gio.h>
#include <glib-object.h>
#include <graphene-gobject.h>

struct _SergProgram {
    GObject parent_instance;
    GLuint id;
    struct {
        GLint model;
        GLint view;
        GLint projection;
        GLint sampler;
        GLint modulate;
    } uniform_locations;
};

G_DEFINE_FINAL_TYPE(SergProgram, serg_program, G_TYPE_OBJECT)

// Private /////////////////////////////////////////////////////////////////////

static GLuint make_shader(GLenum type, char const *name)
{
    g_autofree char const *path
        = g_strdup_printf("/com/github/treecase/serg/glsl/%s", name);

    g_autoptr(GError) error = nullptr;
    GBytes *bytes
        = g_resources_lookup_data(path, G_RESOURCE_LOOKUP_FLAGS_NONE, &error);
    if (error) {
        g_printerr("%s: %s", __FUNCTION__, error->message);
        return 0;
    }

    gsize size = 0;
    g_autofree char const *source = g_bytes_unref_to_data(bytes, &size);

    GLuint shader = glCreateShader(type);
    GLint length = size;
    glShaderSource(shader, 1, &source, &length);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logSize = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
        g_autofree GLchar *log = g_malloc0(logSize);
        glGetShaderInfoLog(shader, logSize, nullptr, log);
        g_printerr("failed to compile %s: %s\n", name, log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static GLuint make_program(void)
{
    GLuint vshader = make_shader(GL_VERTEX_SHADER, "map.vert");
    GLuint fshader = make_shader(GL_FRAGMENT_SHADER, "map.frag");

    GLuint program = glCreateProgram();

    glAttachShader(program, vshader);
    glAttachShader(program, fshader);

    glLinkProgram(program);
    glDetachShader(program, vshader);
    glDetachShader(program, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logSize = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
        g_autofree GLchar *log = g_malloc0(logSize);
        glGetProgramInfoLog(program, logSize, nullptr, log);
        g_printerr("failed to compile program: %s\n", log);
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

// GObject /////////////////////////////////////////////////////////////////////

static void serg_program_dispose(GObject *object)
{
    auto self = SERG_PROGRAM(object);
    glDeleteProgram(self->id);
    G_OBJECT_CLASS(serg_program_parent_class)->dispose(object);
}

// SergRenderGraph /////////////////////////////////////////////////////////////

static void serg_program_class_init(SergProgramClass *klass)
{
    auto oclass = G_OBJECT_CLASS(klass);
    oclass->dispose = serg_program_dispose;
}

static void serg_program_init(SergProgram *self)
{
    self->id = make_program();

    self->uniform_locations.model = glGetUniformLocation(self->id, "model");
    self->uniform_locations.view = glGetUniformLocation(self->id, "view");
    self->uniform_locations.projection = glGetUniformLocation(self->id, "projection");
    self->uniform_locations.sampler = glGetUniformLocation(self->id, "sampler");
    self->uniform_locations.modulate = glGetUniformLocation(self->id, "modulate");
}

// Public //////////////////////////////////////////////////////////////////////

SergProgram *serg_program_new(void)
{
    return g_object_new(SERG_TYPE_PROGRAM, nullptr);
}

void serg_program_use(SergProgram *self, SergProgramUniforms const *uniforms)
{
    g_return_if_fail(SERG_IS_PROGRAM(self));

    GLfloat model[16], view[16], projection[16];
    GLfloat modulate[3];

    graphene_matrix_to_float(&uniforms->model, model);
    graphene_matrix_to_float(&uniforms->view, view);
    graphene_matrix_to_float(&uniforms->projection, projection);
    graphene_vec3_to_float(&uniforms->modulate, modulate);

    glProgramUniformMatrix4fv(self->id, self->uniform_locations.model, 1, GL_FALSE, model);
    glProgramUniformMatrix4fv(self->id, self->uniform_locations.view, 1, GL_FALSE, view);
    glProgramUniformMatrix4fv(self->id, self->uniform_locations.projection, 1, GL_FALSE, projection);
    glProgramUniform1i(self->id, self->uniform_locations.sampler, uniforms->sampler);
    glProgramUniform3fv(self->id, self->uniform_locations.modulate, 1, modulate);

    glUseProgram(self->id);
}
