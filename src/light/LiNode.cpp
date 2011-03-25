/* 
 * File:   Octree.cpp
 * Author: cgibson
 * 
 * Created on March 2, 2011, 7:05 PM
 */

#include "LiNode.h"
#include "../scene/Geometry.h"
#include "Surfel.h"

OctreeNode::OctreeNode( Vector min, Vector max ): _min(min), _max(max) {

    // Clear all children
    for(int i = 0; i < 8; i++)
        m_children[i] = NULL;
}

OctreeNode::OctreeNode(const OctreeNode& orig) {

    // Copy all children
    for(int i = 0; i < 8; i++)
        m_children[i] = orig.m_children[i];
}

OctreeNode::~OctreeNode() {
}

void
OctreeNode::cascadeDelete() {

    // Copy all children
    for(int i = 0; i < 8; i++) {
        if(m_children[i] != 0) {
            m_children[i]->cascadeDelete();
            delete m_children[i];
        }
    }
}

int
OctreeNode::test_intersect( Ray ray, double *t, Vector *n ) {

    BBNode node = BBNode(_min, _max);

    return node.test_intersect(ray, t, n);
    
  double near = -1000; double far = 1000;
  double near_tmp, far_tmp;

  double p[3] = {ray.start.x(), ray.start.y(), ray.start.z()};
  double d[3] = {ray.direction.x(), ray.direction.y(), ray.direction.z()};
  double tmin[3] = {_min.x(), _min.y(), _min.z()};
  double tmax[3] = {_max.x(), _max.y(), _max.z()};
  int r, i;

  for(i = 0; i < 3; i++)
  {
    r = test_intersect_1d(p[i], d[i], tmin[i], tmax[i], &near_tmp, &far_tmp);
    if(!r)
      return false;

    if(near_tmp > near)
    {
      near = near_tmp;
    }

    if(far_tmp < far)
    {
      far = far_tmp;
    }
  }

  if(near > far) return false;
  if(far < 0) return false;

  return true;
}

Color
LiNode::gather( Ray ray, double *t ) {
    double tmin = INFINITY;
    double thit;
    Color closest;
    Color tmp;
    Vector n;

/*
    Disk disk = Disk(1.0, 0.0, 2 * PI, Ray(Vector(1,2,0), Vector(0.0,0.0,-0.5)));//(*_surfelData)->normal()));
    //Sphere sphere;
    //sphere.center = (*_surfelData)->position();
    //sphere.radius = (*_surfelData)->area();
    //if(sphere.test_intersect(ray, &thit, &n)) {
        //printf("WHAT\n");
    Ray tmpRay;
    tmpRay.start = disk.matrix * Vector4(ray.start, 1);
    tmpRay.direction = disk.matrix * Vector4(ray.direction, 0);
    tmpRay.direction.norm();
    if(disk.test_intersect(tmpRay, &thit, &n)) {
        if(thit < tmin) {
            tmin = thit;
            closest = (*_surfelData)->diffuse();
        }
    }

    return closest;
*/

    //printf("GATHERING STAGE BEGIN\n");

    if(!OctreeNode::test_intersect( ray, &thit, &n )){
        //printf("O NOES... NO INTERSECT!\n");
        *t = INFINITY;
        return Color(0);
    }

    //printf("WE CAN HAS INTERSECT!\n");

    if(hasChildren()) {
        for(int i = 0; i < 8; i++) {
            tmp = child(i)->gather( ray, &thit);
            if(thit < tmin) {
                tmin = thit;
                closest = tmp;
            }
        }
    }else{
        /**/
        if((_min.x() < ray.start.x() && _max.x() > ray.start.x()) &&
           (_min.y() < ray.start.y() && _max.y() > ray.start.y()) &&
           (_min.z() < ray.start.z() && _max.z() > ray.start.z())) {
           *t = INFINITY;
           return Color(0);
        }
        //if(_surfelCount == 0) return Color(0,0,0.5);
        //printf("Testing all Disks (%d)!\n", _surfelCount);
        for(int i = 0; i < _surfelCount; i++) {
            //Disk disk = Disk((*_surfelData)->area(), 0, 2 * PI, Ray((*_surfelData)->position(), (*_surfelData)->normal()));
            //Sphere sphere;
            //sphere.center = (*_surfelData)->position();
            //sphere.radius = (*_surfelData)->area();
            //if(sphere.test_intersect(ray, &thit, &n)) {
                //printf("WHAT\n");
            Ray tmpRay;
            tmpRay.start = (*_surfelData)->matrix * Vector4(ray.start, 1);
            tmpRay.direction = (*_surfelData)->matrix * Vector4(ray.direction, 0);
            tmpRay.direction.norm();

            // Would be usefull if we wanted to suck at cheating
            //if((*_surfelData)->normal() * (ray.direction * -1) > 0) {
                if((*_surfelData)->test_intersect(tmpRay, &thit, &n)) {
                    if(thit < tmin) {
                        tmin = thit;
                        closest = (*_surfelData)->diffuse();
                    }
                }
            //}
        }
    }

    *t = tmin;

    return closest;
}

