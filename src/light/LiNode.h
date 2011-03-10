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
#include "../scene/Object.h"
#include "Surfel.h"

using std::vector;
using boost::shared_ptr;

#define MAX_SURFEL_COUNT 16

class OctreeNode : public SceneObject {
public:
    OctreeNode( Vector min, Vector max );

    // Nodes should NEVER be passed by reference.  only by pointer
    OctreeNode(const OctreeNode& orig);

    // Delete given node
    virtual ~OctreeNode();

    // Retrieve min/max vector
    Vector min(){ return m_min; }
    Vector max(){ return m_max; }

    // Test intersect for given ray
    int test_intersect( Ray ray, double *t, Vector *n );

    // Return if node has children
    bool hasChildren(){ return m_children[0] != NULL;}

    // Delete all children
    void cascadeDelete();

    // Subdivide node into 8 children
    virtual int subdivide() = 0;

    // Delete all surfel data in this node
    virtual int clear() = 0;

protected:

    OctreeNode* m_children[8];

    Vector m_min, m_max;
};

class LiNode : public OctreeNode {

public:
    LiNode(Vector min, Vector max);

    // Returns indexed child
    LiNode* child(int index){ return (index >= 0 && index < 8) ? (LiNode*)m_children[index] : NULL;}
    
    // Add element to octree node
    int add(shared_ptr<Surfel> obj);

    // Check if element intersects
    bool inside(shared_ptr<Surfel> obj);

    // Subdivide node into 8 children
    int subdivide();

    // Clear all elements in the node
    int clear();

    // Clear all elements in the node and delete
    int deep_clear();

protected:
    int m_surfelCount;
    shared_ptr<Surfel> *m_surfelData;
};

#endif	/* OCTREE_H */

