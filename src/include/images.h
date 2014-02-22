#ifndef IMAGES_H
#define IMAGES_H

#include <magrathea.h>

/// Constructs a virgin texture
GLuint newTexture(bool smoothTexture);
/// Constructs an opengl texture from the given file
GLuint textureFromTGA(const char*,bool);
#endif
