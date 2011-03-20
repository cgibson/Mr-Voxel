#include <cmath>

#include "Geometry.h"

/*
 * Geometric object constructor
 *----------------------------------------------------------------------------*/
GeomObj::GeomObj( void )
{
}

/*
 * Generate the matrix for the given geometric object
 *----------------------------------------------------------------------------*/
void GeomObj::generateMatrix()
{
  MyMat tmp = MyMat();
  int i;
  for(i = modifier_count - 1; i >= 0; i--)
  {
    tmp = tmp.multRight(modifiers[i]->matrix());
  }
  matrix = tmp.inverse();
}

/*
 * Normal for given geometric object
 *----------------------------------------------------------------------------*/
Vector GeomObj::get_normal( Vector pt )
{

  return NULL;
}

/*
 * Geometric object to string object
 *----------------------------------------------------------------------------*/
char* GeomObj::str( void )
{
  char* buffer = (char*)calloc(11, sizeof(char));
  strcpy(buffer, "[GEOM OBJ]");
  return buffer;
}

/*
 * Test the intersect with a default geometric object
 *----------------------------------------------------------------------------*/
int GeomObj::test_intersect( Ray ray, double *t, Vector *n  )
{
  return false;
}
/*
 * Default Sphere Constructor
 *----------------------------------------------------------------------------*/
Sphere::Sphere( void )
{

}

/*
 * Test sphere intersect. return hit condition, overwrite distance and normal
 * variables given
 *----------------------------------------------------------------------------*/
int Sphere::test_intersect( Ray ray, double *t, Vector *n )
{
  Vector d = Vector(ray.direction);
  Vector p = Vector(ray.start);

  //d.norm();
  double epsilon = 0.0001f;
  double a = d * d;
  Vector tmp = p - center;
  double b = 2 * (d * tmp);
  double c = (tmp * tmp) - (radius * radius);
  double discriminent = (b * b) - (4 * a * c);

  if( discriminent > 0.0f )
  {
    double t1 = (-b - sqrt(discriminent)) / (2.0f * a);
    double t2 = (-b + sqrt(discriminent)) / (2.0f * a);

    if(t1 > epsilon)
    {
      *t = t1;
    }else{
      *t = t2;
    }
    *n = get_normal(ray.start + ray.direction * (*t));
    return true;
  }
  else if( discriminent == 0.0f)
  {
    *t = -b / (2.0f * a);
    *n = get_normal(ray.start + ray.direction * (*t));
    return true;
  }
  else
  {
    return false;
  }
}

/*
 * Return the normal of the sphere
 *----------------------------------------------------------------------------*/
Vector Sphere::get_normal( Vector pt )
{
  Vector tmp = pt - center;
  tmp.norm();
  return tmp;
}

/*
 * Construct the bounding box of the sphere
 *----------------------------------------------------------------------------*/
BBNode Sphere::construct_bb( void )
{
  BBNode node = BBNode(Vector(center.x() - radius, center.y() - radius, center.z() - radius),
                Vector(center.x() + radius, center.y() + radius, center.z() + radius));

  return node.construct(matrix);
}

/*
 * Sphere to string function
 *----------------------------------------------------------------------------*/
char* Sphere::str( void )
{
  char *buffer = (char*)calloc(300, sizeof(char));
  int count = 0;
  sprintf(buffer, "[SPHERE] %.1f %s", radius, center.str());

  if(modifiers != NULL)
  {
    Modifier* current = *modifiers;

    for(count = 0; count < modifier_count; count++)
    {
      strcat(buffer, "\n\t");
      strcat(buffer, modifiers[count]->str());
    }
  }
  return buffer;
}

/*
 * Default Cone Constructor
 *----------------------------------------------------------------------------*/
Cone::Cone( void )
{

}

/*
 * Test cone intersect. return hit condition, overwrite distance and normal
 * variables given
 *----------------------------------------------------------------------------*/
int Cone::test_intersect( Ray ray, double *t, Vector *n )
{
  return false;
}

Vector Cone::get_normal( Vector pt )
{
  return NULL;
}

BBNode Cone::construct_bb( void )
{
  return BBNode();
}

