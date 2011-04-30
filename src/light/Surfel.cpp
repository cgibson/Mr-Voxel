/* 
 * File:   Surfel.cpp
 * Author: cgibson
 * 
 * Created on March 9, 2011, 10:46 PM
 */

#include "Surfel.h"

Surfel::Surfel(const Vec3 &position, const Vec3 &normal, const Color &diffuse, float area):
          LiSample(position, diffuse, area),
          _innerRadius(0), _phiMax(2 * PI),
          _normal(normal) {
    
    Vec3 up = Vec3(0,1,0);
    Vec3 w = normal;
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

    Matrix m1 = Matrix(1, 0, 0, position.x(),
                   0, 1, 0, position.y(),
                   0, 0, 1, position.z(),
                   0, 0, 0, 1);

    Matrix m2 = Matrix(u.x(), v.x(), w.x(), 0,
                   u.y(), v.y(), w.y(), 0,
                   u.z(), v.z(), w.z(), 0,
                   0, 0, 0, 1);

    matrix = m1.multRight(m2);

    matrix = matrix.inverse();
}
/*
Surfel::Surfel(const Surfel& orig) {
    _normal = orig._normal;
    _diffuse = orig._diffuse;
    _pos = orig._pos;
    _radius = orig._radius;
    _innerRadius = orig._innerRadius;
    _phiMax = orig._phiMax;
    _height = orig._height;
    matrix = orig.matrix;
}
*/

Surfel::~Surfel() {
    printf("BEING DELETED O NOES D:!\n");
}

int
Surfel::test_intersect( const Ray &ray, double *t, Vec3 * const n ) {

    // If we are parallel, don't bother
    if(fabs(ray.direction.z()) < 1e-7)
        return false;

    // Find the t and compare
    float thit = (_height - ray.start.z()) / ray.direction.z();
    if(thit < ray.mint || thit > ray.maxt)
        return false;

    //printf("3");
    // Find the intersection point
    Vec3 phit = ray(thit);
    float dist2 = phit.x() * phit.x() + phit.y() * phit.y();

    // Check for inner/outer radius
    if(dist2 > area() * area())
        return false;

    *t = thit;
    *n = get_normal(phit);

    return true;
}

Vec3
Surfel::get_normal(Vec3 pt) {
    return matrix.trans() * Vec4(0,0,1,0);
}