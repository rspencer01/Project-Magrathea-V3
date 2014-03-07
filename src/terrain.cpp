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
  texture = makeTexture();
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
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D,textureNumber);
  glBindVertexArray(VAO);
  glPatchParameteri(GL_PATCH_VERTICES,3);
  glDrawArrays(GL_PATCHES,0,6*NUMBER_OF_PATCHES*NUMBER_OF_PATCHES);
}

GLuint Terrain::makeTexture()
{
  srand(1);
  GLuint texNumber = newTexture(true);
  int width = 2049;
  int height = 2049;
  float* heights = getHeightmapData(width,height);

  float* rgbaData = new float[width*height*4];
  for (int i = 0;i<width;i++)
    for (int j = 0; j<height;j++)
    {
      if (i>0&&j>0&&i<width-1&&j<height-1)
      {
        glm::vec3 norm = -glm::normalize(glm::cross(glm::vec3(-1,(heights[i*width+j]-heights[(i-1)*width+j])*1000,0),
                                                    glm::vec3(0,(heights[i*width+j]-heights[i*width+j-1])*1000,-1))+
                                         glm::cross(glm::vec3(0,(heights[i*width+j]-heights[(i)*width+j-1])*1000,-1),
                                                    glm::vec3(1,(heights[i*width+j]-heights[(i+1)*width+j])*1000,0))+
                                         glm::cross(glm::vec3(1,(heights[i*width+j]-heights[(i+1)*width+j])*1000,0),
                                                    glm::vec3(0,(heights[i*width+j]-heights[(i)*width+j+1])*1000,1))+
                                         glm::cross(glm::vec3(0,(heights[i*width+j]-heights[(i)*width+j+1])*1000,1),
                                                    glm::vec3(-1,(heights[i*width+j]-heights[(i-1)*width+j])*1000,0)));
        rgbaData[4*(i*width+j)] = norm.x;
        rgbaData[4*(i*width+j)+1] = norm.y;
        rgbaData[4*(i*width+j)+2] = norm.z;
      }
      rgbaData[4*(i*width+j)+3] = heights[i*width+j];
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, rgbaData);
  delete[] rgbaData;
  delete[] heights;
  return texNumber;
}
