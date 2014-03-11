#include <stdio.h>
#include <magrathea.h>
#include <terrain.h>
#include <game.h>
#include <dynotree.h>
#include <heightmap.h>
#include <images.h>
#include <smallFern.h>
#include <heightmap.h>
#include <cmath>
#include <texture.h>
#include <stdlib.h>

GLuint texture = 0;

/// Constructs a new region at the given position
/// @param x The x coordinate of the origin
/// @param y The y coordinate of the origin
/// @param parent The game that this region is in.
Terrain::Terrain(glm::vec3 pos,Game* parent) : Object(pos,parent)
{
  initialiseTriangles();
  freeze();
  shaderID = game->shaderManager->newShader("terrain",VERTEX_SHADER|GEOMETRY_SHADER|FRAGMENT_SHADER|TESSELATION_SHADER);
  makeTexture();
}

/// Constructs the triangles
void Terrain::initialiseTriangles()
{
  clearTriangleData((NUMBER_OF_PATCHES+1)*(NUMBER_OF_PATCHES+1)*6,2);
  for (int i= 0;i<NUMBER_OF_PATCHES+1;i++)
    for (int j = 0;j<NUMBER_OF_PATCHES+1;j++)
    {
      addPoint((i*NUMBER_OF_PATCHES+j)*6,
                   glm::vec3(i*PATCH_SIZE,
			                       0,
			                       j*PATCH_SIZE),
                             glm::vec3(1,0,0),
                             1,1,1);
      addPoint((i*NUMBER_OF_PATCHES+j)*6+1,
                   glm::vec3(i*PATCH_SIZE,
			                       0,
                             j*PATCH_SIZE+PATCH_SIZE),
                             glm::vec3(1,0,0),
                             1,1,1);
      addPoint((i*NUMBER_OF_PATCHES+j)*6+2,
                   glm::vec3(i*PATCH_SIZE+PATCH_SIZE,
			                       0,
			                       j*PATCH_SIZE+PATCH_SIZE),
                             glm::vec3(1,0,0),
                             1,1,1);
      addPoint((i*NUMBER_OF_PATCHES+j)*6+3,
                   glm::vec3(i*PATCH_SIZE+PATCH_SIZE,
			                       0,
			                       j*PATCH_SIZE+PATCH_SIZE),
                             glm::vec3(1,0,0),
                             1,1,1);
      addPoint((i*NUMBER_OF_PATCHES+j)*6+4,
                   glm::vec3(i*PATCH_SIZE+PATCH_SIZE,
			                       0,
                             j*PATCH_SIZE),
                             glm::vec3(1,0,0),
                             1,1,1);
      addPoint((i*NUMBER_OF_PATCHES+j)*6+5,
                   glm::vec3(i*PATCH_SIZE,
			                       0,
                             j*PATCH_SIZE),
                             glm::vec3(1,0,0),
                             1,1,1);
    }
  pushTriangleData(); 
}

void Terrain::Render()
{
  game->shaderManager->loadShader(shaderID);
  // Load our transformation matrix etc
  game->shaderManager->getCurrentShader()->setObjectData(objectBO);
  // Select this object's texture
  glBindVertexArray(VAO);
  glPatchParameteri(GL_PATCH_VERTICES,3);
  glDrawArrays(GL_PATCHES,0,6*NUMBER_OF_PATCHES*NUMBER_OF_PATCHES);
}

void Terrain::makeTexture()
{
  srand(1);
  int width = 2049;
  int height = 2049;
  float* heights = getHeightmapData(width,height);
  heightmapTexture = new Texture(HEIGHTMAP_TEXTURE,width,height);
  heightmapTexture->loadBumpData(heights);
  heightmapTexture->load();
  heightmapTexture->toTGA("heightmap.tga");
}
