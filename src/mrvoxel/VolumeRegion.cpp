#include "VolumeRegion.h"
int wtfcount=0;

int VolumeRegion::test_intersect( Ray ray, double *t, Vec3 *n ) {
    printf("WARNING:  Intersect not used in volumes.\n");
    return false;
}

VolumeRegion::VolumeRegion(Vec3 min, Vec3 max):mBounds(min, max) {

}

float PhaseHG(Vec3 w, Vec3 wp, float g) {

	//printf("Phase teim: %s %s %f\n", w.str(), wp.str(), g);
    float costheta = w.dot(wp);
    //printf("Costheta: %f\n", costheta);
    float val = (1.f / (4.f * M_PI)) *
        ((1.f - g*g) / powf(1.f + g*g - 2.f * g * costheta, 1.5f));
    //printf("Val: %f\n", val);
    return val;
}

Spectrum DensityRegion::tau( const Ray &ray, const double &stepSize, const double &offset) {
	// Optical width
	Spectrum tau(0.);

	double t0, t1;
	Vec3 n;
	
	if(ray.direction.length() == 0) return 0.f;

	if(!mBounds.test_intersect(ray, &t0, &t1, &n)) return 0.f;
        
	// Handle rays with a max t
	if(ray.maxt < t1){
		//printf("T1 %f replaced with %f\n", t1, ray.maxt);
		t1 = ray.maxt;
	}
		
	// Include offset
	t0 += offset * stepSize;

	while(t0 < t1) {
		//printf("t0: %f.  Point: %s\n", t0, ray(t0).str());
		//tau = tau + Spectrum(0.1);
		tau = tau + sigma_t(ray(t0));
		t0 += stepSize;
                //printf("tau: %s t: %f\n", (tau * stepSize).str(), t0);
	}
/*
        if(!tau.isBlack())
            printf("teh tau: %s\n", (tau * stepSize).str());
 //*/
	//printf("tau: %s.  Size %f\n", tau.str(), stepSize);
	// Return integral (evaluated at stepsize)
	return tau * stepSize;
}
