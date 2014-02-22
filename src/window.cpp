#include <magrathea.h>
#include <game.h>
#include <window.h>

Window::Window(Game* game,bool fullScreen)
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
  {
    windowHandle = glfwCreateWindow(width, height, "Project Magrathea", NULL, NULL);
    glfwSetWindowPos(windowHandle,800,30);
  }
  if (!windowHandle)
    DIE("Failed to create window");
}

Window::~Window()
{
  glfwDestroyWindow(windowHandle);
}

void Window::setContext()
{
  glfwMakeContextCurrent(windowHandle);
  glViewport(0,0,width,height);
}

int Window::shouldClose()
{
  return glfwWindowShouldClose(windowHandle);
}

void Window::swapBuffers()
{
  glfwSwapBuffers(windowHandle);
}

void Window::setKeyCallBack(GLFWkeyfun func)
{
  glfwSetKeyCallback(windowHandle,func);
}