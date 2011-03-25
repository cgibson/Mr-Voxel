/* 
 * File:   Sampler.h
 * Author: cgibson
 *
 * Created on March 24, 2011, 4:59 PM
 */

#ifndef SAMPLER_H
#define	SAMPLER_H

#include "../util/rayutil.h"

namespace sample {

    extern vector<float> stratefy1D(int samples);

    template <class T>
    class Sampler {
    public:
        Sampler(){};
        virtual ~Sampler(){};

        virtual bool getSample(T* sample) = 0;
    };


    class HemisphereSampler : public Sampler<Vector> {
    public:
        HemisphereSampler(Vector normal, int us, int ts);
        bool getSample(Vector* sample);

    protected:
        int _max_us;
        int _max_ts;
        int _us, _ts;
        Vector _normal;
    };

}

#endif	/* SAMPLER_H */

