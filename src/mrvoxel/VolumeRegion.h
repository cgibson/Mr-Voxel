#ifndef _VOLUME_REGION_H
#define _VOLUME_REGION_H

#include "../scene/Object.h"
#include "../scene/BBNode.h"

#define Spectrum Color

float PhaseHG( Vec3 w, Vec3 w_prime, float g );

class VolumeRegion : public SceneObject {
public:

    VolumeRegion(Vec3 min, Vec3 max);

    virtual inline Vec3 World2Volume( const Vec3 &pt ) {
    	return pt;// - mBounds.min;
    }

    BBNode bounds(){ return mBounds; }
    virtual int test_intersect( Ray ray, double *t, Vec3 *n );
    virtual inline TYPE getType( void ){ return VOLUME; }

    virtual Spectrum sigma_a( const Vec3 &pt ) = 0;
    virtual Spectrum sigma_s( const Vec3 &pt ) = 0;
    virtual Spectrum sigma_t( const Vec3 &pt ) = 0;

    virtual Spectrum Lve( const Vec3 &pt ) = 0;

    virtual double phase( const Vec3 &pt, const Vec3 &w, const Vec3 &w_prime ) = 0;

    virtual Spectrum tau( const Ray &ray, const double &stepSize, const double &offset) = 0;
protected:
    BBNode mBounds;

};

class DensityRegion : public VolumeRegion {
public:

	DensityRegion( Vec3 min, Vec3 max, Spectrum absorbtion, Spectrum scatter,
			       double greenstein, Spectrum emitt )
		: sig_a(absorbtion), sig_s(scatter), g_scatter(greenstein),
		  le(emitt), VolumeRegion(min, max) { }

	virtual double density( const Vec3 &v ) = 0;

	inline Spectrum sigma_a( const Vec3 &pt ) {
		return sig_a * density( pt );
	}

	inline Spectrum sigma_s( const Vec3 &pt ) {
		return sig_s * density( pt );
	}

	inline Spectrum sigma_t( const Vec3 &pt ) {
            //printf("PT: %s\n", pt.str());
            if(density( pt ) < 0.0) {
                printf(" WHAT %f\n", density( pt ));
            }
		return (sig_a + sig_s) * density( pt );
	}

	inline Spectrum Lve( const Vec3 &pt ) {
		return le * density( pt );
	}

	inline double phase( const Vec3 &pt, const Vec3 &w, const Vec3 &w_prime ) {
		return PhaseHG(w, w_prime, g_scatter);
	}

	Spectrum tau( const Ray &ray, const double &stepSize, const double &offset);

protected:

	Spectrum sig_a, sig_s, le;
	float g_scatter;
};

#endif
