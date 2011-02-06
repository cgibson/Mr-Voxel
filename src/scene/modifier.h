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
  virtual MyMat matrix();
private:
};

/*
 * Represents a rotation applied to an object
 *----------------------------------------------------------------------------*/
class Rotation : public Modifier{
public:
  Rotation( void );
  Rotation( Vector rot ): rotation(rot){}
  virtual char* str( void );
  virtual MyMat matrix();

  Vector rotation;
};

/*
 * Represents a translation applied to an object
 *----------------------------------------------------------------------------*/
class Translation : public Modifier{
public:
  Translation( void );
  Translation( Vector trans ): translation(trans){}
  virtual char* str( void );
  virtual MyMat matrix();

  Vector translation;
};

/*
 * Represents a scale applied to an object
 *----------------------------------------------------------------------------*/
class Scale : public Modifier{
public:
  Scale( void );
  Scale( Vector sc): scale(sc){}
  virtual char* str( void );
  virtual MyMat matrix();

  Vector scale;
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
