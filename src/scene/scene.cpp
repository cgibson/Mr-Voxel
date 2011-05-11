#include <stdio.h>
#include <string.h>
#include "scene.h"

#include "BBNode.h"

using namespace std;


/*
 * Collision data constructor including offending object, intersect point, 
 * distance and normal of face
 *----------------------------------------------------------------------------*/
Surface::Surface(const Vec3 &point, double distance, const Vec3 &normal, const Finish &fin, const Color &col)
{
  p = point;
  t = distance;
  n = normal;
  finish = fin;
  color = col;
  hit = true;
  objPtr = NULL;
}

/*
 * Constructs a default empty collision data object
 *----------------------------------------------------------------------------*/
Surface::Surface()
{
  hit = false;
  p = Vec3(0,0,0);
  t = -1;
  n = Vec3(0,1,0);
  objPtr = NULL;
}

/*
 * Scene Constructor
 *----------------------------------------------------------------------------*/
Scene::Scene( void )
{
  mIncrement = 10;
  mLightCount = 0;
  mLightCountMax = 0;
  mObjectCount = 0;
  mObjectCountMax = 0;
  mPlaneCount = 0;
  mPlaneCountMax = 0;
  mVolumeCount = 0;
  mVolumeCountMax = 0;

  mLights = (LightSource**)calloc(mLightCountMax, sizeof(LightSource*));
  mObjects = (GeomObj**)calloc(mObjectCountMax, sizeof(GeomObj*));
  mPlanes = (GeomObj**)calloc(mPlaneCountMax, sizeof(GeomObj*));
  mVolumes = (VolumeRegion**)calloc(mVolumeCountMax, sizeof(VolumeRegion*));
}

bool
Scene::intersect(const Ray &ray, Surface* const surface) {

    if(mObjectBVH == NULL)
        return false;

    return static_cast<BBNode*>(mObjectBVH)->intersect(ray, surface);
}

bool
Scene::intersections(const Ray &ray, vector<Surface> *surfaceList) {

    if(mObjectBVH == NULL)
        return false;

    return static_cast<BBNode*>(mObjectBVH)->intersections(ray, surfaceList);
}

BBNode construct_bb(vector<SceneObject*> objs)
{
  TYPE type;
  BBNode tmp_node, ret_node;
  int bb_built = false;

  vector<SceneObject*>::iterator it;

  for(it = objs.begin(); it != objs.end(); it++)
  {;
    type = (*it)->getType();
    if(type == NODE)
    {
      tmp_node = BBNode(((BBNode*)*it)->min, ((BBNode*)*it)->max);
    }else{
      tmp_node = ((GeomObj*)*it)->construct_bb();
    }
    if(!bb_built)
    {
      ret_node = tmp_node;
      bb_built = true;
    }else{
      ret_node = ret_node.combine(tmp_node);
    }

  }
  return ret_node;
}

/*
 * Split all SceneObjects down a given axis.  All objects are added to a 
 * parent bounding box and the box is split along the given axis
 *----------------------------------------------------------------------------*/
