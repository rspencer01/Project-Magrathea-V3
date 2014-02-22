#include <magrathea.h>
#include <window.h>

Window::Window(bool fullScreen)
{
  width = height = 600;
  if (fullScreen)
  {
    int count;
    GLFWmonitor** monitors = glfwGetMonitors(&count);
    glfwGetMonitorPhysicalSize(monitors[count-1], &width, &height);
    windowHandle = glfwCreateWindow(width, height, "Project Magrathea",monitors[count-1], NULL);
  }
  else
    windowHandle = glfwCreateWindow(width, height, "Project Magrathea", NULL, NULL);
  if (!windowHandle)
    DIE("Failed to create window");
}

void Window::setContext()
{
  glfwMakeContextCurrent(windowHandle);
}

int Window::shouldClose()
{
  return glfwWindowShouldClose(windowHandle);
}

void Window::swapBuffers()
{
  glfwSwapBuffers(windowHandle);
}

