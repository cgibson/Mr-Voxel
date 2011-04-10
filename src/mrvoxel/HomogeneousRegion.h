#ifndef _HOMOGENEOUS_REGION_H
#define _HOMOGENEOUS_REGION_H

#include "VolumeRegion.h"

class HomogeneousRegion : public DensityRegion {
public:
    HomogeneousRegion(Vec3 min, Vec3 max, Spectrum absorbtion, Spectrum scatter,
                      double greenstein, Spectrum emitt, double density)
    	: DensityRegion( min,max,absorbtion,scatter,greenstein,emitt ),
    	                 m_density(density) { };


    double density( const Vec3 &pt )
    {

    	if(!mBounds.inside(pt))
		return 0.0;
        return m_density;
    }

private:
    double m_density;

};

#endif
