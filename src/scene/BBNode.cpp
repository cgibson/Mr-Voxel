#include "BBNode.h"
#include "Geometry.h"

/*
 * Default BBNode Constructor
 *----------------------------------------------------------------------------*/
BBNode::BBNode( void )
{
  child_l = NULL;
  child_r = NULL;
}

/*
 * Default BBNode Constructor, built to encompass a given object
 *----------------------------------------------------------------------------*/
BBNode::BBNode( SceneObject *o )
{
  if(o->getType() == NODE)
  {
    BBNode *node = (BBNode*)o;
    min = node->min;
    max = node->max;
  }else{
    BBNode tmp = ((GeomObj*)o)->construct_bb();
    min = tmp.min;
    max = tmp.max;
  }
  child_l = o;
  child_r = NULL;
}

/*
 * Default BBNode Constructor, built to encompass two objects
 *----------------------------------------------------------------------------*/
BBNode::BBNode( SceneObject *o1, SceneObject *o2 )
{
  BBNode *node;
  if(o1->getType() == NODE)
  {
    node = (BBNode*)o1;
    min = node->min;
    max = node->max;
  }else{
    BBNode tmp = ((GeomObj*)o1)->construct_bb();
    min = tmp.min;
    max = tmp.max;
  }
  child_l = o1;

  if(o2->getType() == NODE)
  {
    node = (BBNode*)o2;
    BBNode tmp = combine(*node);
    min = tmp.min;
    max = tmp.max;
  }else{
    BBNode tmp = ((GeomObj*)o2)->construct_bb();
    tmp = combine(tmp);
    min = tmp.min;
    max = tmp.max;
  }
  child_r = o2;
}

/*
 * Default BBNode Constructor with minimum and maximum points defined
 *----------------------------------------------------------------------------*/
BBNode::BBNode( Vector minimum, Vector maximum )
{
  min = minimum;
  max = maximum;
  child_l = NULL;
  child_r = NULL;
}

/*
 * Return the cost for the given node
 *----------------------------------------------------------------------------*/
double BBNode::cost( void )
{
  double w = max.x() - min.x();
  double d = max.z() - min.z();
  double h = max.y() - min.y();
  double area = w * d * h;
  int children = 0;
  double children_cost = 0;
  if(child_l != NULL)
  {
    children++;
    if(child_l->getType() == NODE)
    {
      children_cost = ((BBNode*)child_l)->cost();
    }
  }
  if(child_r != NULL)
  {
    children++;
    if(child_r->getType() == NODE)
    {
      children_cost = ((BBNode*)child_r)->cost();
    }
  }

  return area * children + children_cost;
}

/*
 * Test BBNODE intersect. return hit condition, overwrite distance and normal
 * variables given
 *----------------------------------------------------------------------------*/
int BBNode::test_intersect( Ray ray, double *t, Vector *n )
{
  double near = -1000; double far = 1000;
  double near_tmp, far_tmp;

  if(min.x() == max.x() &&
     min.y() == max.y() &&
     min.z() == max.z())
     return true;

  double p[3] = {ray.start.x(), ray.start.y(), ray.start.z()};
  double d[3] = {ray.direction.x(), ray.direction.y(), ray.direction.z()};
  double tmin[3] = {min.x(), min.y(), min.z()};
  double tmax[3] = {max.x(), max.y(), max.z()};
  int r, i;

  for(i = 0; i < 3; i++)
  {
    r = test_intersect_1d(p[i], d[i], tmin[i], tmax[i], &near_tmp, &far_tmp);
    if(!r)
      return false;

    if(near_tmp > near)
      near = near_tmp;
    if(far_tmp < far)
      far = far_tmp;
  }

  if(near > far) return false;
  if(far < 0) return false;

  return true;
}

int BBNode::inside( Vector point )
{
  double p[3] = {point.x(), point.y(), point.z()};
  double tmin[3] = {min.x(), min.y(), min.z()};
  double tmax[3] = {max.x(), max.y(), max.z()};

  int inside = 1;

  for(int i = 0; i < 3; i++)
  {
    if(tmin[i] >= p[i]) return false;
    if(tmax[i] <= p[i]) return false;
  }
  return true;
}

/*
 * Test BBNODE intersect. return hit condition, overwrite distance and normal
 * variables given
 *----------------------------------------------------------------------------*/
