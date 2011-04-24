/* 
 * File:   brdf.cpp
 * Author: cgibson
 * 
 * Created on March 27, 2011, 11:51 AM
 */

#include "brdf.h"
#include "../integrator/integrator.h"

inline double max(double a, double b)
{
  return (a > b) ? a : b;
}

namespace light{

    Color shadeIndirect(const Surface &surface, bool gather = true, bool ambient = true) {
        Color result = 0.;

        float pdf = 1. / (4. * PI);
        
        if(gather && config::ambience == AMBIENT_FULL) {
            double tt;
            Color amb;

            // Generate a sampler with a basis on the surface normal
            HemisphereSampler sampler = HemisphereSampler(surface.n, config::hemisphere_u, config::hemisphere_t);

            double rndn;
            Vec3 smpl;

            float sample_mul = pdf * (config::hemisphere_u*config::hemisphere_t);

            Color VLi = 0.;
            Ray sampleRay;

            // Gather samples until the sampler runs out
            while(sampler.getSample(&smpl)) {

                Color Tr = 1.;
                rndn = (smpl * surface.n);
                sampleRay = Ray(surface.p + (surface.n * 0.05), smpl, 0.0);
                amb = config::scenePtr->lightCache()->gather(sampleRay, &tt, &Tr) * rndn;

                //sampleRay = Ray(surface.p + (smpl * 0.0), smpl, 0.0, tt);

                //VLi = config::volume_integrator->Li(sampleRay, &Tr);

                result = result + (amb * surface.finish.ambient * surface.color + VLi * .1) / sample_mul;
            }

        } else if(config::ambience == AMBIENT_FLAT && ambient){
            result = result + surface.color * surface.finish.ambient;
        } else {
            //nothing.
        }

        return result;
    }

    Color shadeDiffuse(const Vec3 &V, const Surface &surf, bool specular = true) {

        Color result = 0.;
        
        // Grab light sources
        LightSource** lights = config::scenePtr->getLightSources();

        Ray shadow_ray;

        // Iterate through each light
        for(int i = 0; i < config::scenePtr->getLightSourceCount(); i++) {

            // Grab temporary light pointer
            LightSource *light = lights[i];

            // Light position, relative to surface
            Vec3 L = light->position - surf.p;
            double l_dist = L.norm();

            // Generate shadow ray
            shadow_ray = Ray(surf.p, L, 0.0, l_dist);

            Surface surface2;

            // Beginning transmittance is 1
            Color Tr(1.);

            // If an object is in the way
            if(config::scenePtr->intersect(shadow_ray, &surface2) && (surface2.t <= l_dist)) {
                Tr = 0.0;

            // Otherwise, integrate through all volumes
            }else{
                Tr = config::volume_integrator->Transmittance( shadow_ray );
            }

            result = result + light::brdf(V, L, surf, light, Tr, specular);
        }

        return result;
        
    }

    Color brdf(const Vec3 &V, const Vec3 &L, const Surface &surf, LightSource *light, const Color &lightTr, bool specular = true) {
        
        Vec3 H = V + L;
        H.norm();

        // invert shininess so it makes sense
        double shininess = 1 / surf.finish.roughness;

        Color result = 0.;
        result = ((surf.color * light->color * lightTr) * surf.finish.diffuse * max(0., surf.n * L));

        if(config::specular && specular && !lightTr.isBlack() && L.dot(surf.n) > 0) {
            result = result + (light->color * surf.finish.specular * lightTr * pow(max(0, H * surf.n), shininess));
        }

        return result;
    }

}

