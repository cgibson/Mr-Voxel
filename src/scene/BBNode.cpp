#include "BBNode.h"
#include "Geometry.h"

#include "scene.h"

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
BBNode::BBNode( Vec3 minimum, Vec3 maximum )
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
int BBNode::test_intersect( Ray ray, double *t, Vec3 *n )
{
    double t2;
    return test_intersect(ray, t, &t2, n);
}

int BBNode::inside( Vec3 point )
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
int BBNode::test_intersect( Ray ray, double *t1, double *t2, Vec3 *n )
{

    double min_t, max_t, near_t, far_t;
    min_t = max_t = near_t = far_t = 0.0;

    // X Axis test
    min_t = (min.x() - ray.start.x()) / ray.direction.x();
    max_t = (max.x() - ray.start.x()) / ray.direction.x();
    if (max_t < min_t)
    {
        double temp = min_t;
        min_t = max_t;
        max_t = temp;
    }

    near_t = min_t;
    far_t = max_t;

    // Y Axis test
    min_t = (min.y() - ray.start.y()) / ray.direction.y();
    max_t = (max.y() - ray.start.y()) / ray.direction.y();
    if (max_t < min_t)
    {
        double temp = min_t;
        min_t = max_t;
        max_t = temp;
    }

    if (min_t > near_t) near_t = min_t;
    if (max_t < far_t) far_t = max_t;

    // Z Axis test
    min_t = (min.z() - ray.start.z()) / ray.direction.z();
    max_t = (max.z() - ray.start.z()) / ray.direction.z();
    if (max_t < min_t)
    {
        double temp = min_t;
        min_t = max_t;
        max_t = temp;
    }

    if (min_t > near_t) near_t = min_t;
    if (max_t < far_t) far_t = max_t;

    // No intersection
    if (near_t > far_t)
    {
        return false;
    }
    // The box is behind us
    else if (far_t < 0.0)
    {
        return false;
    }

    // Return both min_t and max_t
    *t1 = near_t;
    *t2 = far_t;
    
    return true;
}

/*
 * Construct a bounding box with the given matrix applied
 *----------------------------------------------------------------------------*/
BBNode BBNode::construct( Matrix matrix )
{
  matrix = matrix.inverse();
  Vec3 corner[8] = {
                      Vec3(min.x(), min.y(), min.z()),
                      Vec3(min.x(), min.y(), max.z()),
                      Vec3(min.x(), max.y(), min.z()),
                      Vec3(min.x(), max.y(), max.z()),

                      Vec3(max.x(), min.y(), min.z()),
                      Vec3(max.x(), min.y(), max.z()),
                      Vec3(max.x(), max.y(), min.z()),
                      Vec3(max.x(), max.y(), max.z())
                     };
  int i;

  Vec3 tmp;
  Vec3 pmin = Vec3(1000, 1000, 1000);
  Vec3 pmax = pmin * -1;

  for(i = 0; i < 8; i++)
  {
    tmp = matrix * Vec4(corner[i], 1);
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
Vec3 BBNode::getCenter()
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

bool child_intersect(Ray ray, Surface *surface, SceneObject* object) {

    Ray tmp_ray;
    bool hit;
    double epsilon = 0.001f;
    double t = 0;
    Vec3 n;
    
    // create a temporary ray to avoid clipping issues
    GeomObj *obj = (GeomObj*)object;
    tmp_ray.start = obj->matrix * Vec4(ray.start, 1);
    tmp_ray.direction = obj->matrix * Vec4(ray.direction, 0);

    // test the scene object's intersect
    hit = obj->test_intersect(tmp_ray, &t, &n);

    // if the value is closer than the 'closest'
    if(hit && (t > epsilon)) {

        surface->t = t;
        surface->n = obj->matrix.trans() * Vec4(n,1);
        surface->n.norm();
        surface->p = ray.start + (ray.direction * t);
        surface->finish = obj->finish;
        surface->color = obj->pigment.rgbf;
        surface->type = SolidSurface;
        surface->objPtr = obj;
        return true;
    }
    return false;
}

bool
BBNode::intersect(Ray ray, Surface* surface) {

    Surface col_l, col_r;
    double t = 0;
    Vec3 n;
    int hit;
    int hit_l = false;
    int hit_r = false;
    double epsilon = 0.001f;
    TYPE type = getType();

    // if the object is a node
    if(type == NODE) {
        // check to make sure it hits
        hit = test_intersect(ray, &t, &n);

        // return false if no hit, no need to delve deeper
        if(!hit)
            return false;

        // recurse into the left child
        if(has_left()) {
            if(child_l->getType() == NODE) {
                hit_l = static_cast<BBNode*>(child_l)->intersect(ray, &col_l);
            }else{
                hit_l = child_intersect(ray, &col_l, child_l);
            }
        }

        // recurse into the right child
        if(has_right()) {
            if(child_r->getType() == NODE) {
                hit_r = static_cast<BBNode*>(child_r)->intersect(ray, &col_r);
            }else{
                hit_r = child_intersect(ray, &col_r, child_r);
            }
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
    else {
        printf("ERROR: Deprecated code\n");
        exit(1);
        return false;
    }
}