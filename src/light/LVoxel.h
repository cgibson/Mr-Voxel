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
    LVoxel(const Vec3 &position, const Color &diffuse, Color transmittance, Color sigma_t, Color sigma_s, float area):LiSample(position, diffuse, area),_transmittance(transmittance), _sigma_t(sigma_t), _sigma_s(sigma_s){}
    virtual ~LVoxel();

    TYPE getType(){ return LVOXEL; }

    int inside( const Vec3 &p );
    int test_intersect( const Ray &ray, double *t);
    Color integrate( Color *Tr);
    inline Color sigma_t(){ return _sigma_t; }
    inline Color sigma_s(){ return _sigma_s; }
    inline Color transmittance(){ return _transmittance; }

private:
    Color _transmittance;
    Color _sigma_t;
    Color _sigma_s;
};

#endif	/* LIVOXEL_H */

