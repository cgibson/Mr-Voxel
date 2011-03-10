/* 
 * File:   Octree.cpp
 * Author: cgibson
 * 
 * Created on March 2, 2011, 7:05 PM
 */

#include "LiNode.h"

OctreeNode::OctreeNode( Vector min, Vector max ): m_min(min), m_max(max) {

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
    return 0;
}

int
LiNode::subdivide() {

    //printf("Subdividing %s - %s\n", m_min.str(), m_max.str());
    Vector size = (m_max - m_min) * 0.5;
    assert(size.length() > 0.0000001);
    Vector min, max;
    int x,y,z;

    // Loop over children
    for(int i = 0; i < 8; i++) {
        // Find out which child this is (in terms of what quadrant)
        x = i%2;
        y = (i/2)%2;
        z = (i/4);

        // Define minimum by which quadrant it is in
        min = Vector(m_min.x() + (size.x() * x),
                     m_min.y() + (size.y() * y),
                     m_min.z() + (size.z() * z));

        // Max is always the minimum plus the size of the node
        max = min + size;

        
        m_children[i] = new LiNode(min, max);
    }

    int surfCount = m_surfelCount;

    m_surfelCount = 0;

    // Iterate over all existing surfel data
    for(int i = 0; i < surfCount; i++) {
        //printf("Re-Adding %s!\n", m_surfelData[i]->position().str());
        // Re-add the surfel, allowing it to recurse into the children
        add(m_surfelData[i]);
    }

    // Clear all surfel data in this node
    return 0;
}

LiNode::LiNode(Vector min, Vector max):OctreeNode(min,max), m_surfelCount(0) {
    
    // Allocate data enough for the maximum amount of surfels
    m_surfelData = new shared_ptr<Surfel>[MAX_SURFEL_COUNT];

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
        m_surfelCount++;
        //printf("Surfel count: %d\n", m_surfelCount);
        // Any non-zero failures will be recorded
        return res;

    }else{
        //printf("Hit a leaf node\n");
    // Otherwise we are a leaf node
        // Check if there are more surfels than we can hold
        if(m_surfelCount >= MAX_SURFEL_COUNT) {
            subdivide();
            // Re-add this element.  It will recurse in this time
            return add(obj);
        }else{
            //printf("Adding %s!\n", obj->position().str());
            m_surfelData[m_surfelCount] = obj;
            m_surfelCount++;
            //printf("Surfel count: %d\n", m_surfelCount);

            return 0;
        }
    }
}


bool
LiNode::inside(shared_ptr<Surfel> obj) {
    float dmin = 0;

    Vector sphere_pos = obj->position();

    if(sphere_pos.x() > m_min.x() && sphere_pos.x() < m_max.x() &&
            sphere_pos.y() > m_min.y() && sphere_pos.y() < m_max.y() &&
            sphere_pos.z() > m_min.z() && sphere_pos.z() < m_max.z())
        return true;

    float c, bmin, bmax;
    for(int i = 0; i < 3; i++) {
        c = sphere_pos.get(i);
        bmin = m_min.get(i);
        bmax = m_max.get(i);
        if( c < bmin ) dmin += sqrt( c - bmin); else
        if( c > bmax ) dmin += sqrt( c - bmax);
    }
    if( dmin <= obj->area() ) return true;

    return false;
}


int
LiNode::clear() {
    if(m_surfelData != NULL) {
        delete m_surfelData;
    }
}