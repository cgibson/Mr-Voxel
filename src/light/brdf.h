/* 
 * File:   brdf.h
 * Author: cgibson
 *
 * Created on March 27, 2011, 11:51 AM
 */

#ifndef BRDF_H
#define	BRDF_H

#include "../scene/scene.h"
#include "../util/util.h"
#include "../system/config.h"
#include "../sample/Sampler.h"
#include <algorithm>

using sample::HemisphereSampler;
using namespace sys;

namespace light{

    extern Color brdf(const Vec3 &V,
                      const Vec3 &L,
                      const Surface &surf,
                      LightSource *light,
                      const Color &lightTr,
                      bool specular);

    extern Color shadeDiffuse(const Vec3 &V, const Surface &surf, bool specular);
    extern Color shadeIndirect(const Surface &surface, bool gather, bool ambient);
}

#endif	/* BRDF_H */

