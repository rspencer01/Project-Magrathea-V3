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
	logi.log("New region at %d %d",(int)pos.x,(int)pos.z);
  initialiseTriangles();
  freeze();
  shaderID = game->shaderManager->newShader("terrain");
}

/// Constructs the triangles
void Region::initialiseTriangles()
{
  clearTriangleData(25,16);
  for (int i=0;i<5;i++)
    for (int j=0;j<5;j++)
      addPoint(i*5+j,glm::vec3(0,(float)i,(float)j)*0.1f,glm::vec3(0),1,1,1);
  for (int i=0;i<4;i++)
    for (int j=0;j<4;j++)
    {
      addTriangle(i*4+j,i*5+j,i*5+j+1,(i+1)*5+j);
    }

  pushTriangleData();
}

void Region::Render()
{
  Object::Render();
}