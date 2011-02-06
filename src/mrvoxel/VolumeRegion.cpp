#include "VolumeRegion.h"
int wtfcount=0;

int VolumeRegion::test_intersect( Ray ray, double *t, Vector *n ) {
    printf("WARNING:  Intersect not used in volumes.\n");
    return false;
}

VolumeRegion::VolumeRegion(Vector min, Vector max):mBounds(min, max) {

}

float PhaseHG(Vector w, Vector wp, float g) {

	//printf("Phase teim: %s %s %f\n", w.str(), wp.str(), g);
    float costheta = w.dot(wp);
    //printf("Costheta: %f\n", costheta);
    return 1.f / (4.f * M_PI) *
        (1.f - g*g) / powf(1.f + g*g - 2.f * g * costheta, 1.5f);
}

Spectrum DensityRegion::tau( Ray ray, double stepSize, double offset) {
	// Optical width
	Spectrum tau(0.);

	double t0, t1;
	Vector n;
	
	if(ray.direction.length() == 0) return 0.f;

	if(!mBounds.test_intersect(ray, &t0, &t1, &n)) return 0.f;
	
	//if(ray.maxt < 10)
		//printf("MAXT: %f\n", ray.maxt);
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
	}
	//printf("tau: %s.  Size %f\n", tau.str(), stepSize);
	// Return integral (evaluated at stepsize)
	return tau * stepSize;
}
