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
  Matrix tmp = Matrix();
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
Vec3 GeomObj::get_normal( Vec3 pt )
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
int GeomObj::test_intersect( const Ray &ray, double *t, Vec3 * const n )
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
int Sphere::test_intersect( const Ray &ray, double *t, Vec3 * const n )
{
  Vec3 d = Vec3(ray.direction);
  Vec3 p = Vec3(ray.start);

  //d.norm();
  double epsilon = 0.0001f;
  double a = d * d;
  Vec3 tmp = p - center;
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
Vec3 Sphere::get_normal( Vec3 pt )
{
  Vec3 tmp = pt - center;
  tmp.norm();
  return tmp;
}

/*
 * Construct the bounding box of the sphere
 *----------------------------------------------------------------------------*/
BBNode Sphere::construct_bb( void )
{
  BBNode node = BBNode(Vec3(center.x() - radius, center.y() - radius, center.z() - radius),
                Vec3(center.x() + radius, center.y() + radius, center.z() + radius));

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
int Cone::test_intersect( const Ray &ray, double *t, Vec3 * const n )
{
  return false;
}

Vec3 Cone::get_normal( Vec3 pt )
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
Box::Box( Vec3 minimum, Vec3 maximum )
{
  _min = minimum;
  _max = maximum;
}

/*
 * Test box intersect. return hit condition, overwrite distance and normal
 * variables given
 *----------------------------------------------------------------------------*/
int Box::test_intersect( const Ray &ray, double *t, Vec3 * const n )
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
    *t = far;
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
    *t = near;
  }
  //Vec3 mult = Vec3(1,1,1);


  return true;
}

/*
 * Return the normal for the given box.  Not used
 *----------------------------------------------------------------------------*/
Vec3 Box::get_normal( Vec3 pt )
{
  return Vec3(0,1,0);
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
    //orient.direction.norm();

    /*
    Vec3 a;
    float theta;
    Vec3 up = Vec3(0,0,-1);

    if(fabs(orient.direction.z()) <= -0.9995 && false) {
        a = Vec3(0,1,0);
    }else{
        up.cross(orient.direction, &a);
    }
    theta = (float)acos((double)orient.direction.dot(Vec3(0,0,-1)));

    a.norm();

    float s = sin(theta);
    float c = cos(theta);

    Matrix m;

    m[0][0] = a.x() * a.x() + (1. - a.x() * a.x()) * c;
    m[0][1] = a.x() * a.y() + (1. - c) - a.z() * s;
    m[0][2] = a.x() * a.z() + (1. - c) + a.y() * s;
    m[0][3] = 0;
    
    m[1][0] = a.x() * a.y() + (1. - c) + a.z() * s;
    m[1][1] = a.y() * a.y() + (1. - a.y() * a.y()) * c;
    m[1][2] = a.y() * a.z() + (1. - c) - a.x() * s;
    m[1][3] = 0;
    
    m[2][0] = a.x() * a.z() + (1. - c) - a.y() * s;
    m[2][1] = a.y() * a.z() + (1. - c) + a.x() * s;
    m[2][2] = a.z() * a.z() + (1. - a.z() * a.z()) * c;
    m[2][3] = 0;
    
    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;

    Matrix m1 = Matrix(1, 0, 0, -orient.start.x(),
                   0, 1, 0, -orient.start.y(),
                   0, 0, 1, -orient.start.z(),
                   0, 0, 0, 1);

    matrix = m.multRight(m1);

//*/
 /*

    orient.direction = Vec3(0.5, 0.5, 0);

    orient.direction.norm();
    //printf("LOOKAT:\n\tPOS: %s\n\tDIR: %s\n", orient.start.str(), orient.direction.str());
    Vec3 u,v,w;

    // Find U,V,W vectors
    if(fabs(orient.direction.y()) >= 0.9995) {
        //printf("Test\n");
        w = Vec3(0,1,0);//orient.direction;
        u = Vec3(1,0,0);
        v = Vec3(0,0,1);
    }else{
        w = orient.direction;
        Vec3 up = Vec3(0,1,0);
        up.cross(w, &u);
        u.norm();
        w.cross(u, &v);
    }
   // printf("\tU(x): %s\n\tV(y): %s\n\tW(z): %s\n\n", u.str(), v.str(), w.str());

    // Generate matrices
    Matrix m1 = Matrix(1, 0, 0, -orient.start.x(),
                   0, 1, 0, -orient.start.y(),
                   0, 0, 1, -orient.start.z(),
                   0, 0, 0, 1);

    Matrix m2 = Matrix(u.x(), v.x(), w.x(), 0,
                   u.y(), v.y(), w.y(), 0,
                   u.z(), v.z(), w.z(), 0,
                   0, 0, 0, 1);

    // Save transformation
    matrix = m2.multRight(m1);
 //* */

    Vec3 up = Vec3(0,1,0);
    Vec3 w = orient.direction;
    Vec3 u;
    Vec3 v;
    w.norm();
    w = w * -1;

    if(w.y() >= 0.9995 || w.y() <= -0.995) {
        u = Vec3(1,0,0);
        v = Vec3(0,0,1);
    }else{
        up.cross(w, &u);
        u.norm();
        w.cross(u, &v);
    }

  Matrix m1 = Matrix(1, 0, 0, orient.start.x(),
                   0, 1, 0, orient.start.y(),
                   0, 0, 1, orient.start.z(),
                   0, 0, 0, 1);

  //cout << "Camera Loc: " << endl << m1 << endl;

  Matrix m2 = Matrix(u.x(), v.x(), w.x(), 0,
                   u.y(), v.y(), w.y(), 0,
                   u.z(), v.z(), w.z(), 0,
                   0, 0, 0, 1);

  //cout << "UVW: " << endl << m2 << endl;

  matrix = m1.multRight(m2);

  matrix = matrix.inverse();

}

