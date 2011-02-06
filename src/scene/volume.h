#ifndef _VOLUME_H
#define	_VOLUME_H

#include <stdlib.h>
#include <vector>
#include "scene.h"

typedef unsigned short t_val;

enum NodeType{ BranchNode, LeafNode };

class Volume;

namespace vol
{
	typedef struct VOLUME_INFO {
		Volume* object;
		double t1, t2;
	} volinfo;

	extern int recurse_vol_search( SceneObject* root,
			Ray ray,
			float t_max,
			std::vector<vol::volinfo>* list);
	extern void ray_march( std::vector<SceneObject*> vlist, Ray ray, float t1, float t2);
	extern void integrate_volume( SceneObject* root, Ray ray_start, float t_max, Color* ret_color );

}

class VNode {// : public Box{
public:
  NodeType type();
  //VNode( NodeType type);//, Vector min, Vector max );
  VNode( NodeType type );
  ~VNode() {};

private:
  NodeType _type;
};

class VBranch : public VNode{
public:
  //VBranch(Vector min, Vector max);
  VBranch();
  ~VBranch();

  int test_intersect( Ray ray, double *t, Vector *n);

  void setNode(int i, int j, int k, VNode* node);

  const VNode* child(int x, int y, int z) const;
  VNode*& child(int x, int y, int z);

private:
  VNode* _children[2][2][2];
  char _children_count;
};

class VLeaf : public VNode{
public:
  //VLeaf(Vector min, Vector max);
  //VLeaf(Vector min, Vector max, double value);
  VLeaf(t_val value);
  ~VLeaf() {};

  t_val value();
  void setValue( t_val val );

private:
  t_val _value;
};



class Volume : public Box{
public:
  Volume( void );
  Volume( Vector min, Vector max, int size);
  ~Volume();
  virtual char* str( void );
  virtual TYPE getType( void ){ return VOLUME; }

  virtual int test_intersect( Ray ray, double *t, Vector *n);
  int test_intersect( Ray ray, double *t1, double *t2, Vector *n);
  double test_opacity( Ray ray );


    void set( int x, int y, int z, double value );


protected:
  //double getOpacity( Ray ray );

private:
  VNode* _root;
  int _size;
};

#endif

