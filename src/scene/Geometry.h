#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "Object.h"
#include "BBNode.h"
#include "modifier.h"
#include "../util/MyMat.h"
#include <glm/glm.hpp>


/*
 * Represents a geometric scene object
 *----------------------------------------------------------------------------*/
class GeomObj : public SceneObject{
public:
  GeomObj( void );
  virtual char* str( void );
  virtual int test_intersect( Ray ray, double *t, Vector *n );
  virtual Vector get_normal( Vector pt );
  virtual BBNode construct_bb( void );
  virtual TYPE getType( void ){ return GEOM; }
  void generateMatrix();

  Pigment pigment;
  Finish finish;
  MyMat matrix;

  Modifier** modifiers;
  int modifier_count;

private:
};



/*
 * Represents a box
 *----------------------------------------------------------------------------*/
class Box : public GeomObj{
public:
  Box( void );
  Box( Vector minimum, Vector maximum );
  virtual char* str( void );
  int test_intersect( Ray ray, double *t, Vector *n);
  Vector get_normal( Vector pt );
  BBNode construct_bb( void );
  virtual TYPE getType( void ){ return BOX; }

  Vector min(){ return _min; }
  Vector max(){ return _max; }
  void min(Vector min){ _min = min; }
  void max(Vector max){ _max = max; }

protected:
  Vector _min;
  Vector _max;
};



/*
 * Represents a box
 *----------------------------------------------------------------------------*/
class Disk : public GeomObj{
public:
  Disk( float height, float radius, float innerRadius, float tmax);
  Disk(float radius, float innerRadius, float tmax, Ray orient);
  virtual char* str( void );
  int test_intersect( Ray ray, double *t, Vector *n);
  Vector get_normal( Vector pt );
  BBNode construct_bb( void );
  virtual TYPE getType( void ){ return DISK; }


protected:
    float _height;
    float _radius;
    float _innerRadius;
    float _phiMax;
};

#include "volume.h"
/*
 * Represents a sphere
 *----------------------------------------------------------------------------*/
class Sphere : public GeomObj{
public:
  Sphere( void );
  virtual char* str( void );
  virtual int test_intersect( Ray ray, double *t, Vector *n);
  virtual Vector get_normal( Vector pt );
  virtual BBNode construct_bb( void );
  virtual TYPE getType( void ){ return SPHERE; }

  Vector center;
  double radius;
};

/*
 * Represents a cone
 *----------------------------------------------------------------------------*/
class Cone : public GeomObj{
public:
  Cone( void );
  virtual char* str( void );
  virtual int test_intersect( Ray ray, double *t, Vector *n);
  virtual Vector get_normal( Vector pt );
  virtual BBNode construct_bb( void );
  virtual TYPE getType( void ){ return CONE; }

  Vector end1;
  Vector end2;
  double radius1;
  double radius2;
};

/*
 * Represents a triangle
 *----------------------------------------------------------------------------*/
class Triangle : public GeomObj{
public:
  Triangle( void );
  Triangle( Vector c1, Vector c2, Vector c3 );
  virtual char* str( void );
  virtual int test_intersect( Ray ray, double *t, Vector *n);
  virtual Vector get_normal( Vector pt );
  virtual BBNode construct_bb( void );
  virtual TYPE getType( void ){ return TRIANGLE; }

  Vector corner1;
  Vector corner2;
  Vector corner3;
};

/*
 * Represents a plane
 *----------------------------------------------------------------------------*/
class Plane : public GeomObj{
public:
  Plane( void );
  virtual char* str( void );
  virtual int test_intersect( Ray ray, double *t, Vector *n);
  virtual Vector get_normal( Vector pt );
  virtual BBNode construct_bb( void );
  virtual TYPE getType( void ){ return PLANE; }

  Vector normal;
  double distance;
};



#endif /* GEOMETRY_H_ */
