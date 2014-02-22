#include <game.h>
#include <log.h>
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
  int numMonitors;
  GLFWmonitor** monitors = glfwGetMonitors(&numMonitors);
  // Construct window
  mainWindow = glfwCreateWindow(640, 480, "Hello World", monitors[numMonitors-1], NULL);
  if (!mainWindow)
  {
    glfwTerminate();
    DIE("Window construction failed");
  }
  // Make context
  glfwMakeContextCurrent(mainWindow);
}

/// Run the game
void Game::run()
{
  logi.log("Running game...");
  while (!glfwWindowShouldClose(mainWindow))
  {
    glfwSwapBuffers(mainWindow);
    glfwPollEvents();
  }
  glfwTerminate();
}