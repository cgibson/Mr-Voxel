#ifndef OBJECT_H_
#define OBJECT_H_

#include "../util/util.h"
#include "../util/rayutil.h"

enum TYPE {PLANE, SPHERE, BOX, TRIANGLE, NODE, GEOM, CONE, VOLUME, UNDEF, DISK};

/*
 * Represents a generic scene object
 *----------------------------------------------------------------------------*/
class Object{
public:
  Object( void );
  virtual char* str( void );

private:
};

class SceneObject : public Object{
public:
  SceneObject( void );
  virtual int test_intersect( Ray ray, double *t, Vector *n )
    { return false; }
  virtual TYPE getType( void ){ return UNDEF; }
};

#endif /* OBJECT_H_ */
