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
    Window(Game* game,bool fullScreen);
    ~Window();
    int shouldClose();
    void setContext();
    void swapBuffers();
    void setKeyCallBack(GLFWkeyfun func);
};
#endif