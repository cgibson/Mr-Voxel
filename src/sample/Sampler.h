/* 
 * File:   Sampler.h
 * Author: cgibson
 *
 * Created on March 24, 2011, 4:59 PM
 */

#ifndef SAMPLER_H
#define	SAMPLER_H

#include "../util/rayutil.h"
#include "../util/MyMat.h"
#include <vector>

using std::vector;

namespace sample {

    extern vector<float> stratefy1D(int samples);

    template <class T>
    class Sampler {
    public:
        Sampler(){};
        virtual ~Sampler(){};

        virtual bool getSample(T* sample) = 0;
        virtual void reset() = 0;
    };


    class HemisphereSampler : public Sampler<Vec3> {
    public:
        HemisphereSampler(Vec3 normal, int us, int ts);
        bool getSample(Vec3* sample);
        void reset();

    protected:
        int _max_us;
        int _max_ts;
        int _us, _ts;
        Vec3 _normal;
        MyMat _matrix;
    };


    class SphericalSampler : public Sampler<Vec3> {
    public:
        SphericalSampler(int us = 4, int ts = 4, float lbound_us = 0., float ubound_us = 1., float lbound_ts = 0., float ubound_ts = 1.);
        bool getSample(Vec3* sample);
        void reset();

    protected:
        int _max_us;
        int _max_ts;
        int _us, _ts;
        float _lbound_us, _ubound_us;
        float _lbound_ts, _ubound_ts;
    };

    class SphericalSamplerRD : public Sampler<Vec3> {
    public:
        SphericalSamplerRD(int us, int ts);
        ~SphericalSamplerRD();
        bool getSample(Vec3* sample);
        void reset();

    protected:
        int _us_per_sampler;
        int _ts_per_sampler;
        SphericalSampler *samplers[4];
    };

}

#endif	/* SAMPLER_H */

