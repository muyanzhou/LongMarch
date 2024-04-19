#include "iostream"
#include "long_march.h"

int main() {
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    throw std::runtime_error("Failed to initialize GLFW");
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  GLFWwindow *window =
      glfwCreateWindow(800, 600, "Hello World", nullptr, nullptr);

  if (!window) {
    std::cerr << "Failed to create window" << std::endl;
    throw std::runtime_error("Failed to create window");
  }

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
}
