#ifndef OBJECT_H_
#define OBJECT_H_

#include "../util/MyMat.h"
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
  virtual int test_intersect( Ray ray, double *t, Vec3 *n )
    { return false; }
  virtual inline TYPE getType( void ){ return UNDEF; }
};

#endif /* OBJECT_H_ */
