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
  statusWindow = new Window(false);
  mainWindow = new Window(true);
}

/// Run the game
void Game::run()
{
  logi.log("Running game...");
  while ((!mainWindow->shouldClose())||(!statusWindow->shouldClose()))
  {
    mainWindow->setContext();
    mainWindow->swapBuffers();
    statusWindow->setContext();
    statusWindow->swapBuffers();
    glfwPollEvents();
  }
  glfwTerminate();
}