char* Cone::str( void )
{
  char *buffer = (char*)calloc(300, sizeof(char));
  int count = 0;
  sprintf(buffer, "[CONE]\n\tBottom: %s %.2f\n\tTop: %s %.2f",
          end1.str(), radius1,
          end2.str(), radius2);

  Modifier* current = *modifiers;

  for(count = 0; count < modifier_count; count++)
  {
    strcat(buffer, "\n\t");
    strcat(buffer, modifiers[count]->str());
  }
  return buffer;
}

/*
 * Default Box Constructor
 *----------------------------------------------------------------------------*/
Box::Box( void )
{

}

/*
 * Default Box Constructor with minimum and maximum points defined
 *----------------------------------------------------------------------------*/
Box::Box( Vector minimum, Vector maximum )
{
  _min = minimum;
  _max = maximum;
}

/*
 * Test box intersect. return hit condition, overwrite distance and normal
 * variables given
 *----------------------------------------------------------------------------*/
int Box::test_intersect( Ray ray, double *t, Vector *n )
{
  double near = -1000; double far = 1000;
  double near_tmp, far_tmp;

  double p[3] = {ray.start.x(), ray.start.y(), ray.start.z()};
  double d[3] = {ray.direction.x(), ray.direction.y(), ray.direction.z()};
  double tmin[3] = {_min.x(), _min.y(), _min.z()};
  double tmax[3] = {_max.x(), _max.y(), _max.z()};
  Vector tnorm[3] = {Vector(1, 0, 0), Vector(0, 1, 0), Vector(0, 0, 1)};
  Vector near_n, far_n;
  Vector center = (_min + _max) / 2.0;
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

  Vector point;
  Vector mult;
  if(near < 0.0)
  {
    point = ray.start + (ray.direction * far);
    mult = Vector((point.x() < center.x()) ? -1 : 1,
                  (point.y() < center.y()) ? -1 : 1,
                  (point.z() < center.z()) ? -1 : 1
                 );
    *n = Vector(far_n.x() * mult.x(),
                far_n.y() * mult.y(),
                far_n.z() * mult.z());
    *t = far;
  }
  else
  {
    point = ray.start + (ray.direction * near);
    mult = Vector((point.x() < center.x()) ? -1 : 1,
                 (point.y() < center.y()) ? -1 : 1,
                 (point.z() < center.z()) ? -1 : 1
                 );
    *n = Vector(near_n.x() * mult.x(),
                near_n.y() * mult.y(),
                near_n.z() * mult.z());
    *t = near;
  }
  //Vector mult = Vector(1,1,1);


  return true;
}

/*
 * Return the normal for the given box.  Not used
 *----------------------------------------------------------------------------*/
Vector Box::get_normal( Vector pt )
{
  return Vector(0,1,0);
}

/*
 * Return the bounding box for the given box
 *----------------------------------------------------------------------------*/
BBNode Box::construct_bb( void )
{
  BBNode node = BBNode(_min, _max);

  return node.construct(matrix);
}

/*
 * Box to string function
 *----------------------------------------------------------------------------*/
char* Box::str( void )
{
  char *buffer = (char*)calloc(300, sizeof(char));
  int count = 0;
  sprintf(buffer, "[BOX]\n\tCorner1: %s\n\tCorner2: %s",
          _min.str(),
          _max.str());

  if(modifiers != NULL)
  {
    Modifier* current = *modifiers;

    for(count = 0; count < modifier_count; count++)
    {
      strcat(buffer, "\n\t");
      strcat(buffer, modifiers[count]->str());
    }
  }
  return buffer;
}


Disk::Disk(float height, float radius, float innerRadius, float tmax)
    :_height(height), _radius(radius), _innerRadius(innerRadius), _phiMax(tmax)
{
    // Calculate _phiMax in radians
    float tmp = (tmax < 360.0 ? tmax : 360.0);
    tmp = (tmp > 0.0 ? tmp : 0.0);
    _phiMax = (tmp / 360.0) * 2 * PI;
}

