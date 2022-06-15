#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

double last_time = 0;
int frame_count = 0;

void fps_counter() {
  frame_count++;
  double current_time = glfwGetTime();
  if (current_time - last_time > 1.0) {
    printf("%f ms/frame\n", (current_time - last_time) / (float)frame_count * 1000.0);
    last_time = current_time;
    frame_count = 0;
  }
}
