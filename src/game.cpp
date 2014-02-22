#include <game.h>
#include <log.h>

void error_callback(int error, const char* description)
{
  loge.log(description);
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}

/// The constructor for the game class
///
/// This function is called on the creation of a new game.  It loads
/// all the data, initialises the screen and keyboard functions.
/// @param doGraphics Should this game initialise graphics?
Game::Game()
{
  logi.log("New game");
  logi.log("Initialising Graphics");
  initGraphics();
}

void Game::initGraphics()
{
  // Initialise GLFW
  if (!glfwInit())
    DIE("glfwInit failed");
  glfwSetErrorCallback(error_callback);
  statusWindow = new Window(this,false);
  mainWindow = new Window(this,true);
  statusWindow->setKeyCallBack(&key_callback);
  mainWindow->setKeyCallBack(&key_callback);
}

/// Run the game
void Game::run()
{
  logi.log("Running game...");
  while ((!mainWindow->shouldClose())&&(!statusWindow->shouldClose()))
  {
    glfwPollEvents();
    mainWindow->setContext();
    renderMainWindow();
    mainWindow->swapBuffers();
    statusWindow->setContext();
    renderStatusWindow();
    statusWindow->swapBuffers();
  }
  glfwTerminate();
}

void Game::renderStatusWindow()
{

}

void Game::renderMainWindow()
{
   glClear(GL_COLOR_BUFFER_BIT);
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
glOrtho(-1, 1, -1.f, 1.f, 1.f, -1.f);
glMatrixMode(GL_MODELVIEW);
glLoadIdentity();
glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);
glBegin(GL_TRIANGLES);
glColor3f(1.f, 0.f, 0.f);
glVertex3f(-0.6f, -0.4f, 0.f);
glColor3f(0.f, 1.f, 0.f);
glVertex3f(0.6f, -0.4f, 0.f);
glColor3f(0.f, 0.f, 1.f);
glVertex3f(0.f, 0.6f, 0.f);
glEnd();
}
