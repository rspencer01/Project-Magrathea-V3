#ifndef BONES_H
#define BONES_H

#include <glm.hpp>
#include <vector>
#include <object.h>
#include <string>
using namespace std;

class TestPerson;
class Skeleton;

class Bone
{
  public:
    Bone* parent;
    int id;
    float xRot,yRot,zRot;
    glm::vec3 offset;
    glm::vec3 origTotOffset;
    glm::vec3 origOffset;
    glm::vec3 totOffset;
    glm::mat4 transform;
    glm::mat4 netTransform;
    char name[80];
    Bone* children [6];
    int numChildren;
    Bone(const char*,int);
    void genTotOffset(glm::vec3 of,glm::mat4 tf);
    glm::mat4* populateMat(glm::mat4* wher);
    void addChild(Bone*);
};

typedef struct
{
  glm::mat4 transformMatrix;
  glm::mat4 boneMatrices[64];
} PersonObjectData;

class Skeleton : public Object
{
  private:
    vector<string> items;
    int addBone(int i,Bone* parent);
    map<string,vector<vector<float> > > animation;
    int numberOfChannels;
    vector<float*> placesForAnimation;
    PersonObjectData skeletonObjectData;
  public:
    float frameTime;
    Skeleton(glm::vec3,Game*);
    Bone* root;
    void readFromFile(const char*);
    TestPerson* parent;
    void getNextAnimation();
    void Render(float ms);
    void addPt(glm::vec3 pos,glm::vec3 bns,glm::vec3 wgh);
    void pushAnimationData(glm::mat4*);
    int boneIDByName(string);
};



class TestPerson : public Object
{
  private:
    Skeleton* skeleton;
/// The PersonObjectData, colour etc for this object
    PersonObjectData personObjectData;
  public:
    TestPerson(glm::vec3,Game*);
    void LoadFromOBJ(const char*);
    void Render(float ms);
    
};

#endif