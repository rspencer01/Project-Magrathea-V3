/**
 * @file region.h
 * @author  Robert Spencer <robert.spencer94@gmail.com>
 * @section DESCRIPTION
 *
 * This file describes the region class
 */

#ifndef REGION_H
#define REGION_H

class Game;
#include <object.h>
#include <magrathea.h>
#include <forest.h>

#include <vector>
#include <texture.h>


/// The size of a region on a side
#define PATCH_SIZE 100
#define NUMBER_OF_PATCHES 80

/// A region class is a section of terrain covered by a single texture
///
/// Each region is rendered in turn.  Regions are not related to pages in size.
class Terrain : public Object
{
  private:
  	/// Populates the VBOs
	  void initialiseTriangles();
    void makeTexture();
    Texture* heightmapTexture;
    Forest* forest;
  public:
  	/// Constructs the region with the given coordinates
    Terrain(glm::vec3,Game*);
    /// Render the region
    void Render();
};


#endif
