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
    };


    class HemisphereSampler : public Sampler<Vec3> {
    public:
        HemisphereSampler(Vec3 normal, int us, int ts);
        bool getSample(Vec3* sample);

    protected:
        int _max_us;
        int _max_ts;
        int _us, _ts;
        Vec3 _normal;
        MyMat _matrix;
    };

}

#endif	/* SAMPLER_H */

