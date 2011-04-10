#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "Object.h"
#include "BBNode.h"
#include "modifier.h"
#include "../util/MyMat.h"


/*
 * Represents a geometric scene object
 *----------------------------------------------------------------------------*/
class GeomObj : public SceneObject{
public:
  GeomObj( void );
  virtual char* str( void );
  virtual int test_intersect( Ray ray, double *t, Vec3 *n );
  virtual Vec3 get_normal( Vec3 pt );
  virtual BBNode construct_bb( void );
  virtual inline TYPE getType( void ){ return GEOM; }
  void generateMatrix();

  Pigment pigment;
  Finish finish;
  Matrix matrix;

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
  Box( Vec3 minimum, Vec3 maximum );
  virtual char* str( void );
  int test_intersect( Ray ray, double *t, Vec3 *n);
  Vec3 get_normal( Vec3 pt );
  BBNode construct_bb( void );
  virtual inline TYPE getType( void ){ return BOX; }

  Vec3 min(){ return _min; }
  Vec3 max(){ return _max; }
  void min(Vec3 min){ _min = min; }
  void max(Vec3 max){ _max = max; }

protected:
  Vec3 _min;
  Vec3 _max;
};



/*
 * Represents a box
 *----------------------------------------------------------------------------*/
class Disk : public GeomObj{
public:
  Disk( float height, float radius, float innerRadius, float tmax);
  Disk(float radius, float innerRadius, float tmax, Ray orient);
  virtual char* str( void );
  int test_intersect( Ray ray, double *t, Vec3 *n);
  Vec3 get_normal( Vec3 pt );
  BBNode construct_bb( void );
  virtual inline TYPE getType( void ){ return DISK; }


protected:
    float _height;
    float _radius;
    float _innerRadius;
    float _phiMax;
};

/*
 * Represents a sphere
 *----------------------------------------------------------------------------*/
class Sphere : public GeomObj{
public:
  Sphere( void );
  virtual char* str( void );
  virtual int test_intersect( Ray ray, double *t, Vec3 *n);
  virtual Vec3 get_normal( Vec3 pt );
  virtual BBNode construct_bb( void );
  virtual inline TYPE getType( void ){ return SPHERE; }

  Vec3 center;
  double radius;
};

/*
 * Represents a cone
 *----------------------------------------------------------------------------*/
class Cone : public GeomObj{
public:
  Cone( void );
  virtual char* str( void );
  virtual int test_intersect( Ray ray, double *t, Vec3 *n);
  virtual Vec3 get_normal( Vec3 pt );
  virtual BBNode construct_bb( void );
  virtual inline TYPE getType( void ){ return CONE; }

  Vec3 end1;
  Vec3 end2;
  double radius1;
  double radius2;
};

/*
 * Represents a triangle
 *----------------------------------------------------------------------------*/
class Triangle : public GeomObj{
public:
  Triangle( void );
  Triangle( Vec3 c1, Vec3 c2, Vec3 c3 );
  virtual char* str( void );
  virtual int test_intersect( Ray ray, double *t, Vec3 *n);
  virtual Vec3 get_normal( Vec3 pt );
  virtual BBNode construct_bb( void );
  virtual inline TYPE getType( void ){ return TRIANGLE; }

  Vec3 corner1;
  Vec3 corner2;
  Vec3 corner3;
};

/*
 * Represents a plane
 *----------------------------------------------------------------------------*/
class Plane : public GeomObj{
public:
  Plane( void );
  virtual char* str( void );
  virtual int test_intersect( Ray ray, double *t, Vec3 *n);
  virtual Vec3 get_normal( Vec3 pt );
  virtual BBNode construct_bb( void );
  virtual inline TYPE getType( void ){ return PLANE; }

  Vec3 normal;
  double distance;
};



#endif /* GEOMETRY_H_ */