Disk::Disk(float radius, float innerRadius, float tmax, Ray orient)
    :_height(0), _radius(radius), _innerRadius(innerRadius), _phiMax(tmax)
{
    // Normalize direction
    orient.direction.norm();

    // Find U,V,W vectors
    Vector w = orient.direction * -1;
    Vector u, v;
    Vector up = Vector(0,1,0);
    up.cross(w, &u);
    w.cross(u, &v);

    // Generate matrices
    MyMat m1 = MyMat(1, 0, 0, orient.start.x(),
                   0, 1, 0, orient.start.y(),
                   0, 0, 1, orient.start.z(),
                   0, 0, 0, 1);

    MyMat m2 = MyMat(u.x(), v.x(), w.x(), 0,
                   u.y(), v.y(), w.y(), 0,
                   u.z(), v.z(), w.z(), 0,
                   0, 0, 0, 1);

    // Save transformation
    matrix = m1.multRight(m2);
}

BBNode Disk::construct_bb() {
    return BBNode(
            Vector(-_radius, -_radius, _height),
            Vector(_radius, _radius, _height)
            );
}

int Disk::test_intersect(Ray ray, double* t, Vector* n) {

    //printf("1");
    // If we are parallel, don't bother
    if(fabs(ray.direction.z()) < 1e-7)
        return false;

    //printf("2");
    // Find the t and compare
    float thit = (_height - ray.start.z()) / ray.direction.z();
    if(thit < ray.mint || thit > ray.maxt)
        return false;

    //printf("3");
    // Find the intersection point
    Vector phit = ray(thit);
    float dist2 = phit.x() * phit.x() + phit.y() * phit.y();

    // Check for inner/outer radius
    if(dist2 > _radius * _radius)// || dist2 < _innerRadius * _innerRadius)
        return false;

    /*
    // Compare phi and phiMax
    float phi = atan2(phit.y(), phit.x());
    if(phi < 0) phi += 2.0 * PI;
    if(phi > _phiMax)
        return false;

    printf("5");
     * */
    *t = thit;
    *n = get_normal(phit);

    return true;
}

Vector Disk::get_normal(Vector pt) {
    return Vector(0,0,1);
}

char* Disk::str( void )
{
  char *buffer = (char*)calloc(300, sizeof(char));
  int count = 0;
  sprintf(buffer, "[BOX]\n\tRadius: %f\n\tInnerRadius: %f",
          _radius,
          _innerRadius);

  if(modifiers != NULL)
  {
    Modifier* current = *modifiers;

    for(count = 0; count < modifier_count; count++)
    {
      strcat(buffer, "\n\t");
      strcat(buffer, modifiers[count]->str());
    }
  }
  return buffer;
}


/*
 * Default Triangle Constructor
 *----------------------------------------------------------------------------*/
Triangle::Triangle( void )
{

}

/*
 * Default Triangle Constructor
 *----------------------------------------------------------------------------*/
Triangle::Triangle( Vector c1, Vector c2, Vector c3 )
       :corner1(c1),
        corner2(c2),
        corner3(c3)
{
}

/*
 * Test triangle intersect. return hit condition, overwrite distance and normal
 * variables given
 *----------------------------------------------------------------------------*/
int Triangle::test_intersect( Ray ray, double *t, Vector *n  )
{
  Vector dir = ray.direction;
  Vector p0 = ray.start;
  //dir.norm();

  double a = corner1.x() - corner2.x();
  double b = corner1.y() - corner2.y();
  double c = corner1.z() - corner2.z();

  double d = corner1.x() - corner3.x();
  double e = corner1.y() - corner3.y();
  double f = corner1.z() - corner3.z();

  double g = dir.x();
  double h = dir.y();
  double i = dir.z();

  double j = corner1.x() - p0.x();
  double k = corner1.y() - p0.y();
  double l = corner1.z() - p0.z();

  double M = a * ((e * i) - (f * h)) -
            d * ((b * i) - (c * h)) +
            g * ((b * f) - (c * e));

  double B = j * ((e * i) - (f * h)) -
            d * ((k * i) - (l * h)) +
            g * ((k * f) - (l * e));
  B /= M;
  if(B < 0.0f || B > 1.0)
    return false;

  double A = a * ((k * i) - (l * h)) -
            j * ((b * i) - (c * h)) +
            g * ((b * l) - (c * k));
  A /= M;
  if(A < 0.0f || (B + A) > 1.0)
    return false;

  double T = a * ((e * l) - (f * k)) -
            d * ((b * l) - (c * k)) +
            j * ((b * f) - (c * e));
  T /= M;

  *t = T;
  *n = get_normal(ray.start + ray.direction * T);
  return true;
}

