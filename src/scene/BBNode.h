#ifndef BBNODE_H_
#define BBNODE_H_

#include "Object.h"
#include "../util/util.h"
#include "../util/MyMat.h"
#include "../util/rayutil.h"

/*
 * Represents a bounding box node
 *----------------------------------------------------------------------------*/
class BBNode : public SceneObject{
public:
  BBNode( void );
  BBNode( SceneObject *o );
  BBNode( SceneObject *o1, SceneObject *o2 );
  BBNode( Vector minimum, Vector maximum );
  virtual int test_intersect( Ray ray, double *t, Vector *n );
  virtual int test_intersect( Ray ray, double *t1, double *t2, Vector *n );
  BBNode construct( MyMat matrix );
  int inside( Vector );
  virtual char* str( void );
  char* recurse_str( void );
  virtual TYPE getType( void ){ return NODE; }
  double cost();
  BBNode combine( BBNode node );
  Vector getCenter();

  int has_right()
  {
    return child_r != NULL;
  }
  int has_left()
  {
    return child_l != NULL;
  }

  Vector min;
  Vector max;
  SceneObject *child_l, *child_r;

};

#endif /* BBNODE_H_ */
