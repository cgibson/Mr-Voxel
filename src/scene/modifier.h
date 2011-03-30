#ifndef _MODIFIER_H_
#define _MODIFIER_H_

#include <stdlib.h>
#include <stdio.h>
#include "../util/rayutil.h"
#include "../util/MyMat.h"

/*
 * Represents a generic modifier
 *----------------------------------------------------------------------------*/
class Modifier{
public:
  Modifier( void );
  virtual char* str( void );
  virtual Matrix matrix();
private:
};

/*
 * Represents a rotation applied to an object
 *----------------------------------------------------------------------------*/
class Rotation : public Modifier{
public:
  Rotation( void );
  Rotation( Vec3 rot ): rotation(rot){}
  virtual char* str( void );
  virtual Matrix matrix();

  Vec3 rotation;
};

/*
 * Represents a translation applied to an object
 *----------------------------------------------------------------------------*/
class Translation : public Modifier{
public:
  Translation( void );
  Translation( Vec3 trans ): translation(trans){}
  virtual char* str( void );
  virtual Matrix matrix();

  Vec3 translation;
};

/*
 * Represents a scale applied to an object
 *----------------------------------------------------------------------------*/
class Scale : public Modifier{
public:
  Scale( void );
  Scale( Vec3 sc): scale(sc){}
  virtual char* str( void );
  virtual Matrix matrix();

  Vec3 scale;
};

/*
 * Represents an object's pigment
 *----------------------------------------------------------------------------*/
class Pigment{
public:
  Pigment( void );
  virtual char* str( void );

  Color rgbf;
private:
};

/*
 * Represents an object's finish
 *----------------------------------------------------------------------------*/
class Finish{
public:
  Finish( void );
  virtual char* str( void );

  double ambient;
  double diffuse;
  double specular;
  double roughness;
  double reflection;
  double refraction;
  double ior;
private:
};

#endif
