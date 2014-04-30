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

TestPerson::TestPerson(glm::vec3 pos,Game* parent) : Object(pos,parent)
{
  logi.clog(LOG_COLOR_LIGHT_RED,"Constructing person");
  skeleton = new Skeleton(pos,parent);
  skeleton->parent = this;
  skeleton->readFromFile("./assets/human_2.bvh+obj");

  shaderID = game->shaderManager->newShader("human",VERTEX_SHADER|GEOMETRY_SHADER|FRAGMENT_SHADER);
  LoadFromOBJ("./assets/human_2.bvh+obj");
}

void TestPerson::Render(float ms)
{
  skeleton->Render(ms);  
  skeleton->pushAnimationData(personObjectData.boneMatrices);

  glBindBuffer(GL_UNIFORM_BUFFER, objectBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(personObjectData), &personObjectData, GL_DYNAMIC_DRAW);
  
  game->shaderManager->loadShader(shaderID);
  game->shaderManager->getCurrentShader()->setObjectData(objectBO);
  glBindVertexArray(VAO);
  //glDrawArrays(GL_POINTS,0,numberOfPoints);
  glDrawElements(GL_TRIANGLES,numberOfTriangles*3,GL_UNSIGNED_INT,0);
}

void TestPerson::LoadFromOBJ(const char* filepath)
{
  ifstream fp (filepath);
  string it;
  vector<string> items;
  while(fp.eof()==false)
  {
    fp>>it;
    items.push_back(it);
  }
  fp.close();
  int numVertices = 0;
  int numFaces = 0;
  for (int i = 0;i<items.size();i++)
  {
    if (items[i]=="v") numVertices++;
    if (items[i]=="f") numFaces++;
  }
  logi.clog(LOG_COLOR_LIGHT_RED,"Person has %d vertices and %d faces",numVertices,numFaces);
  clearTriangleData(numVertices,numFaces);
  int i = 0;
  vector<glm::vec3> verts;
  vector<glm::vec3> vertNormals;
  int tc = 0;
  while (i<items.size())
  {
    if (items[i]=="v")
    {
      // FIXME
      verts.push_back(glm::vec3(atof(items[i+1].c_str()),atof(items[i+3].c_str())-1,atof(items[i+2].c_str())-0.1));
      i+=3;
    }
    if (items[i]=="vn")
    {
      // FIXME
      vertNormals.push_back(glm::vec3(atof(items[i+1].c_str()),atof(items[i+3].c_str()),atof(items[i+2].c_str())));
      i+=3;
    }
    if (items[i]=="f")
    {
      int a = atoi(items[i+1].substr(0,items[i+1].find('/')).c_str());
      int b = atoi(items[i+2].substr(0,items[i+2].find('/')).c_str());
      int c = atoi(items[i+3].substr(0,items[i+3].find('/')).c_str());
      addTriangle(tc,a,b,c);
      tc++;
    }
    i++;
  }

  for (int i = 0;i<verts.size();i++)
  {
    addPoint(i,verts[i],vertNormals[i],glm::vec4(0,0,0,0));
  }

  i = 0;
  while (i<items.size())
  {
    if (items[i]=="vg")
    {
      string boneName = items[i+1];
      int boneID = skeleton->getBoneIDByName(boneName);
      int nx = atoi(items[i+2].c_str());
      for (int j = 0;j<nx;j++)
      {
        int k = items[i+3+j].find('/');
        string p = items[i+3+j].substr(0,k);
        string q = items[i+3+j].substr(k+1,items[i+3+j].length()-k+1);
        float weight = atof(q.c_str());
        VertexDatum* vd = &vertexData[atoi(p.c_str())];
        if (weight>vd->colour.b)
        {
          vd->texMix.b = boneID;
          vd->colour.b = weight;
        }
        if (weight>vd->colour.g)
        {
          vd->texMix.b = vd->texMix.g;
          vd->colour.b = vd->colour.g;
          vd->texMix.g = boneID;
          vd->colour.g = weight;
        }
        if (weight>vd->colour.r)
        {
          vd->texMix.b = vd->texMix.g;
          vd->colour.b = vd->colour.g;
          vd->texMix.g = vd->texMix.r;
          vd->colour.g = vd->colour.r;
          vd->texMix.r = boneID;
          vd->colour.r = weight;
        }

      }
    }
    i++;
  }


  pushTriangleData();
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