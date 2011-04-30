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
        SphericalSampler(int us, int ts, float min_u = -1., float max_u = 1., float min_t = 0., float max_t = 2.f * PI);
        bool getSample(Vec3* sample);
        void reset();

    protected:
        int _max_us;
        int _max_ts;
        int _us, _ts;
        float _min_u, _max_u;
        float _min_t, _max_t;
    };

    class SphericalSamplerRD : public Sampler<Vec3> {
    public:
        SphericalSamplerRD(int us, int ts);
        bool getSample(Vec3* sample);
        void reset();

    protected:
        int _us_per_sampler;
        int _ts_per_sampler;
        SphericalSampler samplers[4];
    };

}

#endif	/* SAMPLER_H */

