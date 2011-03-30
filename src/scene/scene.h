#ifndef _SCENE_H_
#define _SCENE_H_

#include <boost/shared_ptr.hpp>
#include "../mrvoxel/VolumeRegion.h"
#include "../util/util.h"
#include "../util/MyMat.h"
#include "modifier.h"
#include "BBNode.h"
#include "Geometry.h"
#include "../light/LiNode.h"
#include "../light/Surfel.h"

using namespace std;

enum AXIS {XY, YZ, XZ};

class Camera : public Object{
public:
  Camera( void );
  virtual char* str( void );
  MyMat perspectiveMatrix();

  Vector location;
  Vector up;
  Vector right;
  Vector look_at;
  double fov;
  double fov_ratio;

private:
};

/*
 * Represents a light source in the scene
 *----------------------------------------------------------------------------*/
class LightSource : public Object{
public:
  LightSource( void );
  virtual char* str( void );
  
  Vector position;
  Color color;

private:
};

/*
 * Represents a list that handles scene objects
 *----------------------------------------------------------------------------*/
class ObjectList {
public:
  ObjectList();
  void add(SceneObject *node);
  SceneObject *get(int index);
  SceneObject *remove(int index);
  BBNode construct_bb();
  int size() { return mSize; }
  
  SceneObject *operator[] (unsigned i);
  
private:
  void resize();
  int mSize;
  int mIncrement;
  int mMaxSize;
  SceneObject **mData;
};


enum SurfaceType{ SolidSurface, VolumeSurface };

/*
 * Represents a collision through an intersection of a ray and geometric object
 *----------------------------------------------------------------------------*/
class Surface {
public:
  Vector p;
  double t;
  Vector n;
  Finish finish;
  Color color;
  SurfaceType type;
  GeomObj *objPtr;
  
  Surface();
  Surface(Vector point, double distance, Vector normal, Finish fin, Color col);
  bool isHit( void ){ return hit; }
private:
  bool hit;
};

/*
 * Represents the entire scene as a set of allocated memory for every scene
 * element
 *----------------------------------------------------------------------------*/
class Scene {
public:
  Scene();

  GeomObj** getObjects( void ){ return mObjects; }
  VolumeRegion** getVolumes( void ){ return mVolumes; }
  GeomObj** getPlanes( void ){ return mPlanes; }
  LightSource** getLightSources( void ){ return mLights; }
  void setCamera( Camera* camera ){ mCamera = camera; }
  Camera* getCamera( void ){ return mCamera; }
  int getObjectCount( void ){ return mObjectCount; }
  int getVolumeCount( void ){ return mVolumeCount; }
  int getPlaneCount( void ){ return mPlaneCount; }
  int getLightSourceCount( void ){ return mLightCount; }
  void setObjectBVH( SceneObject* bvh ){ mObjectBVH = bvh; }
  void setVolumeBVH( SceneObject* bvh ){ mVolumeBVH = bvh; }
  SceneObject* getObjectBVH( void ){ return mObjectBVH; }
  SceneObject* getVolumeBVH( void ){ return mVolumeBVH; }

  void generateBoxHierarchy();
  void addObject( GeomObj* object );
  void addObjects( vector<GeomObj*> objects );
  void addVolume( VolumeRegion* volume );
  void addPlane( GeomObj* object );
  void addLightSource( LightSource* light );

  double getVolSampleStep(){ return mVolSampleStep; }
  double setVolSampleStep( double sampleStep ){ mVolSampleStep = sampleStep; }

  LiNode* lightCache(){ return mLiCache; }
  int addSurfel( shared_ptr<Surfel> obj ){ return (mLiCache != NULL) ? mLiCache->add(obj) : -1; }
  LiNode* initCache(Vector min, Vector max){ mLiCache = new LiNode(min, max); }

  bool intersect(Ray ray, Surface *surface);

private:

  int mVolumeCount;
  int mVolumeCountMax;
  int mObjectCount;
  int mObjectCountMax;
  int mLightCount;
  int mLightCountMax;
  int mIncrement;
  int mPlaneCount;
  int mPlaneCountMax;
  LightSource** mLights;
  SceneObject* mObjectBVH;
  SceneObject* mVolumeBVH;
  GeomObj** mPlanes;
  GeomObj** mObjects;
  VolumeRegion** mVolumes;
  Camera* mCamera;
  LiNode *mLiCache;

  double mVolSampleStep;
};
#endif
