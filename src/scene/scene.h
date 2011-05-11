#ifndef _SCENE_H_
#define _SCENE_H_

#include <boost/shared_ptr.hpp>
#include "../mrvoxel/VolumeRegion.h"
#include "../types.h"
#include "modifier.h"
#include "Geometry.h"
#include "../light/LiNode.h"
#include "../light/LVoxel.h"
#include "../light/Surfel.h"

using namespace std;

enum AXIS {XY, YZ, XZ};

class BBNode;

class Camera : public Object{
public:
  Camera( void );
  virtual char* str( void );
  Matrix perspectiveMatrix();

  Vec3 location;
  Vec3 up;
  Vec3 right;
  Vec3 look_at;
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
  virtual Color sample(Vec3 pt);

  Vec3 position;
  Color color;
  light_type_t lightType;
  Vec3 dir;
  double fov_inner, fov_outer;

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
  Vec3 p;
  double t;
  Vec3 n;
  Finish finish;
  Color color;
  SurfaceType type;
  GeomObj *objPtr;
  
  Surface();
  Surface(const Vec3 &point, double distance, const Vec3 &normal, const Finish &fin, const Color &col);
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
  int addLVoxel( shared_ptr<LVoxel> obj ){ return (mLiCache != NULL) ? mLiCache->add(obj) : -1; }
  LiNode* initCache(Vec3 min, Vec3 max){ mLiCache = new LiNode(min, max); }

  bool intersect(const Ray &ray, Surface * const surface);
  bool intersections(const Ray &ray, vector<Surface> *surfaceList);

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
