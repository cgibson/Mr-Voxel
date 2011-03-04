/* 
 * File:   Octree.cpp
 * Author: cgibson
 * 
 * Created on March 2, 2011, 7:05 PM
 */

#include "Octree.h"

OctreeNode::OctreeNode() {

    // Clear all children
    for(int i = 0; i < 8; i++)
        children[i] = 0;
}

OctreeNode::OctreeNode(const OctreeNode& orig) {

    // Copy all children
    for(int i = 0; i < 8; i++)
        children[i] = orig.children[i];
}

OctreeNode::~OctreeNode() {
}

void
OctreeNode::cascadeDelete() {

    // Copy all children
    for(int i = 0; i < 8; i++)
        if(children[i] != 0) {
            children[i]->cascadeDelete();
            delete children[i];
        }

}

bool
OctreeNode::hasChildren() {
    return (children[0] != 0);
}