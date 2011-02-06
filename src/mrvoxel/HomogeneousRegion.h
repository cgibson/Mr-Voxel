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

	int cyl = (sqrt(pt.x()*pt.x() + pt.y()*pt.y()) > 0.5) && \
	          (sqrt(pt.x()*pt.x() + pt.z()*pt.z()) > 0.5) && \
	          (sqrt(pt.z()*pt.z() + pt.y()*pt.y()) > 0.5);

	double result = cyl * sin((1-dist) * 10.0) * m_density;

    	return (result > 0.0) ? result : 0.0;
    }

private:
    double m_density;

};

#endif
