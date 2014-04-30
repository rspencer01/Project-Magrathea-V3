#ifndef SKELETON_H
#define SKELETON_H

#include "magrathea.h"
#include "bones.h"

class Skeleton : public Object
{
  private:
    vector<string> items;
    int addBone(int i,Bone* parent);
    map<string,vector<vector<float> > > animation;
    int numberOfChannels;
    vector<float*> placesForAnimation;
    PersonObjectData skeletonObjectData;
    int numberOfBones;
    float theta;
    int currentFrame;
    string animationName;
    float frameTime;
    Bone* root;
  public:
    Skeleton(glm::vec3,Game*);
    bool display;
    void readFromFile(const char*);
    TestPerson* parent;
    void getNextAnimation();
    void Render(float ms);
    void addPt(glm::vec3 pos,glm::vec3 bns,glm::vec3 wgh);
    void pushAnimationData(glm::mat4*);
    /// Finds the id of a bone, given its human name.
    int getBoneIDByName(string);
    void setAnimationName(string);
};

#endif