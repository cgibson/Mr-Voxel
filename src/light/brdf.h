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

    extern Color brdf(Vec3 &V,
                      Vec3 &L,
                      Surface &surf,
                      LightSource *light,
                      Color &lightTr,
                      bool specular);

    extern Color shadeDiffuse(Vec3 &V, Surface &surf, bool specular);
    extern Color shadeIndirect(Surface &surface, bool gather, bool ambient);
}

#endif	/* BRDF_H */

