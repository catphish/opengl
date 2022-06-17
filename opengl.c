#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define STB_IMAGE_IMPLEMENTATION
#include "shaders.h"
#include "stb_image.h"
#include <cglm/call.h>

#include "cube.h"
#include "utility.h"

int main() {
  // Initialize GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 16);
  glEnable(GL_MULTISAMPLE);

  // Create a window, make it active, disable vsync
  GLFWwindow *window = glfwCreateWindow(1920, 1080, "LearnOpenGL", NULL, NULL);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // Initialize GLEW
  glewInit();

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  GLuint shaderProgram = compile_program("shaders/vertex_1.glsl", "shaders/fragment_1.glsl");
  GLuint shaderProgramLamp = compile_program("shaders/vertex_1.glsl", "shaders/fragment_lamp.glsl");

  // Create a VAO
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Load vertex data into VBO
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Configure offsets in VAO
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // Set background colour
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  glEnable(GL_DEPTH_TEST);
  // glEnable(GL_CULL_FACE);

  mat4 projectionMatrix = GLM_MAT4_IDENTITY_INIT;
  glm_perspective(glm_rad(45), 1920.0 / 1080.0, 0.1, 1000.0, projectionMatrix);

  vec3 cameraPos = {0.0f, 0.0f, 3.0f};
  vec3 cameraFront = {0.0f, 0.0f, -1.0f};
  vec3 cameraUp = {0.0f, 1.0f, 0.0f};

  vec3 lightPos = {0, 0, 0};

  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
    lightPos[0] = sin(glfwGetTime() * 2) * 20.0f;
    lightPos[2] = cos(glfwGetTime() * 2) * 20.0f;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 viewMatrix = GLM_MAT4_IDENTITY_INIT;
    vec3 cameraTarget;
    glm_vec3_add(cameraPos, cameraFront, cameraTarget);
    glm_lookat(cameraPos, cameraTarget, cameraUp, viewMatrix);

    // Draw regular cubes
    glUseProgram(shaderProgram);
    set_mat4(shaderProgram, "projection", projectionMatrix);
    set_mat4(shaderProgram, "view", viewMatrix);
    set_vec3(shaderProgram, "lightPos", lightPos);
    set_vec3(shaderProgram, "viewPos", cameraPos);
    set_vec3(shaderProgram, "material.ambient", (vec3){1.0f, 0.5f, 0.31f});
    set_vec3(shaderProgram, "material.diffuse", (vec3){1.0f, 0.5f, 0.31f});
    set_vec3(shaderProgram, "material.specular", (vec3){0.5f, 0.5f, 0.5f});
    set_float(shaderProgram, "material.shininess", 32.0f);
    set_vec3(shaderProgram, "light.ambient", (vec3){0.2f, 0.2f, 0.2f});
    set_vec3(shaderProgram, "light.diffuse", (vec3){0.5f, 0.5f, 0.5f}); // darken diffuse light a bit
    set_vec3(shaderProgram, "light.specular", (vec3){1.0f, 1.0f, 1.0f});

    for (unsigned int i = 0; i < 10; i++) {
      mat4 modelMatrix = GLM_MAT4_IDENTITY_INIT;
      glm_translate(modelMatrix, cubePositions[i]);
      // glm_rotate(modelMatrix, (float)glfwGetTime() + i, (vec3){1.0, 0.0, 0.0});
      // glm_rotate(modelMatrix, (float)glfwGetTime() + i, (vec3){0.0, 1.0, 0.0});
      set_mat4(shaderProgram, "model", modelMatrix);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Drap lamp
    glUseProgram(shaderProgramLamp);
    set_mat4(shaderProgramLamp, "projection", projectionMatrix);
    set_mat4(shaderProgramLamp, "view", viewMatrix);
    set_vec3(shaderProgramLamp, "lightColor", (vec3){1.0f, 1.0f, 1.0f});
    mat4 modelMatrix = GLM_MAT4_IDENTITY_INIT;
    glm_scale(modelMatrix, (vec3){0.2, 0.2, 0.2});
    glm_translate(modelMatrix, lightPos);
    set_mat4(shaderProgramLamp, "model", modelMatrix);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glfwSwapBuffers(window);
    glfwPollEvents();

    vec3 cameraMovement = {0, 0, 0};
    const float cameraSpeed = 0.05f; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      glm_vec3_scale(cameraFront, cameraSpeed, cameraMovement);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      glm_vec3_scale(cameraFront, -cameraSpeed, cameraMovement);

    glm_vec3_add(cameraPos, cameraMovement, cameraPos);
    fps_counter();
  }
  glfwTerminate();
  return 0;
}
