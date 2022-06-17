#include "shaders.h"
#include "file.h"
#include <GL/glew.h>
#include <cglm/call.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void set_int(GLuint program, char *name, int value) { glUniform1i(glGetUniformLocation(program, name), value); }
void set_float(GLuint program, char *name, float value) { glUniform1f(glGetUniformLocation(program, name), value); }
void set_mat4(GLuint program, char *name, mat4 mat) { glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, (float *)mat); }
void set_vec3(GLuint program, char *name, vec3 vec) { glUniform3fv(glGetUniformLocation(program, name), 1, (float *)vec); }

GLuint compile_shader(char *filename, GLenum shader_type) {
  GLint success;
  GLuint shader = glCreateShader(shader_type);
  GLchar *shader_source = read_file(filename);
  glShaderSource(shader, 1, (const GLchar **)&shader_source, NULL);
  free(shader_source);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    printf("Failed to compile shader %s\n", filename);
    GLint info_log_length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
    GLchar *str_info_log = malloc(info_log_length);
    glGetShaderInfoLog(shader, info_log_length, NULL, str_info_log);
    write(1, str_info_log, info_log_length);
    exit(1);
  }
  return shader;
}

GLuint compile_program(char *vertex_shader_filename, char *fragment_shader_filename) {
  GLuint shader_program = glCreateProgram();
  GLuint vertex_shader = compile_shader(vertex_shader_filename, GL_VERTEX_SHADER);
  GLuint fragment_shader = compile_shader(fragment_shader_filename, GL_FRAGMENT_SHADER);
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  glLinkProgram(shader_program);
  glDetachShader(shader_program, vertex_shader);
  glDetachShader(shader_program, fragment_shader);
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);
  return shader_program;
}
