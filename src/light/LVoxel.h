/* 
 * File:   LiVoxel.h
 * Author: cgibson
 *
 * Created on April 14, 2011, 12:17 AM
 */

#ifndef LIVOXEL_H
#define	LIVOXEL_H

#include "../util/util.h"
#include "../util/rayutil.h"
#include "../scene/Geometry.h"
#include "LSample.h"

class LVoxel : public LiSample{
public:
    LVoxel(const Vec3 &position, const Color &diffuse, Color transmittance, float area):LiSample(position, diffuse, area),_transmittance(transmittance){}
    virtual ~LVoxel();

    TYPE getType(){ return LVOXEL; }

    int test_intersect( const Ray &ray, double *t);
    Color integrate( Color *Tr);
    inline Color transmittance(){ return _transmittance; }

private:
    Color _transmittance;
};

#endif	/* LIVOXEL_H */

