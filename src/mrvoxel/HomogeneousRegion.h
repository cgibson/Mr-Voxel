#ifndef _HOMOGENEOUS_REGION_H
#define _HOMOGENEOUS_REGION_H

#include "VolumeRegion.h"

class HomogeneousRegion : public DensityRegion {
public:
    HomogeneousRegion(Vector min, Vector max, Spectrum absorbtion, Spectrum scatter,
                      double greenstein, Spectrum emitt, double density)
    	: DensityRegion( min,max,absorbtion,scatter,greenstein,emitt ),
    	                 m_density(density) { };


    double density( Vector pt )
    {
    	if(!mBounds.inside(pt))
		return 0.0;
    	//	printf("PT in question: %d")
	double dist = sqrt(pt.x()*pt.x() + pt.y()*pt.y() + pt.z()*pt.z());

	int cyl = (sqrt(pt.x()*pt.x() + pt.y()*pt.y()) < 0.4) || \
	          (sqrt(pt.x()*pt.x() + pt.z()*pt.z()) < 0.4) || \
	          (sqrt(pt.z()*pt.z() + pt.y()*pt.y()) < 0.4);

    int cross = (pt.x() < 0.1 && pt.x() > -0.1) || \
                (pt.y() < 0.1 && pt.y() > -0.1) || \
                (pt.z() < 0.1 && pt.z() > -0.1);

    int top = pt.y() > 0;

    int result = (dist > 0.85 && dist < 1.0) ? 1 : 0.0;

    result &= !(cross || cyl);

    

    	return result ? m_density * result : 0.0;
    }

private:
    double m_density;

};

#endif
