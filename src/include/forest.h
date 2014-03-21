#ifndef FOREST_H
#define FOREST_H

#include <magrathea.h>
#include <texture.h>
#include <object.h>

class Forest : public Object
{
  private:
    Texture* heightmap;
  public:
    Forest(Game* parent,Texture* hm);
    void Render();
};

#endif