#ifndef TEXTURE_H
#define TEXTURE_H

#include <magrathea.h>

#define HEIGHTMAP_TEXTURE GL_TEXTURE0

class Texture
{
  private:
    int textureType;
    int width;
    int height;
    float* data;
    GLuint textureNumber;
  public:
    Texture(int type,int width, int height);
    void toTGA(const char* file);
    void loadData(float* data);
    void loadBumpData(float* data);
    void load();
};

#endif

