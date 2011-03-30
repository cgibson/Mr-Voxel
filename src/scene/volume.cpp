#include "volume.h"

#define SHORT_MAX 65535

int vol::recurse_vol_search( SceneObject* root, Ray ray, float t_max, std::vector<vol::volinfo>* list)
{
	Ray tmp_ray;
	double t = 0;
	double t2 = 0;
	Vec3 n;
	int hit;
	int hit_l = false;
	int hit_r = false;
	double epsilon = 0.001f;

    if(root == NULL)
        return false;

	TYPE type = root->getType();
	BBNode *node;

	if(type == NODE) {
		hit = root->test_intersect(ray, &t, &n);
		if(!hit)
			return false;

		node = (BBNode*)root;
		if(node == NULL){
		      printf("ERROR: cannot use this node!\n");
		      exit(-1);
		} else {
			if(node->has_left())
				hit_l = recurse_vol_search( node->child_l, ray, t_max, list);
			if(node->has_right())
				hit_l = recurse_vol_search( node->child_r, ray, t_max, list);
		}
		return hit_l || hit_r;

	// going to be volumes
	} else {

	    Volume *obj = (Volume*)root;
	    tmp_ray.start = obj->matrix * Vec4(ray.start, 1);
	    tmp_ray.direction = obj->matrix * Vec4(ray.direction, 0);

		hit = obj->test_intersect(tmp_ray, &t, &t2, &n);

		if(hit && (t2 > epsilon) && (t < t_max)) {
			list->push_back(vol::volinfo{obj, t, t2});
			return true;
		}else{
			return false;
		}
	}
}

void vol::ray_march( std::vector<SceneObject*> vlist, Ray ray, float t1, float t2)
{
	return;
}

void vol::integrate_volume( SceneObject* root, Ray ray_start, float t_max, Color* ret_color)
{
	float amount = 0.0;
	std::vector<volinfo> vlist;

	int hit;

	double tmp_t1, tmp_t2;

	vol::volinfo info;

	hit = recurse_vol_search( root, ray_start, t_max, &vlist);
	if(hit)
	{
		for(int i = 0; i < vlist.size(); i++) {
			info = vlist[i];
			tmp_t1 = info.t1;
			tmp_t2 = (info.t2 < t_max) ? info.t2 : t_max;
			amount += (tmp_t2 - tmp_t1);
		}
	}

	//amount = sqrt(amount*4.0);
	*ret_color = Color(0.2, 0.0, 0.0, (amount < 3.0) ? (amount/3.0) : 1.0);
}

NodeType
VNode::type()
{
  return _type;
}

VNode::VNode( NodeType type )
{
  _type = type;
}

/*VNode::VNode( NodeType type )//, Vec3 min, Vec3 max )
  //: Box(min, max)
{
  _type = type;
}*/

/*VBranch::VBranch()//Vec3 min, Vec3 max)
  : VNode(BranchNode)//, min, max)
{
  for ( int i = 0; i < 2; ++i ) {
    for ( int j = 0; j < 2; ++j ) {
      for ( int k = 0; k < 2; ++k ) {
        _children[i][j][k] = NULL;
      }
    }
  }
}*/

VBranch::VBranch()
  : VNode(BranchNode)
  , _children_count(0)
{
  for ( int i = 0; i < 2; ++i ) {
    for ( int j = 0; j < 2; ++j ) {
      for ( int k = 0; k < 2; ++k ) {
        _children[i][j][k] = NULL;
      }
    }
  }
}

VBranch::~VBranch()
{
  for ( int i = 0; i < 2; ++i ) {
    for ( int j = 0; j < 2; ++j ) {
      for ( int k = 0; k < 2; ++k ) {
        delete _children[i][j][k];
      }
    }
  }
}

void
VBranch::setNode(int i, int j, int k, VNode* node)
{
    
  if(_children[i][j][k] != NULL) {
    delete _children[i][j][k];
  }
  else {
    _children_count++;
  }
    
  _children[i][j][k] = node;
}

VLeaf::VLeaf(t_val value)
  : VNode(LeafNode)
{
  _value = value;
}

t_val
VLeaf::value()
{
  return _value;
}

void
VLeaf::setValue( t_val val )
{
  _value = val;
}

/*
 * Default Volume Constructor
 *----------------------------------------------------------------------------*/
Volume::Volume( void )
  : _root(NULL)
  , _size(-1)
{
}

/*
 * Default Volume Constructor with minimum and maximum points defined
 *----------------------------------------------------------------------------*/
Volume::Volume( Vec3 min, Vec3 max, int size )
  //: Box(min, max)
  : _root(NULL)
  , Box(min, max)
  , _size(size)
{
  _root = new VBranch();//min, max);

}

