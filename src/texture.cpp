#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <texture.h>
#include <cmath>
#include <glm.hpp>
using namespace std;

Texture::Texture(int type, int w, int h)
{
  textureType = type;
  width = w;
  height = h;
  data = new float[width*height*4];
  for (int i = 0;i<width*height*4;i++)
    data[i] = i%4<4?0:1;
  glGenTextures(1,&textureNumber);
  glBindTexture(GL_TEXTURE_2D,textureNumber);
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
}

void Texture::toTGA(const char* file)
{
  FILE* fp = fopen(file,"wb");
  putc(0,fp);
  putc(0,fp);
  putc(2,fp);                         /* uncompressed RGB */
  putc(0,fp); putc(0,fp);
  putc(0,fp); putc(0,fp);
  putc(0,fp);
  putc(0,fp); putc(0,fp);           /* X origin */
  putc(0,fp); putc(0,fp);           /* y origin */
  putc((width & 0x00FF),fp);
  putc((width & 0xFF00) / 256,fp);
  putc((height & 0x00FF),fp);
  putc((height & 0xFF00) / 256,fp);
  putc(32,fp);                        /* 24 bit bitmap */
  putc(0,fp);
  for (int i = 0;i<width;i++)
    for (int j = 0;j<height;j++)
    {
      float f = abs(data[(i*height+j)*4+0]);
      if (f<0)f=-f;
      unsigned char c = (abs(data[(i*height+j)*4+0])*255);
      putc((unsigned char)(abs(data[(i*height+j)*4+0]*255)),fp);
      putc((unsigned char)(abs(data[(i*height+j)*4+1]*255)),fp);
      putc((unsigned char)(abs(data[(i*height+j)*4+2]*255)),fp);
      putc((unsigned char)(abs(data[(i*height+j)*4+3]*255)),fp);
    }
  fclose(fp);
}

void Texture::loadData(float* inData)
{
  memcpy(data,inData,width*height*4*sizeof(float));
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data);
}

void Texture::loadBumpData(float* inData)
{
  for (int i=0;i<width*height;i++)
  {
    glm::vec3 no = glm::vec3(1.f);
    if ((i%width)>0 && (i/width>0) && (i%width<width-1) && (i/width<height-1))
    {
      float a,b,c,d,m;
      a = inData[i-width]*1000;
      b = inData[i+1]*1000;
      c = inData[i+width]*1000;
      d = inData[i-1]*1000;
      m = inData[i]*1000;
      no = -glm::cross(glm::vec3(0.f,a-m,-1.f),glm::vec3(1.f,b-m,0.f));
    }
    no = glm::normalize(no);
    data[i*4  ] = no.x;
    data[i*4+1] = no.y;
    data[i*4+2] = no.z;
    data[i*4+3] = inData[i];
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, data);
}

void Texture::load()
{
  glActiveTexture(textureType);
  glBindTexture(GL_TEXTURE_2D,textureNumber);
}


#ifdef TEST_TEXTURE
int main()
{
  Texture texture(HEIGHTMAP_TEXTURE,8,8);
  texture.toTGA("test.tga");
}
#endif
