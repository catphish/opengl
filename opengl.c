#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cglm/cglm.h>
#include <cglm/call.h>

typedef GLfloat vector4[4];

float vertices[] = {
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

vec3 cubePositions[] = {
    (vec3){0.0f, 0.0f, 0.0f},
    (vec3){2.0f, 5.0f, -15.0f},
    (vec3){-1.5f, -2.2f, -2.5f},
    (vec3){-3.8f, -2.0f, -12.3f},
    (vec3){2.4f, -0.4f, -3.5f},
    (vec3){-1.7f, 3.0f, -7.5f},
    (vec3){1.3f, -2.0f, -2.5f},
    (vec3){1.5f, 2.0f, -2.5f},
    (vec3){1.5f, 0.2f, -1.5f},
    (vec3){-1.3f, 1.0f, -1.5f}};

double last_time;
int frame_count = 0;
void fps_counter()
{
  frame_count++;
  double current_time = glfwGetTime();
  if (current_time - last_time > 1.0)
  {
    printf("%f ms/frame\n", (current_time - last_time) / (float)frame_count * 1000.0);
    last_time = current_time;
    frame_count = 0;
  }
}

int load_shader(char *filename, unsigned int shader_type)
{
  int f = open(filename, O_RDONLY);
  if (f < 0)
  {
    printf("Failed to open shader source %s", filename);
    perror("");
    exit(1);
  }
  int n = lseek(f, 0, SEEK_END);
  char *shader_source = malloc(n + 1);
  lseek(f, 0, SEEK_SET);
  n = read(f, shader_source, n);
  shader_source[n] = 0;
  close(f);
  GLint shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, (const GLchar **)&shader_source, NULL);
  free(shader_source);
  glCompileShader(shader);
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success)
  {
    printf("Failed to compile shader %s\n", filename);
    GLint infoLogLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar *strInfoLog = malloc(infoLogLength);
    glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
    write(1, strInfoLog, infoLogLength);
    exit(1);
  }
  return shader;
}

int main()
{
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

  // Compile shaders
  unsigned int shaderProgram = glCreateProgram();
  int vertex_shader = load_shader("shaders/vertex_1.glsl", GL_VERTEX_SHADER);
  int fragment_shader = load_shader("shaders/fragment_1.glsl", GL_FRAGMENT_SHADER);
  glAttachShader(shaderProgram, vertex_shader);
  glAttachShader(shaderProgram, fragment_shader);
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  // Create a VAO
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  // Load vertex data into VBO
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // If we want to use an EBO, set one up like this
  // unsigned int EBO;
  // glGenBuffers(1, &EBO);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // Configure offsets in VAO
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // Load texture
  int width, height, nrChannels;
  unsigned int textures[2];
  unsigned char *data;

  glGenTextures(2, textures);

  stbi_set_flip_vertically_on_load(1);

  data = stbi_load("textures/wall.jpg", &width, &height, &nrChannels, 0);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // set texture filtering to nearest neighbor to clearly see the texels/pixels
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  data = stbi_load("textures/awesomeface.png", &width, &height, &nrChannels, 0);
  glBindTexture(GL_TEXTURE_2D, textures[1]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // set texture filtering to nearest neighbor to clearly see the texels/pixels
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);

  glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
  glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, textures[1]);

  // Set background colour
  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  last_time = glfwGetTime();

  unsigned int modelUniform = glGetUniformLocation(shaderProgram, "model");
  unsigned int viewUniform = glGetUniformLocation(shaderProgram, "view");
  unsigned int projectionUniform = glGetUniformLocation(shaderProgram, "projection");

  glEnable(GL_DEPTH_TEST);

  mat4 projectionMatrix = GLM_MAT4_IDENTITY_INIT;
  glm_perspective(glm_rad(45), 1920.0 / 1080.0, 0.1, 1000.0, projectionMatrix);
  glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, (float *)projectionMatrix);

  vec3 cameraPos = {0.0f, 0.0f, 3.0f};
  vec3 cameraFront = {0.0f, 0.0f, -1.0f};
  vec3 cameraUp = {0.0f, 1.0f, 0.0f};

  // Main rendering loop
  while (!glfwWindowShouldClose(window))
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 viewMatrix = GLM_MAT4_IDENTITY_INIT;
    vec3 cameraTarget;
    glm_vec3_add(cameraPos, cameraFront, cameraTarget);
    glm_lookat(cameraPos, cameraTarget, cameraUp, viewMatrix);
    glUniformMatrix4fv(viewUniform, 1, GL_FALSE, (float *)viewMatrix);

    for (unsigned int i = 0; i < 10; i++)
    {

      mat4 modelMatrix = GLM_MAT4_IDENTITY_INIT;
      glm_translate(modelMatrix, cubePositions[i]);
      glm_rotate(modelMatrix, (float)glfwGetTime() + i, (vec3){1.0, 0.0, 0.0});
      glm_rotate(modelMatrix, (float)glfwGetTime() + i, (vec3){0.0, 1.0, 0.0});
      glUniformMatrix4fv(modelUniform, 1, GL_FALSE, (float *)modelMatrix);

      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

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
