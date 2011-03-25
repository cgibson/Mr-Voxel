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
    Surfel(Vector position, Vector normal, Color diffuse, float area);
    Surfel(const Surfel& orig);
    virtual ~Surfel();

    Vector position(){ return _pos; }
    Vector normal(){ return _normal; }
    Color diffuse(){ return _diffuse; }
    float area(){ return _radius; }
    
private:

    Vector _pos;       // Surface position
    Vector _normal;    // Surface normal
    Color _diffuse;    // Direct lighting data
};

#endif	/* SURFEL_H */