void split(vector<SceneObject*> A, vector<SceneObject*> *l1, vector<SceneObject*> *l2, AXIS axis)
{
  int i;
  int rotate = 0;
  Vec3 cur_center, list_center;
  BBNode node = construct_bb(A);
  list_center = node.getCenter();

  /*
  printf("node length: %s", (node.max - node.min).str());
  //*/
  //*
  float axisLen = 0.0;

  axis = YZ;
  axisLen = node.max.x() - node.min.x();

  float tmp;

  tmp = node.max.y() - node.min.y();
  if(tmp > axisLen)
  {
      axis = XZ;
      axisLen = tmp;
  }

  tmp = node.max.z() - node.min.z();
  if(tmp > axisLen)
  {
      axis = XY;
      axisLen = tmp;
  }

//*/

  /*
  switch(axis){
      case XY: printf(" GOING XY\n"); break;
      case XZ: printf(" GOING XZ\n"); break;
      case YZ: printf(" GOING YZ\n"); break;
  }
  //*/
  
  // For every object in the list
  for(i = 0; i < A.size(); i++)
  {
    // if the scene object is a node
    if(A[i]->getType() == NODE)
    {
      // retrieve the center of the given node
      cur_center = ((BBNode*)A[i])->getCenter();
    }else{
      // generate a bounding box and grab its center from the given object
      BBNode node = ((GeomObj*)A[i])->construct_bb();
      cur_center = node.getCenter();
    }
    
    // for each axis
    switch(axis)
    {
      // handle a cut along the z axis
      case XY:
        if(cur_center.z() < list_center.z())
          l1->push_back(A[i]);
        else if(cur_center.z() > list_center.z())
          l2->push_back(A[i]);
	// if the object falls in the middle of the axis
        else
        {
	  // rotate which list you hand off the center object to
          if(rotate % 2)
            l1->push_back(A[i]);
          else
            l2->push_back(A[i]);
          rotate++;
        }
        break;
      // handle a cut along the y axis
      case XZ:
        if(cur_center.y() < list_center.y())
          l1->push_back(A[i]);
        else if(cur_center.y() > list_center.y())
          l2->push_back(A[i]);
	// if the object falls in the middle of the axis
        else
        {
	  // rotate which list you hand off the center object to
          if(rotate % 2)
            l1->push_back(A[i]);
          else
            l2->push_back(A[i]);
          rotate++;
        }
        break;
      // handle a cut along the x axis
      case YZ:
        if(cur_center.x() < list_center.x())
          l1->push_back(A[i]);
        else if(cur_center.x() > list_center.x())
          l2->push_back(A[i]);
	// if the object falls in the middle of the axis
        else
        {
	  // rotate which list you hand off the center object to
          if(rotate % 2)
            l1->push_back(A[i]);
          else
            l2->push_back(A[i]);
          rotate++;
        }
        break;
    }
  }
  if(l1->size() == 0)
  {
      for(int i = 0; i < l2->size() / 2; i++)
      {
        l1->push_back(l2->back());
        l2->pop_back();
      }
  }else if(l2->size() == 0)
  {
      for(int i = 0; i < l1->size() / 2; i++)
      {
          l2->push_back(l1->back());
          l1->pop_back();
      }
  }
}

/*
 * Recurse through the list, spatially subdividing the objects and building 
 * a hierarchical box structure
 *----------------------------------------------------------------------------*/
BBNode *recurseHierarchy(vector<SceneObject*> A, AXIS axis)
{
  BBNode *node1, *node2;
  // if this is an empty list
  if(A.size() == 0)
  {
    // return null, we don't need this node
    return NULL;
    
  // if the list size is 1
  }else if(A.size() == 1)
  {
    // double check to make sure the object is not NULL
    if(A[0])
    {
      // create a simple node to encapsulate the object
      node1 = new BBNode(A[0]);
      return node1;
    }else{
      return NULL;
    }
  // if there are two elements in the list
  }else if(A.size() == 2)
  {
    // make sure both exist
    if(A[0] && A[1])
    {
      node1 = new BBNode(A[0], A[1]);
      return node1;
    
    // if only the first element exists
    }else if(A[0])
    {
      return new BBNode(A[0]);
    
    // if only the second element exists
    }else if(A[1])
    {
      return new BBNode(A[1]);
    }
    
    // return nothing.  no node needed
    return NULL;
  
  // otherwise, there are too many objects, split the list
  }else
  {
    vector<SceneObject*> l1, l2;
    
    // split the list along an axis
    split(A, &l1, &l2, axis);
    
    // rotate the axis so we don't always cute the same way
    
    switch(axis)
    {
      case XY:;
        axis = YZ;
      break;
      case YZ:
        axis = XZ;
      break;
      case XZ:
        axis = XY;
      break;
    }
    
    // recurse for both lists
    node1 = recurseHierarchy(l1, axis);
    node2 = recurseHierarchy(l2, axis);
    
    // generate a node for the two nodes
    if(node1 && node2)
      return new BBNode(node1, node2);
    else if(node1)
      return node1;
    else if(node2)
      return node2;
    else
      return NULL;
  }
}

/*
 * Generate the bounding box hierarchical structure
 *----------------------------------------------------------------------------*/
void Scene::generateBoxHierarchy( void )
{
  int i;
  TYPE type;
  BBNode *node;

  vector<SceneObject*> list;
  vector<SceneObject*> list_vol;
  
  // create object list
  for(i = 0; i < mObjectCount; i++)
  {
    if(mObjects[i] != NULL)
    {
        type = mObjects[i]->getType();
        if(type == PLANE)
        {
          addPlane(mObjects[i]);
        }else if(type == CONE){
	    }else{
		    node = new BBNode(mObjects[i]->construct_bb());
		    node->child_l = mObjects[i];
		    node->child_r = NULL;
		    list.push_back(node);
	    }
    }
  }

  for(i = 0; i < mVolumeCount; i++)
  {
    node = new BBNode(mVolumes[i]->bounds());
    node->child_l = mVolumes[i];
    node->child_r = NULL;
    list_vol.push_back(node);
  }

  printf("list size: %d\n", list_vol.size());

  //Generate Hierarchy
  BBNode *result = recurseHierarchy(list, XY);
  BBNode *result_vol = recurseHierarchy(list_vol, XY);

  setObjectBVH(result);
  setVolumeBVH(result_vol);

}

