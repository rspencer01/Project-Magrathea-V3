#include <stdio.h>
#include <magrathea.h>
#include <region.h>
#include <game.h>
#include <dynotree.h>
#include <images.h>
#include <smallFern.h>
#include <heightmap.h>

GLuint texture = 0;

/// Constructs a new region at the given position
/// @param x The x coordinate of the origin
/// @param y The y coordinate of the origin
/// @param parent The game that this region is in.
Region::Region(glm::vec3 pos,Game* parent) : Object(pos,parent)
{
  initialiseTriangles();
  freeze();
  shaderID = game->shaderManager->newShader("terrain",VERTEX_SHADER|GEOMETRY_SHADER|FRAGMENT_SHADER|TESSELATION_SHADER);
}

/// Constructs the triangles
void Region::initialiseTriangles()
{
  clearTriangleData(6,2);
  addPoint(0,
               glm::vec3(0,
			                   0,
			                   0),
                         glm::vec3(1,0,0),
                         1,1,1);
  addPoint(1,
               glm::vec3(0,
			                   0,
                         REGION_SIZE),
                         glm::vec3(1,0,0),
                         1,1,1);
  addPoint(2,
               glm::vec3(REGION_SIZE,
			                   0,
			                   REGION_SIZE),
                         glm::vec3(1,0,0),
                         1,1,1);
  addPoint(3,
               glm::vec3(REGION_SIZE,
			                   0,
			                   0),
                         glm::vec3(1,0,0),
                         1,1,1);
  addPoint(4,
               glm::vec3(0,
			                   0,
                         0),
                         glm::vec3(1,0,0),
                         1,1,1);
  addPoint(5,
               glm::vec3(REGION_SIZE,
			                   0,
			                   REGION_SIZE),
                         glm::vec3(1,0,0),
                         1,1,1);
  pushTriangleData(); 
}

void Region::Render()
{
  game->shaderManager->loadShader(shaderID);
  // Load our transformation matrix etc
  game->shaderManager->getCurrentShader()->setObjectData(objectBO);
  // Select this object's texture
  glBindVertexArray(VAO);
  glPatchParameteri(GL_PATCH_VERTICES,3);
  glDrawArrays(GL_PATCHES,0,6);
}