int
LiNode::subdivide() {

    //printf("Subdividing %s - %s\n", m_min.str(), m_max.str());
    Vector size = (_max - _min) * 0.5;

    if(size.length() < 0.00000001) {
        printf("SIZE: %d\n", _surfelCount);
        for(int i = 0; i < _surfelCount; i++) {
            Surfel surf = *_surfelData[i];
            printf("\t%s\n", surf.position().str());
        }
        printf("ERROR\n\tMIN: %s\n\tMAX: %s\n", _min.str(), _max.str());
        exit(1);
    }
    assert(size.length() > 0.00000001);
    Vector min, max;
    int x,y,z;

    // Loop over children
    for(int i = 0; i < 8; i++) {
        // Find out which child this is (in terms of what quadrant)
        x = i%2;
        y = (i/2)%2;
        z = (i/4);

        // Define minimum by which quadrant it is in
        min = Vector(_min.x() + (size.x() * x),
                     _min.y() + (size.y() * y),
                     _min.z() + (size.z() * z));

        // Max is always the minimum plus the size of the node
        max = min + size;

        
        m_children[i] = new LiNode(min, max);
    }

    int surfCount = _surfelCount;

    //_surfelCount = 0;

    // Iterate over all existing surfel data
    for(int i = 0; i < surfCount; i++) {
        //printf("Re-Adding %s!\n", _surfelData[i]->position().str());
        // Re-add the surfel, allowing it to recurse into the children
        add(_surfelData[i]);
    }
    // Clear all surfel data in this node
    return 0;
}

int
LiNode::size_of() {
    int size = 0;

    if(hasChildren()) {
        for(int i = 0; i < 8; i++) {
            size += child(i)->size_of();
        }
            size += sizeof(m_children);
            return size;
    }else{
        return sizeof(_surfelData) + (sizeof(shared_ptr<Surfel>) + sizeof(Surfel)) * _surfelCount;
    }

    return size;
}

int
LiNode::count() {
    int size = 0;

    if(hasChildren()) {
        for(int i = 0; i < 8; i++) {
            size += child(i)->count();
        }
        return size;
        
    }else{
        return _surfelCount;
    }
}

LiNode::LiNode(Vector min, Vector max):OctreeNode(min,max), _surfelCount(0) {
    
    // Allocate data enough for the maximum amount of surfels
    _surfelData = new shared_ptr<Surfel>[MAX_SURFEL_COUNT];

   // printf("GENERATING NODE: %s - %s\n", m_min.str(), m_max.str());
    
}

int
LiNode::add(shared_ptr<Surfel> obj) {

        //printf("Adding to NODE: %s - %s\n", m_min.str(), m_max.str());

    if(!inside(obj))
        return 0;
    
    // If this is a branch node
    if(hasChildren()) {
        //printf("Recursing\n");
        int res = 0;
        // Try adding it to every child node
        for(int i = 0; i < 8; i++) {
            
            res += child(i)->add(obj);
        }

        // Keep track of how many surfels are underneath a given node
        _surfelCount++;
        //printf("Surfel count: %d\n", _surfelCount);
        // Any non-zero failures will be recorded
        return res;

    }else{
        //printf("Hit a leaf node\n");
    // Otherwise we are a leaf node
        // Check if there are more surfels than we can hold
        if(_surfelCount >= MAX_SURFEL_COUNT) {
            subdivide();
            // Re-add this element.  It will recurse in this time
            return add(obj);
        }else{
            //printf("Adding %s!\n", obj->position().str());
            _surfelData[_surfelCount] = obj;
            _surfelCount++;
            //printf("Surfel count: %d\n", _surfelCount);

            return 0;
        }
    }
}


bool
LiNode::inside(shared_ptr<Surfel> obj) {
    float dmin = 0;

    Vector sphere_pos = obj->position();

    if(sphere_pos.x() > _min.x() && sphere_pos.x() < _max.x() &&
            sphere_pos.y() > _min.y() && sphere_pos.y() < _max.y() &&
            sphere_pos.z() > _min.z() && sphere_pos.z() < _max.z())
        return true;

    float c, bmin, bmax;
    for(int i = 0; i < 3; i++) {
        c = sphere_pos.get(i);
        bmin = _min.get(i);
        bmax = _max.get(i);
        if( c < bmin ) dmin += sqrt( c - bmin); else
        if( c > bmax ) dmin += sqrt( c - bmax);
    }
    if( dmin <= obj->area() ) return true;

    return false;
}


int
LiNode::clear() {
    if(_surfelData != NULL) {
        delete _surfelData;
    }
}