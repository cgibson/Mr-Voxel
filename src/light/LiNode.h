/* 
 * File:   Octree.h
 * Author: cgibson
 *
 * Created on March 2, 2011, 7:05 PM
 */

#ifndef OCTREE_H
#define	OCTREE_H

#include <stdio.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/pointer_cast.hpp>

using std::vector;
using boost::shared_ptr;

#include "../scene/Object.h"
#include "Surfel.h"

namespace sys{};

using namespace sys;

class Surfel;

#define MAX_SAMPLE_COUNT 16

class OctreeNode : public SceneObject {
public:
    OctreeNode( const Vec3 &min, const Vec3 &max );

    // Nodes should NEVER be passed by reference.  only by pointer
    OctreeNode(const OctreeNode& orig);

    // Delete given node
    virtual ~OctreeNode();

    // Retrieve min/max vector
    Vec3 min(){ return _min; }
    Vec3 max(){ return _max; }

    // Test intersect for given ray
    inline int test_intersect( const Ray &ray, double *t, Vec3 * const n );

    // Return if node has children
    bool hasChildren(){ return _hasChildren;}

    // Delete all children
    void cascadeDelete();

    // Subdivide node into 8 children
    virtual int subdivide() = 0;

    // Delete all surfel data in this node
    virtual int clear() = 0;
    
    virtual void postprocess() = 0;

    virtual void cleanEmpty() = 0;
protected:

    bool _hasChildren;
    OctreeNode* m_children[8];

    Vec3 _min, _max;
};

class LiNode : public OctreeNode {

public:
    LiNode(Vec3 min, Vec3 max);

    // Returns indexed child
    LiNode* child(int index){ return (index >= 0 && index < 8) ? (LiNode*)m_children[index] : NULL;}
    
    // Add element to octree node
    int add(const shared_ptr<LiSample> obj);

    // Check if element intersects
    bool inside(const shared_ptr<LiSample> obj);

    // Subdivide node into 8 children
    int subdivide();

    // Clear all elements in the node
    int clear();

    // Clear all elements in the node and delete
    int deep_clear();

    int count();

    int size_of();

    Color gather(const Ray &ray, double *t);
    
    // Generate layers of multi-resolution lighting using spherical harmonics
    void postprocess();

    int getTestCount( const Ray &ray, int *testCount );

    void cleanEmpty();


protected:
    int _surfelCount;
    int _lvoxelCount;
    shared_ptr<Surfel> *_surfelData;

    SHCoef sh_c[9];
};

#endif	/* OCTREE_H */

