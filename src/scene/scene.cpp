#include <stdio.h>
#include <string.h>
#include "scene.h"

using namespace std;


/*
 * Collision data constructor including offending object, intersect point, 
 * distance and normal of face
 *----------------------------------------------------------------------------*/
Surface::Surface(Vector point, double distance, Vector normal, Finish fin, Color col)
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
  p = Vector(0,0,0);
  t = -1;
  n = Vector(0,1,0);
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

/*
 * Split all SceneObjects down a given axis.  All objects are added to a 
 * parent bounding box and the box is split along the given axis
 *----------------------------------------------------------------------------*/
void split(ObjectList A, ObjectList *l1, ObjectList *l2, AXIS axis)
{
  int i;
  int rotate = 0;
  Vector cur_center, list_center;
  BBNode node = A.construct_bb();
  list_center = node.getCenter();

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
          l1->add(A[i]);
        else if(cur_center.z() > list_center.z())
          l2->add(A[i]);
	// if the object falls in the middle of the axis
        else
        {
	  // rotate which list you hand off the center object to
          if(rotate % 2)
            l1->add(A[i]);
          else
            l2->add(A[i]);
          rotate++;
        }
        break;
      // handle a cut along the y axis
      case XZ:
        if(cur_center.y() < list_center.y())
          l1->add(A[i]);
        else if(cur_center.y() > list_center.y())
          l2->add(A[i]);
	// if the object falls in the middle of the axis
        else
        {
	  // rotate which list you hand off the center object to
          if(rotate % 2)
            l1->add(A[i]);
          else
            l2->add(A[i]);
          rotate++;
        }
        break;
      // handle a cut along the x axis
      case YZ:
        if(cur_center.x() < list_center.x())
          l1->add(A[i]);
        else if(cur_center.x() > list_center.x())
          l2->add(A[i]);
	// if the object falls in the middle of the axis
        else
        {
	  // rotate which list you hand off the center object to
          if(rotate % 2)
            l1->add(A[i]);
          else
            l2->add(A[i]);
          rotate++;
        }
        break;
    }
  }
}

/*
 * Recurse through the list, spatially subdividing the objects and building 
 * a hierarchical box structure
 *----------------------------------------------------------------------------*/
