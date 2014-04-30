#ifndef AVATAR_H
#define AVATAR_H

class Avatar;

#include <skeleton.h>

typedef struct
{
  glm::mat4 transformMatrix;
  glm::mat4 boneMatrices[64];
} AvatarObjectData;

class Avatar : public Object
{
  private:
    Skeleton* skeleton;
    /// The AvatarObjectData, colour etc for this object
    AvatarObjectData objectData;
  public:
    Avatar(glm::vec3,Game*);
    void LoadFromOBJ(const char*);
    void Render(float ms);
};

#endif