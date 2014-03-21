#include <forest.h>
#include <log.h>
#include <gtx/random.hpp>
#include <game.h>
#include <images.h>
int sz=800;
Forest::Forest(Game* parent,Texture* hm) : Object(glm::vec3(0),parent)
{
  
  logi.log("Constructing trees");
  heightmap = hm;
  shaderID = game->shaderManager->newShader("tree",VERTEX_SHADER|GEOMETRY_SHADER|FRAGMENT_SHADER);
  clearTriangleData(sz*sz,0);
  for (int i = 0;i<sz;i++)
    for (int j = 0;j<sz;j++)
    {
      glm::vec2 disp = glm::diskRand(3.f);
      addPoint(i*sz+j,(8000.f/sz)*glm::vec3(i+disp.x,0,j+disp.y),glm::vec3(0),glm::vec4(1));
    }
  texture = textureFromTGA("assets/tree.tga",true);
  pushTriangleData();
}

void Forest::Render()
{
  game->shaderManager->loadShader(shaderID);
  // Load our transformation matrix etcqq
  game->shaderManager->getCurrentShader()->setObjectData(objectBO);
  // Select this object's texture
  texture->load();
  glBindVertexArray(VAO);
  glDrawArrays(GL_POINTS,0,sz*sz);
}