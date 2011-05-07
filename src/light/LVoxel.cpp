/* 
 * File:   LiVoxel.cpp
 * Author: cgibson
 * 
 * Created on April 14, 2011, 12:17 AM
 */

#include "LVoxel.h"
#include "../system/config.h"

using namespace sys;

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
    return (p - position()).length() <= area() * 2.;//sqrt(area() * area());
}

Color
LVoxel::integrate( Color* Tr) {
    *Tr = (*Tr) * Exp(sigma_t() * config::lvoxel_size * -1);//_transmittance;//Exp(_transmittance * -1);
    //printf("Sigma_s: %s\n", sigma_s().str());
    return *Tr * sigma_s() * config::lvoxel_size * _transmittance;
}
