#ifndef _RAYCAST_H_
#define _RAYCAST_H_

#include "scene/scene.h"
#include "util/image.h"
#include "integrator/integrator.h"

#define ORTHOGRAPHIC 0
#define PERSPECTIVE 1
#define STOCRATIC 1

class Raycaster {
public:
  Raycaster( Scene* scene );
  int find_intersect(Ray ray, Surface *surface);
  Color cast( int x, int y, int width, int height );
  Color handleIntersect( Ray ray, int depth );
  Color sumLights( Surface surface, Ray ray );
  Color initialCast( Ray ray, int depth );
  int raycast( 
            int start_x,
            int start_y,
            int end_x,
            int end_y,
            int width,
            int height,
            int depth,
            ImageWriter *writer);
  int recurse_intersect(Ray ray, Surface *surface, SceneObject *parent);
  int intersect(Ray ray, Surface *surface);
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
  VolumeIntegrator *mVolumeIntegrator;
  Color background;
};

#endif
