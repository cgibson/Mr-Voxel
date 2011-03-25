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
    Vector sampleToHCoord(float us, float ts) {

        const float r = sqrt(us);
        const float theta = 2 * PI * ts;

        const float x = r * cos(theta);
        const float y = r * sin(theta);

        return Vector(x, y, sqrt(1.0f - us));

    }

    HemisphereSampler::HemisphereSampler(Vector normal, int us, int ts)
        : _max_us(us), _max_ts(ts), _us(0), _ts(0), _normal(normal) {

    }

    bool
    HemisphereSampler::getSample(Vector* sample) {
        // End of the line
        if(_ts == _max_ts) return false;

        // Jitter calculation
        const float jitter_us = (drand48() - 0.5f) * (1. / _max_us);
        const float jitter_ts = (drand48() - 0.5f) * (1. / _max_ts);

        *sample = sampleToHCoord(_us + jitter_us, _ts + jitter_ts);

        // Incrememt for next sample
        if(++_us == _max_us) {
            _us = 0;
            ++_ts;
        }

        return true;
    }
}