#ifndef _VOLUME_REGION_H
#define _VOLUME_REGION_H

#include "../scene/Object.h"
#include "../scene/BBNode.h"

#define Spectrum Color

float PhaseHG( Vector w, Vector w_prime, float g );

class VolumeRegion : public SceneObject {
public:

    VolumeRegion(Vector min, Vector max);

    virtual Vector World2Volume( Vector pt ) {
    	return pt;// - mBounds.min;
    }

    BBNode bounds(){ return mBounds; }
    virtual int test_intersect( Ray ray, double *t, Vector *n );
    virtual TYPE getType( void ){ return VOLUME; }

    virtual Spectrum sigma_a( Vector pt ) = 0;
    virtual Spectrum sigma_s( Vector pt ) = 0;
    virtual Spectrum sigma_t( Vector pt ) = 0;

    virtual Spectrum Lve( Vector pt ) = 0;

    virtual double phase( Vector pt, Vector w, Vector w_prime ) = 0;

    virtual Spectrum tau( Ray ray, double stepSize, double offset) = 0;
protected:
    BBNode mBounds;

};

class DensityRegion : public VolumeRegion {
public:

	DensityRegion( Vector min, Vector max, Spectrum absorbtion, Spectrum scatter,
			       double greenstein, Spectrum emitt )
		: sig_a(absorbtion), sig_s(scatter), g_scatter(greenstein),
		  le(emitt), VolumeRegion(min, max) { }

	virtual double density( Vector v ) = 0;

	Spectrum sigma_a( Vector pt ) {
		return sig_a * density( World2Volume(pt) );
	}

	Spectrum sigma_s( Vector pt ) {
		return sig_s * density( World2Volume(pt) );
	}

	Spectrum sigma_t( Vector pt ) {
		return (sig_a + sig_s) * density( World2Volume(pt) );
	}

	Spectrum Lve( Vector pt ) {
		return le * density( World2Volume(pt));
	}

	double phase( Vector pt, Vector w, Vector w_prime ) {
		return PhaseHG(w, w_prime, g_scatter);
	}

	Spectrum tau( Ray ray, double stepSize, double offset);

protected:

	Spectrum sig_a, sig_s, le;
	float g_scatter;
};

#endif
