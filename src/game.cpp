#include <GL/glew.h>
#include <game.h>
#include <log.h>
#include <text.h>

void error_callback(int error, const char* description){loge.log(description);}
void glDebugMessageCallbackFunction( GLenum source, GLenum type, GLuint id,
                   GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
{
  loge.log("%s",message);
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

void Game::initGLEW()
{
  // Initialise glew.  If there is an error, report it
  glewExperimental=GL_TRUE;
  GLenum res = glewInit();
  if (res != GLEW_OK)
  {
    DIE2("Could not start GLEW:", glewGetErrorString(res));
  }
  glDebugMessageCallbackARB((GLDEBUGPROCARB) glDebugMessageCallbackFunction, NULL); 
}

void Game::initGraphics()
{
  glEnable(GL_TEXTURE_2D);
  // Initialise GLFW
  if (!glfwInit())
    DIE("glfwInit failed");
  glfwSetErrorCallback(error_callback);
  statusWindow = new Window(this,false);
  mainWindow = new Window(this,true);
  statusWindow->setKeyCallBack(&key_callback);
  mainWindow->setKeyCallBack(&key_callback);
  statusWindow->setContext();
  initGLEW();


  initTextEngine();
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
  printString(0,0,"   Project Magrathea V3");
  printString(0,1,"+-------------------------+");
  for (int i = 2;i<26;i++)
    printString(0,i,"|                         |");
  printString(0,26,"+-------------------------+");
  printString(2,2,"FPS: %.2f",10.5);

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