void
Volume::set(int x, int y, int z, double value)
{
    if(value < 0.0000000001)
      return;
    if(!_root)
        _root = new VBranch();//(min(), max());

    int size = _size;
    VNode **n = &_root;
    int tmp_x, tmp_y, tmp_z;

    while(size != 1)
    {
        if(!*n) {
            //printf("Creating new branch\n");
            *n = new VBranch();//(tmp_min, tmp_max);
        }
        else {
            size /= 2;
            tmp_x = !!(x & size);
            tmp_y = !!(y & size);
            tmp_z = !!(z & size);

              if(size == 1) {
                tmp_x = (x % 2);
                tmp_y = (y % 2);
                tmp_z = (z % 2);
                reinterpret_cast<VBranch*>(*n)->setNode(tmp_x, tmp_y, tmp_z, new VLeaf(value));
                return;
              }
              else {
                n = &reinterpret_cast<VBranch*>(*n)->child(tmp_x, tmp_y, tmp_z);
              }
        }
    }
}


Volume::~Volume()
{
  if(_root)
    delete _root;
}

/*
 * Volume to string function
 *----------------------------------------------------------------------------*/
char* Volume::str( void )
{
  char *buffer = (char*)calloc(300, sizeof(char));
  int count = 0;
  sprintf(buffer, "[VOLUME]\n\tCorner1: %s\n\tCorner2: %s\n\tSize: %d",
          min().str(), 
          max().str(),
          _size);

  if(modifiers != NULL)
  {
    for(count = 0; count < modifier_count; count++)
    {
      strcat(buffer, "\n\t");
      strcat(buffer, modifiers[count]->str());
    }
  }
  return buffer;
}


int Volume::test_intersect( Ray ray, double *t1, double *t2, Vec3 *n )
{
	  double near = -1000; double far = 1000;
	  double near_tmp, far_tmp;

	  double p[3] = {ray.start.x(), ray.start.y(), ray.start.z()};
	  double d[3] = {ray.direction.x(), ray.direction.y(), ray.direction.z()};
	  double tmin[3] = {_min.x(), _min.y(), _min.z()};
	  double tmax[3] = {_max.x(), _max.y(), _max.z()};
	  Vec3 tnorm[3] = {Vec3(1, 0, 0), Vec3(0, 1, 0), Vec3(0, 0, 1)};
	  Vec3 near_n, far_n;
	  Vec3 center = (_min + _max) / 2.0;
	  int r, i;

	  for(i = 0; i < 3; i++)
	  {
	    r = test_intersect_1d(p[i], d[i], tmin[i], tmax[i], &near_tmp, &far_tmp);
	    if(!r)
	      return false;

	    if(near_tmp > near)
	    {
	      near_n = tnorm[i];
	      near = near_tmp;
	    }

	    if(far_tmp < far)
	    {
	      far_n = tnorm[i];
	      far = far_tmp;
	    }
	  }


	  if(near > far) return false;
	  if(far < 0) return false;

	  Vec3 point;
	  Vec3 mult;
	  if(near < 0.0)
	  {
	    point = ray.start + (ray.direction * far);
	    mult = Vec3((point.x() < center.x()) ? -1 : 1,
	                  (point.y() < center.y()) ? -1 : 1,
	                  (point.z() < center.z()) ? -1 : 1
	                 );
	    *n = Vec3(far_n.x() * mult.x(),
	                far_n.y() * mult.y(),
	                far_n.z() * mult.z());
	    *t1 = 0.0;
	    *t2 = far;
	  }
	  else
	  {
	    point = ray.start + (ray.direction * near);
	    mult = Vec3((point.x() < center.x()) ? -1 : 1,
	                 (point.y() < center.y()) ? -1 : 1,
	                 (point.z() < center.z()) ? -1 : 1
	                 );
	    *n = Vec3(near_n.x() * mult.x(),
	                near_n.y() * mult.y(),
	                near_n.z() * mult.z());
	    *t1 = near;
	    *t2 = far;
	  }
	  //Vec3 mult = Vec3(1,1,1);


	  return true;
}


int Volume::test_intersect( Ray ray, double *t, Vec3 *n )
{
  //printf("testing... %d %s\n", _size, (_root==NULL ? "it's null" : "it's not null"));
  if(!_root || (_size < 2))
    return false;

  return Box(min(), max()).test_intersect(ray, t, n);
}

const VNode*
VBranch::child(int x, int y, int z)
const
{
    return _children[x][y][z];
}

VNode*&
VBranch::child(int x, int y, int z)
{
    if(!_children[x][y][z])
      _children_count++;
    return _children[x][y][z];
}
