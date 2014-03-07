#include <stdio.h>
#include <magrathea.h>
#include <terrain.h>
#include <game.h>
#include <dynotree.h>
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
  float* heights = new float[width*height];
  for (int i=0;i<(width)*(height);i++) heights[i] = 0;
  heights[0] = 0.1;
  heights[width-1] = 0;
  heights[(height-1)*(width)] = 0;
  heights[(height-1)*(width)+height-1] = 0;

  int sz = width-1;
  int count = 1;while(1<<count<sz)count++;count--;
  while (sz>1)
  {
    for (int i = 0;i<width-1;i+=sz)
      for (int j = 0;j<height-1;j+=sz)
      {
        float a = heights[i*width+j];
        float b = heights[i*width+j+sz];
        float c = heights[(i+sz)*width+j];
        float d = heights[(i+sz)*width+j+sz];

        float hvar = 1.f/800.f;
        heights[(i+sz/2)*width+(j+sz/2)] = (a+b+c+d)/4 + sz*hvar * (rand()%100-50)/100.f ;
        heights[(i+sz/2)*width+(j)] = (a+c)/2 + sz*hvar * (rand()%100-50)/100.f;
        heights[(i)*width+(j+sz/2)] = (a+b)/2 + sz*hvar * (rand()%100-50)/100.f;
        heights[(i+sz/2)*width+(j+sz)] = (b+d)/2 + sz*hvar * (rand()%100-50)/100.f;
        heights[(i+sz)*width+(j+sz/2)] = (c+d)/2 + sz*hvar * (rand()%100-50)/100.f;

      }
    sz /= 2;
    count--;
  }
  for (int i = 0;i<width;i++)
    for (int j = 0; j<height;j++)
      heights[i*width+j] = pow(std::abs((double)heights[i*width+j]),1.5)*0.7;
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