/*
 * Add a light source to the scene
 *----------------------------------------------------------------------------*/
void Scene::addLightSource( LightSource* light )
{
  if(mLightCount >= mLightCountMax)
  {
    mLightCountMax += mIncrement;
    mLights = (LightSource**)realloc(mLights, mLightCountMax * sizeof(LightSource*));
  }
  mLights[mLightCount++] = light;
}

/*
 * Add a set of objects to the scene
 *----------------------------------------------------------------------------*/
void Scene::addObjects( vector<GeomObj*> objects )
{
    for(int i = 0; i < objects.size(); i++) {
        addObject(objects[i]);
    }
}

/*
 * Add an object to the scene
 *----------------------------------------------------------------------------*/
void Scene::addObject( GeomObj* object )
{
  if(mObjectCount >= mObjectCountMax)
  {
    mObjectCountMax += mIncrement;
    mObjects = (GeomObj**)realloc(mObjects, mObjectCountMax * sizeof(GeomObj*));
  }
  mObjects[mObjectCount++] = object;
}

/*
 * Add an object to the scene
 *----------------------------------------------------------------------------*/
void Scene::addVolume( VolumeRegion* volume )
{
  if(mVolumeCount >= mVolumeCountMax)
  {
    mVolumeCountMax += mIncrement;
    mVolumes = (VolumeRegion**)realloc(mVolumes, mVolumeCountMax * sizeof(VolumeRegion*));
  }
  mVolumes[mVolumeCount++] = volume;
}

/*
 * Add an plane to the scene
 *----------------------------------------------------------------------------*/
void Scene::addPlane( GeomObj* object )
{
  if(mPlaneCount >= mPlaneCountMax)
  {
	  mPlaneCountMax += mIncrement;
	  mPlanes = (GeomObj**)realloc(mPlanes, mPlaneCountMax * sizeof(GeomObj*));
  }
  mPlanes[mPlaneCount++] = object;
}


/*
 * Default camera constructor
 *----------------------------------------------------------------------------*/
Camera::Camera( void )
{

}

/*
 * Camera to string function
 *----------------------------------------------------------------------------*/
char* Camera::str( void )
{
  char *buffer = (char*)calloc(300, sizeof(char));
  sprintf(buffer, "[CAMERA]\n\tPosition: %s\n\tUp: %s\n\tRight: %s\n\tLookat: %s",
          location.str(),
          up.str(),
          right.str(),
          look_at.str());
  return buffer;
}

Matrix
Camera::perspectiveMatrix() {
    
  Vec3 w = look_at - location;
  Vec3 u;
  Vec3 v;
  w.norm();
  w = w * -1;
  up.cross(w, &u);
  w.cross(u, &v);

  Matrix m1 = Matrix(1, 0, 0, location.x(),
                     0, 1, 0, location.y(),
                     0, 0, 1, location.z(),
                     0, 0, 0, 1);

  //cout << "Camera Loc: " << endl << m1 << endl;

  Matrix m2 = Matrix(u.x(), v.x(), w.x(), 0,
                     u.y(), v.y(), w.y(), 0,
                     u.z(), v.z(), w.z(), 0,
                     0, 0, 0, 1);

  //cout << "UVW: " << endl << m2 << endl;

  return m1.multRight(m2);
}

/*
 * Light source constructor
 *----------------------------------------------------------------------------*/
LightSource::LightSource( void )
{

}

/*
 * Light source to string function
 *----------------------------------------------------------------------------*/
char* LightSource::str( void )
{
  char *buffer = (char*)calloc(300, sizeof(char));
  sprintf(buffer, "[LIGHT SOURCE]\n\tPosition: %s\n\tColor: %s", position.str(), color.str());
  return buffer;
}

Color
LightSource::sample(Vec3 pt) {
    if(lightType == LIGHT_POINT)
        return color;

    Vec3 d = pt - position;
    d.norm();
    
    double theta = d.dot(dir);

    double angle = acos(theta);

    if(angle > fov_outer)
        return 0;
    else if(angle < fov_inner)
        return color;
    else {
        double ang_diff = angle - fov_inner;
        double ang_t = ang_diff / (fov_outer - fov_inner);
        return color * (1. - ang_t);
    }
}
