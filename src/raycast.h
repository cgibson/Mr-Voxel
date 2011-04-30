#ifndef _RAYCAST_H_
#define _RAYCAST_H_

#include "scene/scene.h"
#include "util/image.h"
#include "sample/Sampler.h"
#include "system/config.h"
//#include "light/brdf.h"

#define ORTHOGRAPHIC 0
#define PERSPECTIVE 1
#define STOCRATIC 1

class Raycaster {
public:
  Raycaster( Scene* scene );
  Raycaster( Scene* scene, const Camera &cam );
  Color cast( int x, int y, int width, int height );
  void cam2World( int x, int y, int width, int height, Ray *ray );
  int iterate( Ray *ray, Surface *surface);
  Color handleIntersect( const Ray &ray, int depth );
  Color sumLights( const Surface &surface, const Ray &ray, int specular, int ambient, bool gather );
  Color initialCast( const Ray &ray, int depth );
  int raycast( 
            int start_x,
            int start_y,
            int end_x,
            int end_y,
            int width,
            int height,
            int depth,
            ImageWriter *writer);

  int surfelCast(
            const Dimension &size,
            int step_x,
            int step_y,
            ImageWriter *writer);
private:
  int mDepth;
  Scene *mScene;
  int mCastMode;
  int mAliasMode;
  double *mRandData;
  int mCurrentRand;
  double mNear;
  double mLeft, mRight;
  double mTop, mBottom;
  Camera _camera;
  MyMat _matrix;
};

#endif
