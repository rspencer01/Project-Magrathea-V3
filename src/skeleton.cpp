#include <string>
#include <fstream>
#include "log.h"
#include "skeleton.h"
#include "game.h"
using namespace std;

// A dummy variable, in the case of bad formatted files.
float dummy;

Skeleton::Skeleton(glm::vec3 p,Game* parent) : Object(p,parent)
{
  // We have triangles and points to visualise the skeleton
  clearTriangleData(100,100);
  numberOfPoints = 0;
  numberOfTriangles = 0;
  numberOfBones=0;
  theta=0.f;
  currentFrame=0;
  animationName="Rest";

  display=false;

  shaderID = game->shaderManager->newShader("skeleton",VERTEX_SHADER|GEOMETRY_SHADER|FRAGMENT_SHADER);
  skeletonObjectData.transformMatrix = glm::mat4(1);
}

int Skeleton::addBone(int i,Bone* parent)
{
  // Check that this line in the file describes a joint.
  if (items[i]!="JOINT") DIE("Bad file format");
  // Make a new bone
  Bone* newBone = new Bone(items[i+1].c_str(),numberOfBones);
  numberOfBones++;
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

void Skeleton::readFromFile(const char* fn)
{
  logi.log("Loading skeleton from file %s",fn);
  ifstream fp (fn);
  string it;
  while(fp.eof()==false)
  {
    fp>>it;
    items.push_back(it);
  }
  fp.close();
  numberOfChannels = 0;
  for (unsigned int i = 0;i<items.size();i++)
  {
    if (items[i]=="CHANNELS")
      numberOfChannels += atoi(items[i+1].c_str());
  }

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
    logi.log(" action: %s",thisAction.c_str());
    ii+=2;
    int numberOfFrames = atoi(items[ii+2].c_str());
    frameTime = atof(items[ii+5].c_str());

    animation.insert(std::pair<string,vector<vector<float> > >(thisAction,vector<vector<float> >()));
    animation[thisAction].resize(numberOfFrames);
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
  }

  pushTriangleData();
}

int Skeleton::getBoneIDByName(std::string nm)
{
  Bone* b = root->findByName(nm);
  if (b==NULL)
    return 0;
  return b->id;
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

void Skeleton::Render(float ms)
{
  theta += ms;
  if (theta>frameTime)
  {
    theta = 0;
    getNextAnimation();
  }
  pushAnimationData(skeletonObjectData.boneMatrices);
  
  if (display)
  {
    glBindBuffer(GL_UNIFORM_BUFFER, objectBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(skeletonObjectData), &skeletonObjectData, GL_DYNAMIC_DRAW);

    game->shaderManager->loadShader(shaderID);
    game->shaderManager->getCurrentShader()->setObjectData(objectBO);
    glBindVertexArray(VAO);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_LINES,0,numberOfPoints);
    glEnable(GL_DEPTH_TEST);
  }
}

void Skeleton::getNextAnimation()
{
  currentFrame++;
  currentFrame %= animation[animationName].size();
  for (int i = 0;i<numberOfChannels;i++)
  {
    *placesForAnimation[i] = animation[animationName][currentFrame][i];
  }
}

void Skeleton::setAnimationName(string nm)
{
  animationName = nm;
}