#pragma once

#include <GL/glew.h>
#include <cglm/call.h>

GLuint compile_shader(char *filename, GLenum shader_type);
GLuint compile_program(char *vertex_shader_filename, char *fragment_shader_filename);
void set_int(GLuint program, char *name, int value);
void set_float(GLuint program, char *name, float value);
void set_mat4(GLuint program, char *name, mat4 mat);
void set_vec3(GLuint program, char *name, vec3 vec);