#ifndef _VOLUME_REGION_H
#define _VOLUME_REGION_H

#include "../scene/Object.h"
#include "../scene/BBNode.h"

#define Spectrum Color

float PhaseHG( Vec3 w, Vec3 w_prime, float g );

class VolumeRegion : public SceneObject {
public:

    VolumeRegion(Vec3 min, Vec3 max);

    virtual Vec3 World2Volume( Vec3 pt ) {
    	return pt;// - mBounds.min;
    }

    BBNode bounds(){ return mBounds; }
    virtual int test_intersect( Ray ray, double *t, Vec3 *n );
    virtual TYPE getType( void ){ return VOLUME; }

    virtual Spectrum sigma_a( Vec3 pt ) = 0;
    virtual Spectrum sigma_s( Vec3 pt ) = 0;
    virtual Spectrum sigma_t( Vec3 pt ) = 0;

    virtual Spectrum Lve( Vec3 pt ) = 0;

    virtual double phase( Vec3 pt, Vec3 w, Vec3 w_prime ) = 0;

    virtual Spectrum tau( Ray ray, double stepSize, double offset) = 0;
protected:
    BBNode mBounds;

};

class DensityRegion : public VolumeRegion {
public:

	DensityRegion( Vec3 min, Vec3 max, Spectrum absorbtion, Spectrum scatter,
			       double greenstein, Spectrum emitt )
		: sig_a(absorbtion), sig_s(scatter), g_scatter(greenstein),
		  le(emitt), VolumeRegion(min, max) { }

	virtual double density( Vec3 v ) = 0;

	Spectrum sigma_a( Vec3 pt ) {
		return sig_a * density( World2Volume(pt) );
	}

	Spectrum sigma_s( Vec3 pt ) {
		return sig_s * density( World2Volume(pt) );
	}

	Spectrum sigma_t( Vec3 pt ) {
		return (sig_a + sig_s) * density( World2Volume(pt) );
	}

	Spectrum Lve( Vec3 pt ) {
		return le * density( World2Volume(pt));
	}

	double phase( Vec3 pt, Vec3 w, Vec3 w_prime ) {
		return PhaseHG(w, w_prime, g_scatter);
	}

	Spectrum tau( Ray ray, double stepSize, double offset);

protected:

	Spectrum sig_a, sig_s, le;
	float g_scatter;
};

#endif