/*
 * Get the normal for the given triangle
 *----------------------------------------------------------------------------*/
Vector Triangle::get_normal( Vector pt )
{
  Vector a = corner2 - corner1;
  Vector b = corner3 - corner1;
  a.norm();
  b.norm();
  Vector n = Vector(0,0,0);
  a.cross(b, &n);
  n.norm();
  return n;
}

/*
 * Construct a bounding box for the triangle
 *----------------------------------------------------------------------------*/
BBNode Triangle::construct_bb( void )
{
  Vector pmin = Vector(1000, 1000, 1000);
  Vector pmax = Vector(-1000, -1000, -1000);
  Vector tmp;
  Vector c[3] = {corner1, corner2, corner3};
  int i;

  MyMat matInv = matrix.inverse();

  for(int i = 0; i < 3; i++)
  {
    tmp = matInv * Vector4(c[i], 1);
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
 * Triangle to string function
 *----------------------------------------------------------------------------*/
char* Triangle::str( void )
{
  char *buffer = (char*)calloc(300, sizeof(char));
  int count = 0;
  sprintf(buffer, "[TRIANGLE]\n\tCorner1: %s\n\tCorner2: %s\n\tCorner3: %s",
          corner1.str(),
          corner2.str(),
          corner3.str());

  Modifier* current = *modifiers;

  for(count = 0; count < modifier_count; count++)
  {
    strcat(buffer, "\n\t");
    strcat(buffer, modifiers[count]->str());
  }
  return buffer;
}

/*
 * Default Plane Constructor
 *----------------------------------------------------------------------------*/
Plane::Plane( void )
{

}

/*
 * Test plane intersect. return hit condition, overwrite distance and normal
 * variables given
 *----------------------------------------------------------------------------*/
int Plane::test_intersect( Ray ray, double *t, Vector *n  )
{
  if(normal.y() != 0)
  {
    double y = distance / normal.y(); // if x and z are set to zero, y = D / B
    Vector tmp = Vector(0, y, 0);

    *t = ((tmp - ray.start) * normal) / (ray.direction * normal);
    *n = get_normal(ray.start + ray.direction * (*t));
    return *t > 0;
  }else if(normal.z() != 0){
    double z = distance / normal.z(); // if x and y are set to zero, y = D / B
    Vector tmp = Vector(0, 0, z);

    *t = ((tmp - ray.start) * normal) / (ray.direction * normal);
    *n = get_normal(ray.start + ray.direction * (*t));
    return *t > 0;
  }else{
    double x = distance / normal.x(); // if x and y are set to zero, y = D / B
    Vector tmp = new Vector(x, 0, 0);

    *t = ((tmp - ray.start) * normal) / (ray.direction * normal);
    *n = get_normal(ray.start + ray.direction * (*t));
    return *t > 0;
  }
}

/*
 * Return the normal of the plane
 *----------------------------------------------------------------------------*/
Vector Plane::get_normal( Vector pt )
{
  return normal;
}

/*
 * Return the bounding box for the given plane.  NEVER USE
 *----------------------------------------------------------------------------*/
BBNode Plane::construct_bb( void )
{
  return BBNode();
}

char* Plane::str( void )
{
  char *buffer = (char*)calloc(300, sizeof(char));
  int count = 0;
  sprintf(buffer, "[PLANE] %s %f", normal.str(), distance);

  if(modifiers != NULL)
  {
    Modifier* current = *modifiers;

    for(count = 0; count < modifier_count; count++)
    {
      strcat(buffer, "\n\t");
      strcat(buffer, modifiers[count]->str());
    }
  }
  return buffer;
}
