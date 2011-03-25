/* 
 * File:   Surfel.cpp
 * Author: cgibson
 * 
 * Created on March 9, 2011, 10:46 PM
 */

#include "Surfel.h"

Surfel::Surfel(Vector position, Vector normal, Color diffuse, float area)
        : Disk(area, 0, 2 * PI,  Ray(position, normal)),
          _pos(position), _normal(normal), _diffuse(diffuse) {
}
/**/
Surfel::Surfel(const Surfel& orig)
    : Disk(orig) {
    _normal = orig._normal;
    _diffuse = orig._diffuse;
    _pos = orig._pos;
    _radius = orig._radius;
    _innerRadius = orig._innerRadius;
    _phiMax = orig._phiMax;
    _height = orig._height;
    finish = orig.finish;
    matrix = orig.matrix;
    pigment = orig.pigment;
}

Surfel::~Surfel() {
}

