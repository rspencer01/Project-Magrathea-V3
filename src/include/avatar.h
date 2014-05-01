#ifndef AVATAR_H
#define AVATAR_H

class Avatar;

#include <skeleton.h>

class Avatar : public Object
{
  private:
    Skeleton* skeleton;
  public:
    Avatar(glm::vec3,Game*);
    void LoadFromOBJ(const char*);
    void Render(float ms);
};

#endif