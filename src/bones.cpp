#include <bones.h>
#include <log.h>
#include <fstream>
#include <string>
#include <vector>
#include <game.h>
#include <magrathea.h>
using namespace std;

// A counter for the number of bones.  This needs to be moved somewhere.


void Bone::addChild(Bone* c)
{
  children[numChildren] = c;
  numChildren++;
  c->parent = this;
}



void Bone::genTotOffset(glm::vec3 of,glm::mat4 tf)
{
  totOffset = of+offset;
  //if (strcmp("Upperarm.Right",name)!=0)
  //  xRot = yRot = zRot = 0;
  //else
  //  logi.log("%f %f %f",xRot,yRot,zRot);
  glm::mat4 xmat = glm::mat4(glm::vec4(1,0.f,0,0.f),
                             glm::vec4(0.f,cos(xRot/180.0*3.1415),-sin(xRot/180.0*3.1415),0.f),
                             glm::vec4(0.f,sin(xRot/180.0*3.1415),cos(xRot/180.0*3.1415),0.f),
                             glm::vec4(0.f,0.f,0.f,1.f));
  glm::mat4 ymat = glm::mat4(glm::vec4(cos(-yRot/180.0*3.1415 ),0.f,-sin(-yRot/180.0*3.1415),0.f),
                             glm::vec4(0.f,1.f,0.f,0.f),
                             glm::vec4(sin(-yRot/180.0*3.1415),0.f,cos(-yRot/180.0*3.1415),0.f),
                             glm::vec4(0.f,0.f,0.f,1.f));
  glm::mat4 zmat = glm::mat4(glm::vec4(cos(zRot/180.0*3.1415),-sin(zRot/180.0*3.1415),0.f,0.f),
                             glm::vec4(sin(zRot/180.0*3.1415),cos(zRot/180.0*3.1415),0.f,0.f),
                             glm::vec4(0.f,0.f,1.f,0.f),
                             glm::vec4(0.f,0.f,0.f,1.f));
  transform = (glm::mat4(glm::vec4(1.f,0.f,0.f,0),
                        glm::vec4(0.f,1.f,0.f,0),
                        glm::vec4(0.f,0.f,1.f,0),
                        glm::vec4(offset,1.f)))*
                        xmat*zmat*ymat


                        ;

  netTransform = tf*transform;
  for (int i = 0;i<numChildren;i++)
    children[i]->genTotOffset(totOffset,netTransform);

  netTransform = netTransform*
                 (glm::mat4(glm::vec4(1.f,0.f,0.f,0),
                  glm::vec4(0.f,1.f,0.f,0),
                  glm::vec4(0.f,0.f,1.f,0),
                  glm::vec4(-origTotOffset,1.f)));
}

glm::mat4* Bone::populateMat(glm::mat4* wher)
{
  wher[id] = netTransform;
  for (int i = 0;i<numChildren;i++)
    children[i]->populateMat(wher);
  return wher;
}

Bone::Bone(const char* n,int i)
{
  id = i;
  parent=NULL;
  strcpy(name,n);
  numChildren = 0;
  transform = glm::mat4(1);
  netTransform = glm::mat4(1);
  xRot = yRot = zRot = 0;
}


Bone* Bone::findByName(string nm)
{
  if (nm==name)
    return this;
  for (int i = 0;i<numChildren;i++)
  {
    Bone* t = children[i]->findByName(nm);
    if (t!=NULL)
      return t;
  }
  return NULL;
}