#ifndef BONES_H
#define BONES_H

#include <glm.hpp>
#include <vector>
#include <object.h>
#include <string>
using namespace std;

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
    Bone* findByName(string);
};

#endif