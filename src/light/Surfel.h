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

class Surfel {
public:
    Surfel(Vector position, Vector normal, Color diffuse, float area);
    Surfel(const Surfel& orig);
    virtual ~Surfel();

    Vector position(){ return m_pos; }
    Vector normal(){ return m_normal; }
    Color diffuse(){ return m_diffuse; }
    float area(){ return m_area; }
    
private:

    float m_area;         // Surfal area;
    Vector m_pos;       // Surface position
    Vector m_normal;    // Surface normal
    Color m_diffuse;    // Direct lighting data
};

#endif	/* SURFEL_H */

