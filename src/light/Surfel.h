/* 
 * File:   Surfel.h
 * Author: cgibson
 *
 * Created on March 9, 2011, 10:46 PM
 */

#ifndef SURFEL_H
#define	SURFEL_H

#include "../util/util.h"
#include "../util/rayutil.h"
#include "../scene/Geometry.h"

class Surfel : public Disk{
public:
    Surfel(Vec3 position, Vec3 normal, Color diffuse, float area);
    Surfel(const Surfel& orig);
    virtual ~Surfel();

    inline Vec3 position(){ return _pos; }
    inline Vec3 normal(){ return _normal; }
    inline Color diffuse(){ return _diffuse; }
    inline float area(){ return _radius; }
    
private:

    Vec3 _pos;       // Surface position
    Vec3 _normal;    // Surface normal
    Color _diffuse;    // Direct lighting data
};

#endif	/* SURFEL_H */

