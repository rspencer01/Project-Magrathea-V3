#ifndef WINDOW_H
#define WINDOW_H

#include <magrathea.h>

class Window
{
  private:
    GLFWwindow* windowHandle;
    int width;
    int height;
  public:
    Window(bool fullScreen);
    int shouldClose();
    void setContext();
    void swapBuffers();
};
#endif