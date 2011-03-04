/* 
 * File:   Octree.h
 * Author: cgibson
 *
 * Created on March 2, 2011, 7:05 PM
 */

#ifndef OCTREE_H
#define	OCTREE_H

#define CACHE_LEAF_COUNT 32

class OctreeNode {
public:
    OctreeNode();
    OctreeNode(const OctreeNode& orig);
    virtual ~OctreeNode();

    bool hasChildren();
    void cascadeDelete();

    virtual void subdivide()= 0;
    virtual void combine()= 0;
private:

    OctreeNode* children[8];
};

class LightCacheNode : public OctreeNode {

};

namespace octree {



}

#endif	/* OCTREE_H */

