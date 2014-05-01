#include <avatar.h>
#include <fstream>
#include <game.h>
#include <heightmap.h>
using namespace std;

Avatar::Avatar(glm::vec3 pos,Game* parent) : Object(pos,parent)
{
  logi.clog(LOG_COLOR_LIGHT_RED,"Constructing person");
  skeleton = new Skeleton(pos,parent);
  skeleton->parent = this;
  skeleton->readFromFile("./assets/human_2.bvh+obj");

  shaderID = game->shaderManager->newShader("human",VERTEX_SHADER|GEOMETRY_SHADER|FRAGMENT_SHADER);
  LoadFromOBJ("./assets/human_2.bvh+obj");
}

void Avatar::Render(float ms)
{
  ms = min (ms,1.f);
  skeleton->setAnimationName("Run-M");
  setPosition(position+glm::vec3(0.f,0.f,6.f)*ms);
  position.y = getHeight(position.x,position.z);
  setPosition(position);

  skeleton->Render(ms);  
  skeleton->pushAnimationData(objectData.boneMatrices);

  glBindBuffer(GL_UNIFORM_BUFFER, objectBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(objectData), &objectData, GL_DYNAMIC_DRAW);
  
  game->shaderManager->loadShader(shaderID);
  game->shaderManager->getCurrentShader()->setObjectData(objectBO);
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES,numberOfTriangles*3,GL_UNSIGNED_INT,0);
}

void Avatar::LoadFromOBJ(const char* filepath)
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
