/* 
 * File:   sh.cpp
 * Author: cgibson
 * 
 * Created on March 30, 2011, 6:31 PM
 */

#include "sh.h"
#include "Surfel.h"

#include <assert.h>
#include <stdlib.h>
#include <vector>

#define INV_FOURPI 0.079578f

namespace light{
namespace sh{

    inline int SHTerms(int lmax) {
        return (lmax + 1) * (lmax + 1);
    }

    inline int SHIndex(int l, int m) {
        return l * l + l + m;
    }

    // Mostly snipped from pbrt-v2 src/core/sh.cpp
    void legendrep(float x, int lmax, float *out) {
        #define P(l,m) out[SHIndex(l,m)]
        P(0,0) = 1.f;
        P(1,0) = x;
        for (int l = 2; l <= lmax; ++l)
        {
            P(l, 0) = ((2*l-1)*x*P(l-1,0) - (l-1)*P(l-2,0)) / l;
            assert(!isnan(P(l, 0)));
            assert(!isinf(P(l, 0)));
        }

        // Compute m=l edge using Legendre recurrence
        float neg = -1.f;
        float dfact = 1.f;
        float xroot = sqrt(max(0.f, 1.f - x*x));
        float xpow = xroot;
        for (int l = 1; l <= lmax; ++l) {
            P(l, l) = neg * dfact * xpow;
            assert(!isnan(P(l, l)));
            assert(!isinf(P(l, l)));
            neg *= -1.f;      // neg = (-1)^l
            dfact *= 2*l + 1; // dfact = (2*l-1)!!
            xpow *= xroot;    // xpow = powf(1.f - x*x, float(l) * 0.5f);
        }

        // Compute m=l-1 edge using Legendre recurrence
        for (int l = 2; l <= lmax; ++l)
        {
            P(l, l-1) = x * (2*l-1) * P(l-1, l-1);
            assert(!isnan(P(l, l-1)));
            assert(!isinf(P(l, l-1)));
        }

        // Compute m=1, ldots, l-2 values using Legendre recurrence
        for (int l = 3; l <= lmax; ++l)
            for (int m = 1; m <= l-2; ++m)
            {
                P(l, m) = ((2 * (l-1) + 1) * x * P(l-1,m) -
                           (l-1+m) * P(l-2,m)) / (l - m);
                assert(!isnan(P(l, m)));
                assert(!isinf(P(l, m)));
            }

    #undef P
    }

    inline float divfact(int a, int b) {
        if (b == 0) return 1.f;
        float fa = a, fb = fabs(b);
        float v = 1.f;
        for (float x = fa-fb+1.f; x <= fa+fb; x += 1.f)
            v *= x;
        return 1.f / v;
    }

    inline float K(int l, int m) {
        return sqrt((2.f * l + 1.f) * INV_FOURPI * divfact(l, m));
    }

    void sinCosIndexed(float s, float c, int n,
                          float *sout, float *cout) {
        float si = 0, ci = 1;
        for (int i = 0; i < n; ++i) {
            // Compute $\sin{}i\phi$ and $\cos{}i\phi$ using recurrence
            *sout++ = si;
            *cout++ = ci;
            float oldsi = si;
            si = si * c + ci * s;
            ci = ci * c - oldsi * s;
        }
    }

    void SHEval( Vec3 w, int lmax, float *out) {
        assert(w.isNormalized());

        legendrep(w.z(), lmax, out);

        // Compute K(l,m) coefficients
        float *Klm = (float*)malloc(SHTerms(lmax) * sizeof(float));
        for (int l = 0; l <= lmax; ++l)
            for (int m = -l; m <= l; ++m)
                Klm[SHIndex(l, m)] = K(l, m);

        // Compute sin(phi) and cos(phi) values
        float *sines = (float*)malloc((lmax+1) * sizeof(float));
        float *cosines = (float*)malloc((lmax+1) * sizeof(float));

        float xyLen = sqrt(max(0., 1. - w.z()*w.z()));
        if (xyLen == 0.f) {
            for (int i = 0; i <= lmax; ++i) sines[i] = 0.f;
            for (int i = 0; i <= lmax; ++i) cosines[i] = 1.f;
        }
        else {
            sinCosIndexed(w.y() / xyLen, w.x() / xyLen, lmax+1, sines, cosines);
        }

        // Apply SH definitions to compute final $(l,m)$ values
        float sqrt2 = sqrt(2.f);
        for (int l = 0; l <= lmax; ++l) {
            for (int m = -l; m < 0; ++m)
            {
                out[SHIndex(l, m)] = sqrt2 * Klm[SHIndex(l, m)] *
                    out[SHIndex(l, -m)] * sines[-m];
                assert(!isnan(out[SHIndex(l,m)]));
                assert(!isinf(out[SHIndex(l,m)]));
            }
            out[SHIndex(l, 0)] *= Klm[SHIndex(l, 0)];
            for (int m = 1; m <= l; ++m)
            {
                out[SHIndex(l, m)] *= sqrt2 * Klm[SHIndex(l, m)] * cosines[m];
                assert(!isnan(out[SHIndex(l,m)]));
                assert(!isinf(out[SHIndex(l,m)]));
            }
        }

        // Free all alloc'd memory
        free(sines);
        free(cosines);
        free(Klm);
    }

    void SHProject(vector<shared_ptr<Surfel> > surfels, int lmax, SHCoef* coefs) {

        float pdf = 1. / (4. * PI);

        float *Ylm = (float*)malloc(SHTerms(lmax) * sizeof(float));

        vector<shared_ptr<Surfel> >::iterator s_it;

        shared_ptr<Surfel> s;

        SHCoef *c_ptr;
        for(int i = 0; i < surfels.size(); i++) {

            c_ptr = coefs;

            s = surfels[i];
            SHEval(s->normal(), lmax, Ylm);
            for( int i = 0; i < SHTerms(lmax); i++) {
                //printf("testing %s\n", s->normal());
                *c_ptr++ = coefs[i] + s->diffuse() * Ylm[i] /
                        (pdf * surfels.size());
            }
        }

        free(Ylm);
    }

    Color reconstruct(Vec3 w, int lmax, SHCoef* coefs) {
        
        // This may someday include emitted light from the point
        Color L = 0.;

        float *K_c = (float*)malloc(SHTerms(lmax) * sizeof(float));
        SHEval(w, lmax, K_c);
        
        // Includes reflection and diffuse coloring?
        Color Kd = 1.;

        Color Lo = 0.;
        for( int i = 0; i < SHTerms(lmax); ++i) {
            Lo = Lo + coefs[i] * K_c[i];
        }
        free(K_c);
        Lo.clamp(0., 1.);
        return L + Kd * Lo;
    }

    

}
}

