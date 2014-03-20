#include <heightmap.h>
#include <string.h>
#include <math.h>
#include <log.h>
#include <stdlib.h>

float* getHeightmapData(int width, int height)
{
  int seed = 1;
  srand(seed);
  logi.log("Constructing heightmap, seed %d",seed);
  float* heights = new float[width*height];
  for (int i=0;i<(width)*(height);i++) heights[i] = 0;
  heights[0] = 0.1;
  heights[width-1] = 0;
  heights[(height-1)*(width)] = 0.0;
  heights[(height-1)*(width)+height-1] = 0;

  int sz = width-1;
  int count = 1;while(1<<count<sz)count++;count--;
  while (sz>1)
  {
    for (int i = 0;i<width-1;i+=sz)
      for (int j = 0;j<height-1;j+=sz)
      {
        float a = heights[i*width+j];
        float b = heights[i*width+j+sz];
        float c = heights[(i+sz)*width+j];
        float d = heights[(i+sz)*width+j+sz];

        float hvar = 1.f/400.f;
        if (sz>width/4) hvar =0;
        heights[(i+sz/2)*width+(j+sz/2)] = (a+b+c+d)/4 + sz*hvar * (rand()%100-50)/100.f ;
        heights[(i+sz/2)*width+(j)] = (a+c)/2 + sz*hvar * (rand()%100-50)/100.f;
        heights[(i)*width+(j+sz/2)] = (a+b)/2 + sz*hvar * (rand()%100-50)/100.f;
        heights[(i+sz/2)*width+(j+sz)] = (b+d)/2 + sz*hvar * (rand()%100-50)/100.f;
        heights[(i+sz)*width+(j+sz/2)] = (c+d)/2 + sz*hvar * (rand()%100-50)/100.f;

      }
    sz /= 2;
    count--;
  }
  for (int i = 0;i<width;i++)
    for (int j = 0; j<height;j++)
      heights[i*width+j] = pow(abs((double)heights[i*width+j]),1.5)*0.7;
  return heights;
}