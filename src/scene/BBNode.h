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
  BBNode( Vec3 minimum, Vec3 maximum );
  virtual int test_intersect( Ray ray, double *t, Vec3 *n );
  virtual int test_intersect( Ray ray, double *t1, double *t2, Vec3 *n );
  BBNode construct( Matrix matrix );
  int inside( Vec3 );
  virtual char* str( void );
  char* recurse_str( void );
  virtual TYPE getType( void ){ return NODE; }
  double cost();
  BBNode combine( BBNode node );
  Vec3 getCenter();

  bool intersect(Ray ray, Surface* surface);

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