int BBNode::test_intersect( Ray ray, double *t1, double *t2, Vector *n )
{
  double near = -1000; double far = 1000;
  double near_tmp, far_tmp;

  if(min.x() == max.x() &&
     min.y() == max.y() &&
     min.z() == max.z())
     return true;

  double p[3] = {ray.start.x(), ray.start.y(), ray.start.z()};
  double d[3] = {ray.direction.x(), ray.direction.y(), ray.direction.z()};
  double tmin[3] = {min.x(), min.y(), min.z()};
  double tmax[3] = {max.x(), max.y(), max.z()};
  int r, i;

  for(i = 0; i < 3; i++)
  {
    r = test_intersect_1d(p[i], d[i], tmin[i], tmax[i], &near_tmp, &far_tmp);
    if(!r)
      return false;

    if(near_tmp > near)
      near = near_tmp;
    if(far_tmp < far)
      far = far_tmp;
  }

  if(near > far) return false;
  if(far < 0) return false;

  Vector point;
  if(near < 0.0)
  {
    *t1 = 0.0;
    *t2 = far;
  }
  else
  {
    *t1 = near;
    *t2 = far;
  }
  return true;
}

/*
 * Construct a bounding box with the given matrix applied
 *----------------------------------------------------------------------------*/
BBNode BBNode::construct( MyMat matrix )
{
  matrix = matrix.inverse();
  Vector corner[8] = {
                      Vector(min.x(), min.y(), min.z()),
                      Vector(min.x(), min.y(), max.z()),
                      Vector(min.x(), max.y(), min.z()),
                      Vector(min.x(), max.y(), max.z()),

                      Vector(max.x(), min.y(), min.z()),
                      Vector(max.x(), min.y(), max.z()),
                      Vector(max.x(), max.y(), min.z()),
                      Vector(max.x(), max.y(), max.z())
                     };
  int i;

  Vector tmp;
  Vector pmin = Vector(1000, 1000, 1000);
  Vector pmax = pmin * -1;

  for(i = 0; i < 8; i++)
  {
    tmp = matrix * Vector4(corner[i], 1);
    if(tmp.x() < pmin.x()) pmin.x(tmp.x());
    if(tmp.y() < pmin.y()) pmin.y(tmp.y());
    if(tmp.z() < pmin.z()) pmin.z(tmp.z());

    if(tmp.x() > pmax.x()) pmax.x(tmp.x());
    if(tmp.y() > pmax.y()) pmax.y(tmp.y());
    if(tmp.z() > pmax.z()) pmax.z(tmp.z());
  }

  return BBNode(pmin, pmax);
}


/*
 * BBNode to string function
 *----------------------------------------------------------------------------*/
char* BBNode::str( void )
{
  char *buffer = (char*)calloc(300, sizeof(char));
  int count = 0;
  sprintf(buffer, "[BBNode] %s %s",
          min.str(),
          max.str());

  return buffer;
}

/*
 * Print the current indent level
 *----------------------------------------------------------------------------*/
void printInd( int indent )
{
  int i = 0;
  for(i = 0; i < indent; i++)
    printf(" ");
}

/*
 * Recursively print the given node
 *----------------------------------------------------------------------------*/
void recursePrintNode(SceneObject *node, int indent)
{
  if(!node)
    return;
  TYPE type = node->getType();
  if(type == NODE)
  {
    BBNode *n = (BBNode*)node;
    printInd( indent );
    printf("%s\n", n->str());

    printInd( indent );
    printf("LEFT\n");
    recursePrintNode(n->child_l, indent + 2);

    printInd( indent );
    printf("RIGHT\n");
    recursePrintNode(n->child_r, indent + 2);
  }else{
    printInd( indent );
    printf("%s\n", node->str());
  }
}

/*
 * Return the recursive string for the given node
 *----------------------------------------------------------------------------*/
char* BBNode::recurse_str()
{
  int indent = 2;

  printInd( indent );
  printf("%s\n", str());

  printInd( indent );
  printf("LEFT\n");
  recursePrintNode(child_l, indent + 2);

  printInd( indent );
  printf("RIGHT\n");
  recursePrintNode(child_r, indent + 2);
}

/*
 * Construct the bounding box for the given object
 *----------------------------------------------------------------------------*/
BBNode GeomObj::construct_bb( void )
{
  return BBNode();
}

/*
 * Return the center of the bounding box
 *----------------------------------------------------------------------------*/
Vector BBNode::getCenter()
{
  return (min + max) / 2.0f;
}

/*
 * Combine with the given bounding box and return a new BBNode
 *----------------------------------------------------------------------------*/
BBNode BBNode::combine(BBNode node)
{
  BBNode ret;
  if(node.min.x() < min.x())
    ret.min.x(node.min.x());
  else
    ret.min.x(min.x());
  if(node.min.y() < min.y())
    ret.min.y(node.min.y());
  else
    ret.min.y(min.y());
  if(node.min.z() < min.z())
    ret.min.z(node.min.z());
  else
    ret.min.z(min.z());

  if(node.max.x() > max.x())
    ret.max.x(node.max.x());
  else
    ret.max.x(max.x());
  if(node.max.y() > max.y())
    ret.max.y(node.max.y());
  else
    ret.max.y(max.y());
  if(node.max.z() > max.z())
    ret.max.z(node.max.z());
  else
    ret.max.z(max.z());

  return ret;
}
