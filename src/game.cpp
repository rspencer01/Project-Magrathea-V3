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
}

/// Run the game
void Game::run()
{
  logi.log("Running game...");
  while (1);
}