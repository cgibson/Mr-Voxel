#ifndef BBNODE_H_
#define BBNODE_H_

#include "Object.h"

class Surface;

/*
 * Represents a bounding box node
 *----------------------------------------------------------------------------*/
class BBNode : public SceneObject{
public:
  BBNode( void );
  BBNode( SceneObject *o );
  BBNode( SceneObject *o1, SceneObject *o2 );
  BBNode( const Vec3 &minimum, const Vec3 &maximum );
  virtual int test_intersect( const Ray &ray, double *t, Vec3 * const n );
  virtual int test_intersect( const Ray &ray, double *t1, double *t2, Vec3 * const n );
  BBNode construct( const Matrix &matrix );
  int inside( Vec3 );
  virtual char* str( void );
  char* recurse_str( void );
  virtual inline TYPE getType( void ){ return NODE; }
  double cost();
  BBNode combine( const BBNode &node );
  Vec3 getCenter();

  bool intersect(const Ray &ray, Surface* surface);

  int has_right()
  {
    return child_r != NULL;
  }
  int has_left()
  {
    return child_l != NULL;
  }

  Vec3 min;
  Vec3 max;
  SceneObject *child_l, *child_r;

};

#endif /* BBNODE_H_ */
