#include <stdio.h>
#include <images.h>
#include <magrathea.h>

/// Genertate a new texture from scratch
/// @param smoothTexture Should the texture be smoothly interpolated?
GLuint newTexture(bool smoothTexture)
{
  GLuint returnValue;
  glGenTextures(1,&returnValue);
	glBindTexture(GL_TEXTURE_2D,returnValue);

  // Set the samplers for the smooth and solid versions
  if (smoothTexture)
  {
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  }
  else
  {
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  }

  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

  return returnValue;
}

GLuint textureFromTGA(const char* filePath, bool smoothTexture)
{
	logi.log("Loading texture %s",filePath);
	FILE* fp = fopen(filePath,"rb");
  if (fp==NULL)
    DIE2("Texture path doesn't exist:",filePath);
  unsigned char inp;
  fseek(fp,2,0);
  fread(&inp,1,1,fp);
  unsigned short width;
  unsigned short height;
  fseek(fp,12,0);
  fread(&inp,1,1,fp);
  width = inp;
  fread(&inp,1,1,fp);
  width += inp<<8;
  fread(&inp,1,1,fp);
  height = inp;
  fread(&inp,1,1,fp);
  height += inp<<8;

  fread(&inp,1,1,fp);
  if (inp!=32)
  {
    loge.log("Error loading texture:  All TGA files must be 32 bit.\n");
    return -1;
  }
  fread(&inp,1,1,fp);

  unsigned char* rgbaData = new unsigned char[width*height*4];
  fread(rgbaData,4,height*width,fp);
  for (int i = 0;i<width*height;i++)
  {
    unsigned char t = rgbaData[i*4 ];
    rgbaData[i*4     ] = rgbaData[i*4 + 2];
    rgbaData[i*4 + 2 ] = t;
  }
  GLuint returnValue = newTexture(smoothTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaData);

  delete rgbaData;
	fclose(fp);

	return returnValue;

}
