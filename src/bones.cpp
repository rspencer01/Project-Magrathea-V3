#include <bones.h>
#include <log.h>
#include <fstream>
#include <string>
#include <vector>
#include <game.h>
#include <magrathea.h>
using namespace std;

// A counter for the number of bones.  This needs to be moved somewhere.
int numberOfBones = 0;
// A dummy variable, in the case of bad formatted files.
float dummy;

void Bone::addChild(Bone* c)
{
  children[numChildren] = c;
  numChildren++;
  c->parent = this;
}

int Skeleton::addBone(int i,Bone* parent)
{
  // Check that this line in the file describes a joint.
  if (items[i]!="JOINT") DIE("Bad file format");
  // Make a new bone
  Bone* newBone = new Bone(items[i+1].c_str(),numberOfBones);
  numberOfBones++;
  logi.clog(LOG_COLOR_LIGHT_RED,"Adding bone %s (%d) to %s",newBone->name,newBone->id,parent->name);
  // Add it to its parent
  parent->addChild(newBone);
  // Skip three entries
  i+=3;
  if (items[i]!="OFFSET")DIE("Bad file format");
  newBone->offset = glm::vec3(atof(items[i+1].c_str()),atof(items[i+3].c_str()),atof(items[i+2].c_str()));
  newBone->origTotOffset= parent->origTotOffset+ newBone->offset;
  newBone->origOffset = newBone->offset;
  i+=4;
  // Check that this specifies the channels and get the number of them
  if (items[i]!="CHANNELS") DIE("Bad file format");
  int nc = atoi(items[i+1].c_str());
  i+=2;
  // Now for each channel, check its type and put the relevant pointer in the correct place.
  for (int k = 0;k<nc;k++)
  {
    if (items[i+k]=="Xposition")
      placesForAnimation.push_back(&(newBone->offset.x));
    else if (items[i+k]=="Yposition")
      placesForAnimation.push_back(&(newBone->offset.z));
    else if (items[i+k]=="Zposition")
      placesForAnimation.push_back(&(newBone->offset.y));
    else if (items[i+k]=="Zrotation")
      placesForAnimation.push_back(&(newBone->yRot));
    else if (items[i+k]=="Yrotation")
      placesForAnimation.push_back(&(newBone->zRot));
    else if (items[i+k]=="Xrotation")
      placesForAnimation.push_back(&(newBone->xRot));
    else
      placesForAnimation.push_back(&dummy);
  }
  i+=nc;
  while (1)
  {
    if (items[i]=="End")
    {
      Bone* nnb = new Bone("end",numberOfBones);numberOfBones++;
      newBone->addChild(nnb);
      nnb->offset= glm::vec3(atof(items[i+4].c_str()),atof(items[i+6].c_str()),atof(items[i+5].c_str()));
      i+=8;
    }
    if (items[i]=="JOINT")
      i = addBone(i,newBone);
    else
      return i+1;
  }
  return i;
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

Skeleton::Skeleton(glm::vec3 p,Game* parent) : Object(p,parent)
{
  clearTriangleData(100,100);
  numberOfPoints = 0;
  numberOfTriangles = 0;
}

float theta = 0;
void Skeleton::Render(float ms)
{
  theta += ms;
  if (theta>frameTime)
  {
    theta = 0;
    getNextAnimation();
  }
  pushAnimationData(skeletonObjectData.boneMatrices);

  glBindBuffer(GL_UNIFORM_BUFFER, objectBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(skeletonObjectData), &skeletonObjectData, GL_DYNAMIC_DRAW);

  game->shaderManager->loadShader(shaderID);
  game->shaderManager->getCurrentShader()->setObjectData(objectBO);
  glBindVertexArray(VAO);
  glDisable(GL_DEPTH_TEST);
  //glDrawArrays(GL_POINTS,0,numberOfPoints);
  glDrawArrays(GL_LINES,0,numberOfPoints);
}

void Skeleton::readFromFile(const char* fn)
{
  ifstream fp (fn);
  string it;
  while(fp.eof()==false)
  {
    fp>>it;
    items.push_back(it);
  }
  fp.close();
  numberOfChannels = 0;
  for (int i = 0;i<items.size();i++)
  {
    if (items[i]=="CHANNELS")
      numberOfChannels += atoi(items[i+1].c_str());
  }
  logi.clog(LOG_COLOR_LIGHT_RED,"Skeleton has %d channels",numberOfChannels);

  if (items[0]!="HIERARCHY")
    DIE("Bad File Format");
  if (items[1]!="ROOT")
    DIE("Bad File Format");
  root = new Bone(items[2].c_str(),numberOfBones);numberOfBones++;

  int i = 3; 
  while (items[i]!="CHANNELS")i++;
  int nc = atoi(items[i+1].c_str());
  i+=2;
  for (int k = 0;k<nc;k++)
  {
    if (items[i+k]=="Xposition")
      placesForAnimation.push_back(&(root->offset.x));
    else if (items[i+k]=="Yposition")
      placesForAnimation.push_back(&(root->offset.z));
    else if (items[i+k]=="Zposition")
      placesForAnimation.push_back(&(root->offset.y));
    else if (items[i+k]=="Zrotation")
      placesForAnimation.push_back(&(root->yRot));
    else if (items[i+k]=="Yrotation")
      placesForAnimation.push_back(&(root->zRot));
    else if (items[i+k]=="Xrotation")
      placesForAnimation.push_back(&(root->xRot));
    else
      placesForAnimation.push_back(&dummy);
  }
  while (items[i]!="JOINT")i++;
  while (items[i]=="JOINT")
    i=addBone(i,root);

  root->genTotOffset(glm::vec3(0),glm::mat4(1));

  vector<Bone*> stk;
  stk.push_back(root);
  while (stk.size()>0)
  {
    Bone* c = stk.back();
    if (c->parent!=NULL)
    {
      addPt(c->parent->origTotOffset,glm::vec3(c->parent->id,0,0),glm::vec3(1,0,0));
      addPt(c->origTotOffset,glm::vec3(c->id,0,0),glm::vec3(1,0,0));
    }
    stk.pop_back();
    for (int i =0;i<c->numChildren;i++)
      stk.push_back(c->children[i]);
  }
  int numAction;
  int ii = 0;
  while (items[ii]!="MOTION")ii++;
  numAction = atoi(items[ii+3].c_str());
  ii+=3;
  for (int l = 0;l<numAction;l++)
  {
    string thisAction = items[ii+2];
    ii+=2;
    int numberOfFrames = atoi(items[ii+2].c_str());
    frameTime = atof(items[ii+5].c_str());

    animation.insert(std::pair<string,vector<vector<float> > >(thisAction,vector<vector<float> >()));
    animation[thisAction].resize(numberOfFrames);
    logi.clog(LOG_COLOR_LIGHT_RED,"Loading animation of %d frames",numberOfFrames);
    ii += 6;
    for (int j = 0;j<numberOfFrames;j++)
    {
      for (int k = 0;k<numberOfChannels;k++)
      {
        animation[thisAction][j].push_back(atof(items[ii].c_str()));
        ii++;
      }
    }
    ii--;
    //break;
  }

  pushTriangleData();
  shaderID = game->shaderManager->newShader("person",VERTEX_SHADER|GEOMETRY_SHADER|FRAGMENT_SHADER);
  skeletonObjectData.transformMatrix = glm::mat4(1);

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

int ct = 0;
void Skeleton::getNextAnimation()
{
  string anim = "Walk";
  ct++;
  ct %= animation[anim].size();
  for (int i = 0;i<numberOfChannels;i++)
  {
    *placesForAnimation[i] = animation[anim][ct][i];
  }
}

void Skeleton::pushAnimationData(glm::mat4* wher)
{
  root->genTotOffset(glm::vec3(0),glm::mat4(1));
  root->populateMat(wher);
}

void Skeleton::addPt(glm::vec3 pos,glm::vec3 bns,glm::vec3 wgh)
{
  addPoint(numberOfPoints,pos,bns,glm::vec4(wgh,1));
  numberOfPoints++;
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
  
  skeleton->pushAnimationData(personObjectData.boneMatrices);

  glBindBuffer(GL_UNIFORM_BUFFER, objectBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(personObjectData), &personObjectData, GL_DYNAMIC_DRAW);

  game->shaderManager->loadShader(shaderID);
  game->shaderManager->getCurrentShader()->setObjectData(objectBO);
  glBindVertexArray(VAO);
  //glDrawArrays(GL_POINTS,0,numberOfPoints);
  glDrawElements(GL_TRIANGLES,numberOfTriangles*3,GL_UNSIGNED_INT,0);

  skeleton->Render(ms);
  glEnable(GL_DEPTH_TEST);
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
      int boneID = skeleton->boneIDByName(boneName);
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

int Skeleton::boneIDByName(string nm)
{
  vector<Bone*> stk;
  stk.push_back(root);
  while (stk.size()>0)
  {
    Bone* c = stk.back();
    stk.pop_back();
    if (nm==c->name)
      return c->id;
    for (int i = 0;i<c->numChildren;i++)
      stk.push_back(c->children[i]);
  }
  return 0;
}