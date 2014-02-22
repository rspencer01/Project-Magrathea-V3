/**
 * @file game.h
 * @author Robert Spencer <robert.spencer94@gmail.com>
 * @section DESCRIPTION
 *
 * This file contains the game class, that handles keypresses and rendering.
 */

#ifndef GAMEOPS_H
#define GAMEOPS_H

#include <magrathea.h>
#include <window.h>

/// The gameops class contains all the methods to do with gameplay
///
/// This class interacts with glut graphics and keyboard handlers, and contains all the objects assiciated with a game instance.
/// It handles the render sequence (shadows) and all interactions with the user go through here.
class Game
{
  private:
    /// Initialises OpenGL and the window
    void initGraphics();
    /// Renders the status window
    void renderStatusWindow();
    void renderMainWindow();
    /// The main window;
    Window* mainWindow;
    /// The status window
    Window* statusWindow;
  public:
    /// Creates a game instance and initialises all variables.
    Game();
    /// Runs the game
    void run();
};

#endif