BBNode Disk::construct_bb() {
    return BBNode(
            Vec3(-_radius, -_radius, _height),
            Vec3(_radius, _radius, _height)
            );
}

int Disk::test_intersect( const Ray &ray, double *t, Vec3 * const n ) {

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
    Vec3 phit = ray(thit);
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

Vec3 Disk::get_normal(Vec3 pt) {
    return matrix.trans() * Vec4(0,0,1,0);
}

char* Disk::str( void )
{
  char *buffer = (char*)calloc(300, sizeof(char));
  int count = 0;
  sprintf(buffer, "[BOX]\n\tRadius: %f\n\tInnerRadius: %f",
          _radius,
          _innerRadius);

  /*
  if(modifiers != NULL)
  {
    Modifier* current = *modifiers;

    for(count = 0; count < modifier_count; count++)
    {
      strcat(buffer, "\n\t");
      strcat(buffer, modifiers[count]->str());
    }
  }
   * */
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
Triangle::Triangle( Vec3 c1, Vec3 c2, Vec3 c3 )
       :corner1(c1),
        corner2(c2),
        corner3(c3)
{
}

/*
 * Test triangle intersect. return hit condition, overwrite distance and normal
 * variables given
 *----------------------------------------------------------------------------*/
int Triangle::test_intersect( const Ray &ray, double *t, Vec3 * const n )
{
  Vec3 dir = ray.direction;
  Vec3 p0 = ray.start;
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
Vec3 Triangle::get_normal( Vec3 pt )
{
  Vec3 a = corner2 - corner1;
  Vec3 b = corner3 - corner1;
  a.norm();
  b.norm();
  Vec3 n = Vec3(0,0,0);
  a.cross(b, &n);
  n.norm();
  return n;
}

/*
 * Construct a bounding box for the triangle
 *----------------------------------------------------------------------------*/
BBNode Triangle::construct_bb( void )
{
  Vec3 pmin = Vec3(1000, 1000, 1000);
  Vec3 pmax = Vec3(-1000, -1000, -1000);
  Vec3 tmp;
  Vec3 c[3] = {corner1, corner2, corner3};
  int i;

  Matrix matInv = matrix.inverse();

  for(int i = 0; i < 3; i++)
  {
    tmp = matInv * Vec4(c[i], 1);
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

  /*
  Modifier* current = *modifiers;

  for(count = 0; count < modifier_count; count++)
  {
    strcat(buffer, "\n\t");
    strcat(buffer, modifiers[count]->str());
  }
   * */
  return buffer;
}


SmoothTriangle::SmoothTriangle(Vec3 c1, Vec3 n1, Vec3 c2, Vec3 n2, Vec3 c3, Vec3 n3):_n1(n1), _n2(n2), _n3(n3),Triangle(c1,c2,c3)
{
  
}

Vec3
SmoothTriangle::get_normal( Vec3 p )
{
    Vec3 abcCross; (corner2 - corner1).cross(corner3 - corner1, &abcCross);
    Vec3 pbcCross; (corner2 - p).cross(corner3 - p, &pbcCross);
    Vec3 pcaCross; (corner3 - p).cross(corner1 - p, &pcaCross);
    Vec3 pabCross; (corner1 - p).cross(corner2 - p, &pabCross);

    Vec3 N = abcCross;
    N.norm();

    float areaABC = N.dot(abcCross);

    float areaPBC = N.dot(pbcCross);
    float a = areaPBC / areaABC;

    float areaPCA = N.dot(pcaCross);
    float b = areaPCA / areaABC;

    float areaPAB = N.dot(pabCross);
    float c = areaPAB / areaABC;

    Vec3 final = _n1 * a + _n2 * b + _n3 * c;
    final.norm();
    return final;
}


char* SmoothTriangle::str( void )
{
  char *buffer = (char*)calloc(500, sizeof(char));
  int count = 0;
  sprintf(buffer, "[TRIANGLE]\n\tCorner1: %s\n\tNormal1: %s\n\tCorner2: %s\n\tNormal2: %s\n\tCorner3: %s\n\tNormal3: %s\n",
          corner1.str(),
          _n1.str(),
          corner2.str(),
          _n2.str(),
          corner3.str(),
          _n3.str());

  /*
  Modifier* current = *modifiers;

  for(count = 0; count < modifier_count; count++)
  {
    strcat(buffer, "\n\t");
    strcat(buffer, modifiers[count]->str());
  }
   * */
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
int Plane::test_intersect( const Ray &ray, double *t, Vec3 * const n )
{
  if(normal.y() != 0)
  {
    double y = distance / normal.y(); // if x and z are set to zero, y = D / B
    Vec3 tmp = Vec3(0, y, 0);

    *t = ((tmp - ray.start) * normal) / (ray.direction * normal);
    *n = get_normal(ray.start + ray.direction * (*t));
    return *t > 0;
  }else if(normal.z() != 0){
    double z = distance / normal.z(); // if x and y are set to zero, y = D / B
    Vec3 tmp = Vec3(0, 0, z);

    *t = ((tmp - ray.start) * normal) / (ray.direction * normal);
    *n = get_normal(ray.start + ray.direction * (*t));
    return *t > 0;
  }else{
    double x = distance / normal.x(); // if x and y are set to zero, y = D / B
    Vec3 tmp = new Vec3(x, 0, 0);

    *t = ((tmp - ray.start) * normal) / (ray.direction * normal);
    *n = get_normal(ray.start + ray.direction * (*t));
    return *t > 0;
  }
}

/*
 * Return the normal of the plane
 *----------------------------------------------------------------------------*/
Vec3 Plane::get_normal( Vec3 pt )
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
