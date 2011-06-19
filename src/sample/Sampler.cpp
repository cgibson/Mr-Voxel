/* 
 * File:   Sampler.cpp
 * Author: cgibson
 * 
 * Created on March 24, 2011, 4:59 PM
 */

#include <stdlib.h>

#include "Sampler.h"

namespace sample{

    vector<float> stratefy1D(int samples) {
        //TODO: Implement
    }

    // From http://www.rorydriscoll.com/2009/01/07/better-sampling/
    inline Vec3 sampleToHCoord(float us, float ts) {

        const float r = sqrt(us);
        const float theta = 2 * PI * ts;

        const float x = r * cos(theta);
        const float y = r * sin(theta);

        return Vec3(x, y, sqrt(1.0f - us));

    }

    // Basically from PBRv2
    inline Vec3 sampleToSCoord(float us, float ts) {

        const float z = 1.f - 2.f * us;
        const float r = sqrt(max(0.f, 1.f - z * z));
        const float phi = 2.f * PI * ts;
        const float x = r * cos(phi);
        const float y = r * sin(phi);

        return Vec3(x, y, z);

    }

    inline void bind_val(float *val, float lbound, float ubound) {
        if(ubound < lbound) {
            float tmp = ubound;
            ubound = lbound;
            lbound = tmp;
        }
        
        float diff = ubound - lbound;

        // Assuming min and max are 0 and 1 respectively for value
        (*val) *= diff;
        (*val) += lbound;
    }



    // From http://www.rorydriscoll.com/2009/01/07/better-sampling/
    inline Vec3 sampleToHCoordX(float us, float ts) {

        const float r = sin(us * PI / 2.);
        const float theta = 2 * PI * ts;

        const float x = r * cos(theta);
        const float y = r * sin(theta);

        return Vec3(x, y, sin((PI / 2.) - (us * PI / 2.)));

    }

    HemisphereSampler::HemisphereSampler(Vec3 normal, int us, int ts)
        : _max_us(us), _max_ts(ts), _us(0), _ts(0), _normal(normal) {

        Vec3 up = Vec3(0,1,0);
        Vec3 w = normal;
        Vec3 u;
        Vec3 v;
        w.norm();
        //w = w * -1;

        if(w.y() >= 0.9995 || w.y() <= -0.995) {
        u = Vec3(1,0,0);
        v = Vec3(0,0,1);
        }else{
        up.cross(w, &u);
        u.norm();
        w.cross(u, &v);
        }

        //cout << "Camera Loc: " << endl << m1 << endl;

        MyMat m = MyMat(u.x(), v.x(), w.x(), 0,
                   u.y(), v.y(), w.y(), 0,
                   u.z(), v.z(), w.z(), 0,
                   0, 0, 0, 1);

        _matrix = m;
        //cout << "UVW: " << endl << m2 << endl;

        //_matrix = m.inverse();
    }

    void
    HemisphereSampler::reset() {
        _ts = 0;
        _us = 0;
    }

    bool
    HemisphereSampler::getSample(Vec3* sample) {
        // End of the line
        if(_ts == _max_ts){ /*printf("DONE\n");*/return false;}

        // Jitter calculation
        const float jitter_us = (drand48() - 0.f) * (1. / _max_us);
        const float jitter_ts = (drand48() - 0.f) * (1. / _max_ts);

        float us_val = (_us / (float)_max_us) + jitter_us;
        float ts_val = (_ts / (float)_max_ts) + jitter_ts;

        const Vec3 tmpSample = sampleToHCoord(us_val, ts_val);

        *sample = _matrix * Vec4(tmpSample,0);
        sample->norm();

        //printf("\tsampling %d %d\n", _us, _ts);
        // Incrememt for next sample
        if(++_us == _max_us) {
            _us = 0;
            ++_ts;
        }

        return true;
    }

    SphericalSampler::SphericalSampler(int us, int ts, float lbound_us, float ubound_us, float lbound_ts, float ubound_ts)
        : _lbound_us(lbound_us), _ubound_us(ubound_us), _lbound_ts(lbound_ts), _ubound_ts(ubound_ts), _max_us(us), _max_ts(ts), _us(0), _ts(0) {

    }

    void
    SphericalSampler::reset() {
        _ts = 0;
        _us = 0;
    }

    bool
    SphericalSampler::getSample(Vec3* sample) {
        // End of the line
        if(_ts == _max_ts){ /*printf("DONE\n");*/return false;}

        // Jitter calculation
        const float jitter_us = (drand48() - 0.f) * (1. / _max_us);
        const float jitter_ts = (drand48() - 0.f) * (1. / _max_ts);

        float us_val = (_us / (float)_max_us) + jitter_us;
        float ts_val = (_ts / (float)_max_ts) + jitter_ts;

        bind_val(&us_val, _lbound_us, _ubound_us);
        bind_val(&ts_val, _lbound_ts, _ubound_ts);

        const Vec3 tmpSample = sampleToSCoord(us_val, ts_val);

        *sample = tmpSample;
        sample->norm();

        // Incrememt for next sample
        if(++_us == _max_us) {
            _us = 0;
            ++_ts;
        }

        return true;
    }


    SphericalSamplerRD::SphericalSamplerRD(int us, int ts):_us_per_sampler(us / 4), _ts_per_sampler(ts / 4)
    {
        samplers[0] = new SphericalSampler(_us_per_sampler, _ts_per_sampler, 0.0, 0.5, 0.0, 0.5);
        samplers[1] = new SphericalSampler(_us_per_sampler, _ts_per_sampler, 0.5, 1.0, 0.0, 0.5);
        samplers[2] = new SphericalSampler(_us_per_sampler, _ts_per_sampler, 0.5, 1.0, 0.5, 1.0);
        samplers[3] = new SphericalSampler(_us_per_sampler, _ts_per_sampler, 0.0, 0.5, 0.5, 1.0);
    }

    void
    SphericalSamplerRD::reset()
    {
        samplers[0]->reset();
        samplers[1]->reset();
        samplers[2]->reset();
        samplers[3]->reset();
    }

    bool
    SphericalSamplerRD::getSample(Vec3 *sample)
    {
        int sample_index = (int)(drand48() * 4.0);
        if(!samplers[sample_index]->getSample(sample))
        {
            samplers[sample_index]->reset();
            samplers[sample_index]->getSample(sample);
        }
        return true;
    }


    SphericalSamplerRD::~SphericalSamplerRD()
    {
        delete samplers[0];
        delete samplers[1];
        delete samplers[2];
        delete samplers[3];
    }

}

    //SphericalSampler::SphericalSampler(int us, int ts, bool random)
    //    : _us_per_sampler(us), _ts_per_sampler(ts) {
//
    //}
