#include <game.h>
#include <log.h>
#include <text.h>
#include <fpscounter.h>
#include <camera.h>
#include <noise.h>
#include <graphics.h>

Game* currentGame;
void error_callback(int error, const char* description){loge.log("GLFW Error: %s",description);}
void glDebugMessageCallbackFunction( GLenum source, GLenum type, GLuint id,
                   GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
{
  loge.log("OpenGL: %s",message);
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  else
    currentGame->key(key,scancode,action,mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  currentGame->scroll(xoffset,yoffset);
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
  initNoise();
  terrain = new Terrain(glm::vec3(0),this);
  person = new Avatar(glm::vec3(3010,0,4010),this);
  mouseCameraControl = false;
  currentGame = this;
  for (int i = 0;i<256;i++)keys[i] =false;
}

glm::vec2 oldMousePos;
void Game::initGraphics()
{
  // Initialise GLFW
  if (!glfwInit())
    DIE("glfwInit failed");
  glfwSetErrorCallback(error_callback);
  mainWindow = new Window(this);
  mainWindow->setKeyCallBack(&key_callback);
  mainWindow->setScrollCallBack(&scroll_callback);
  // Initialise glew.  If there is an error, report it
  glewExperimental=GL_TRUE;
  GLenum res = glewInit();
  if (res != GLEW_OK)
  {
    DIE2("Could not start GLEW:", glewGetErrorString(res));
  }
  glDebugMessageCallbackARB((GLDEBUGPROCARB) glDebugMessageCallbackFunction, NULL); 
  glEnable(GL_DEPTH_TEST);
  glPointSize(2);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

  initTextEngine();
  shaderManager = new ShaderManager(); 
  camera = new Camera(&shaderManager->frameData.cameraMatrix,&shaderManager->frameData.cameraPosition);
  //camera->setPosition(glm::vec3(4000,100,4000));
  camera->setPosition(glm::vec3(-5,0,0));
  oldMousePos = mainWindow->getSize()/2.f;
}

/// Run the game
void Game::run()
{
  logi.log("Running game...");
  while ((!mainWindow->shouldClose()))
  {
    float ms = getFrameDiff();
    logFrame();
    float fps = getFrameRate();
    char buffer[80]; sprintf(buffer,"Project Magrathea III (%.2f FPS : %.0fms)",fps,ms*1000);
    mainWindow->setTitle(buffer);


    if (mouseCameraControl)
      camera->getInputFromWindow(mainWindow);
    if (keys['W'])
      camera->MoveForward(ms*50.f);
    if (keys['S'])
      camera->MoveForward(-ms*50.f);

    glfwPollEvents();
    mainWindow->setContext();
    renderMainWindow(ms);
    mainWindow->swapBuffers();
  }
  glfwTerminate();
}

void Game::renderMainWindow(float ms)
{
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
  glClearColor(0.54,0.93,0.93,1.0);
  camera->Render();
  shaderManager->setFrameData();
  terrain->Render();
  person->Render(ms);
}

void Game::key(int key, int scancode, int action, int mods)
{
  if (action==GLFW_PRESS)
  {
    if (key=='Q')
    {
      mouseCameraControl = !mouseCameraControl;
      if (mouseCameraControl)
        mainWindow->setMouseCentre();
    }
    if (key=='E')
    {
      if (camera->attachment==NULL)
        camera->attachment = person;
      else
        camera->attachment = NULL;
    }
    if (key==GLFW_KEY_F5)
      shaderManager->reloadAll();
    keys[key] = true;
  }
  if (action==GLFW_RELEASE)
    keys[key] = false;
}

void Game::scroll(float xoffset, float yoffset)
{
  camera->zoomIn(yoffset*-0.5);
}