BBNode *recurseHierarchy(ObjectList A, AXIS axis)
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
    ObjectList l1, l2;
    
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
  int i, tmp;
  int cur_node = 0;
  int best_node1, best_node2;
  BBNode *best_node1_ptr, *best_node2_ptr;
  double tmp_cost;
  double best_cost;
  TYPE type;
  BBNode *node;
  int tmp_node;

  ObjectList list;
  ObjectList list_vol;
  
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
		    list.add(node);
	    }
    }
  }

  for(i = 0; i < mVolumeCount; i++)
  {
    node = new BBNode(mVolumes[i]->bounds());
    node->child_l = mVolumes[i];
    node->child_r = NULL;
    list_vol.add(node);
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
 * Recursively test all objects within the given bounding box hierarchy
 *----------------------------------------------------------------------------*/
int Scene::recurse_intersect(Ray ray, Surface *surface, SceneObject *parent)
{
  if(parent == NULL)
    return false;

  Ray tmp_ray;
  Surface col_l, col_r;
  double t = 0;
  Vector n;
  int hit;
  int hit_l = false;
  int hit_r = false;
  double epsilon = 0.001f;
  TYPE type = parent->getType();
  BBNode *node;

  // if the object is a node
  if(type == NODE) {
    // check to make sure it hits
    hit = parent->test_intersect(ray, &t, &n);

    // return false if no hit, no need to delve deeper
    if(!hit)
      return false;

    node = (BBNode*)parent;
    if(node == NULL) {
      printf("ERROR: cannot use this node!\n");
      exit(-1);
    }
    else {
      // recurse into the left child
      if(node->has_left())
        hit_l = recurse_intersect(ray, &col_l, node->child_l);

      // recurse into the right child
      if(node->has_right())
        hit_r = recurse_intersect(ray, &col_r, node->child_r);
    }

    // if both children return a hit
    if(hit_l && hit_r) {
      // compare the hit values and return the closest
      if(col_l.t + epsilon < col_r.t) {
        *surface = col_l;
      }
      else {
        *surface = col_r;
      }
    // if the left child is hit
    }
    else if(hit_l) {
      *surface = col_l;
    // if the right child is hit
    }
    else if(hit_r) {
      *surface = col_r;
    }
    else {
      // no one was hit
      return false;
    }
    return true;

  // otherwise, the current node is a scene object
  }
  else if(type == VOLUME) {
	  printf("ERROR: Intersecting volume is deprecated.");
	  exit(-1);
    // create a temporary ray to avoid clipping issues
    GeomObj *obj = (GeomObj*)parent;
    tmp_ray.start = obj->matrix * Vector4(ray.start, 1);
    tmp_ray.direction = obj->matrix * Vector4(ray.direction, 0);

    // test to see if the volume is hit
    hit = parent->test_intersect(tmp_ray, &t, &n);

    if(hit && (t > epsilon)) {
      surface->t = t;
      surface->n =  ((GeomObj*)parent)->matrix.trans() * Vector4(n,1);
      surface->n.norm();
      surface->p = ray.start + (ray.direction * t);
      surface->finish = ((GeomObj*)parent)->finish;
      surface->color = ((GeomObj*)parent)->pigment.rgbf;
      surface->type = VolumeSurface;
      surface->objPtr = obj;
      return true;
    }
    return false;
  }
  else {
    // create a temporary ray to avoid clipping issues
    GeomObj *obj = (GeomObj*)parent;
    tmp_ray.start = obj->matrix * Vector4(ray.start, 1);
    tmp_ray.direction = obj->matrix * Vector4(ray.direction, 0);

    // test the scene object's intersect
    hit = parent->test_intersect(tmp_ray, &t, &n);

    // if the value is closer than the 'closest'
    if(hit && (t > epsilon)) {
      surface->t = t;
      surface->n = ((GeomObj*)parent)->matrix.trans() * Vector4(n,1);
      surface->n.norm();
      surface->p = ray.start + (ray.direction * t);
      surface->finish = ((GeomObj*)parent)->finish;
      surface->color = ((GeomObj*)parent)->pigment.rgbf;
      surface->type = SolidSurface;
      surface->objPtr = obj;
      return true;
    }
    return false;
  }
}



/*
 * Intersect given a ray
 *----------------------------------------------------------------------------*/
int Scene::intersect(Ray ray, Surface *surface)
{
	SceneObject *root = mObjectBVH;
	GeomObj **planes = mPlanes;
	SceneObject *cur;
	int plane_count = mPlaneCount;
	double epsilon = 0.001f;

	Surface closest, tmp_surf;
	double closest_dist;

	int hit;
	int has_hit = false;

	ray.start = ray.start + ray.direction * epsilon * 10;

	int i;
	// for every plane
	for(i = 0; i < plane_count; i++) {

		cur = planes[i];

		// check to see if the plane hits
		hit = recurse_intersect(ray, &tmp_surf, cur);
		if(hit) {
			// if the current intersection is the closest (or first)
			if((!closest.isHit()) ||
			(closest.t + epsilon > tmp_surf.t)) {
				// set the intersect information as the 'closest'
				closest = tmp_surf;
				has_hit = true;
			}
		}
	}

	// recurse through the root
	hit = recurse_intersect(ray, &tmp_surf, root);
	if(hit) {
	  // if the current intersection is the closest (or first)
	  if((!closest.isHit()) ||
		 (closest.t + epsilon > tmp_surf.t)) {
	// set the intersect information as the 'closest'
		closest = tmp_surf;
		has_hit = true;
	  }
	}


  *surface = closest;
  return has_hit;
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
