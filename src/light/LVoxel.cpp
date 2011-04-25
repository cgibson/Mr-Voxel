/* 
 * File:   LiVoxel.cpp
 * Author: cgibson
 * 
 * Created on April 14, 2011, 12:17 AM
 */

#include "LVoxel.h"

LVoxel::~LVoxel() {
}

int
LVoxel::test_intersect( const Ray &ray, double *t )
{
  Vec3 d = Vec3(ray.direction);
  Vec3 p = Vec3(ray.start);

  //d.norm();
  double epsilon = 0.0001f;
  double a = d * d;
  Vec3 tmp = p - position();
  double b = 2 * (d * tmp);
  double c = (tmp * tmp) - (area() * area());
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
    return true;
  }
  else if( discriminent == 0.0f)
  {
    *t = -b / (2.0f * a);
    return true;
  }
  else
  {
    return false;
  }
}

int
LVoxel::inside( const Vec3 &p ) {
    return (p - position()).length() <= sqrt(area() * area());
}

Color
LVoxel::integrate( Color* Tr) {
    *Tr = (*Tr) * Exp(sigma_t() * 0.125 * -1);//_transmittance;//Exp(_transmittance * -1);
    return *Tr * sigma_s() * 0.125 * _transmittance;
}
