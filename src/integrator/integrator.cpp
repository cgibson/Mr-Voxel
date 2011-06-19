#include "integrator.h"
#include "../raycast.h"
#include "../sample/Sampler.h"

Spectrum
VolumeIntegrator::Li(Ray ray, Spectrum *T, bool ambientTest = true) {

    int numSamples;

    double step;

    int numVolumes = mScene->getVolumeCount();
    VolumeRegion **volumes = mScene->getVolumes();

    if(numVolumes < 1) {
        return 0.;
    }

    double t1, t0 = 0;
    Vec3 n;
    Surface surface;
    if(!volumes[0]->bounds().test_intersect(ray, &t0, &t1, &n)) return 0;


    if(ray.maxt < t1)
        t1 = ray.maxt;


    numSamples = (int)((t1 - t0) / mStepSize);

    if(numSamples == 0) numSamples = 1;


    //printf("NumSamples: %d\n", numSamples);

    step = (t1 - t0) / (double)numSamples;

    t0 += ((rand() / (float)RAND_MAX)) * step;

    //printf("T02: %f\n", t0);
    //printf("step: %f\n", step);
    //TODO: NOTE: multiple volumes will need aggregators!
    //*T = *T * Color(0.5);

    Spectrum Lv(0);

    LightSource** lights = mScene->getLightSources();
    int light_count = mScene->getLightSourceCount();

    Vec3 p = ray(t0);
    Vec3 pPrev;
    Vec3 w = ray.direction * -1;

    sample::SphericalSamplerRD sampler = sample::SphericalSamplerRD(16,16);

    Spectrum Tr(1.);
    for( int i = 0; i < numSamples; ++i, t0 += step )
    {
        pPrev = p;
        p = ray(t0);
        //printf("T03: %f\n", t0);
        //printf("Step: %f\n", step);
        //printf("Difference: %s %s\n", p.str(), ray(t0 + step).str());
        //printf("Curr T: %f\n", (p - pPrev).length());
        Ray tRay = Ray(pPrev, p - pPrev, 0.0, step);

        Spectrum stepTau = volumes[0]->tau(tRay, 0.5 * step, 0);
        
        Tr = Tr * Exp(stepTau * -1);

        if (Tr.toTrans() < 1e-3) {
            const float continueProb = .5f;
            if ((rand() / (float)RAND_MAX) > continueProb) break;
            Tr = Tr / continueProb;
        }//else if (stepTau.toTrans() > 0.99995)
        //   continue;


        Lv = Lv + volumes[0]->Lve(p);
        Spectrum ss = volumes[0]->sigma_s(p);
        if(!ss.isBlack()) {

            LightSource *light;

            for(int i = 0; i < light_count; i++)
            {
                light = lights[i];

                Vec3 wL = light->position - p;


                float Ldist = wL.norm();

                Ray shadRay = Ray(p, wL);

                if(!mScene->intersect(shadRay, &surface) || (surface.t > Ldist))
                {
                    Spectrum lTr = Exp(volumes[0]->tau(shadRay, mStepSize * 2, 0.0) * -1);
                    Spectrum Ld = lTr * light->sample(p);
                    Lv = Lv + Ld * ss * Tr;// * volumes[0]->phase(p, w, wL * 1);
                }
            }

        }

        float pdf = 2. / (1. * PI);
        
        Color AdTot = 0.;

        if(config::ambience == AMBIENT_FULL && !ss.isBlack() && ambientTest)
        {
            int sAmt;
            if(Tr.toTrans() > 0.8) {
                sAmt = config::volume_samples;
            }else if(Tr.toTrans() > 0.5) {
                sAmt = config::volume_samples >> 1;
            }else if(Tr.toTrans() > 0.3) {
                sAmt = config::volume_samples >> 2;
            }else{
                sAmt = config::volume_samples >> 3;
            }

            if(sAmt > 0 && config::useLvoxels)
            {
                //sample::SphericalSampler sampler = sample::SphericalSampler(sAmt,sAmt);
                Vec3 smpl;
                Color aTr;
                Spectrum Ad;
                Ray indirRay;
                double tt;
                //while(sampler.getSample(&smpl))
                #pragma omp parallel for private(Ad, tt, aTr, indirRay) shared(AdTot) schedule(dynamic, 4)
                for(int i = 0; i < sAmt; i++)
                {
                    sampler.getSample(&smpl);
                    // Indirect lighting
                    indirRay = Ray(p, smpl);
                    indirRay.direction.norm();
                    aTr = 1.;
                    if(config::useCache)
                    {
                        Ad = config::scenePtr->lightCache()->gather(indirRay, &tt, &aTr);
                    }else{
                        Ad = Raycaster::handleIntersect(indirRay, 1, false, true);
                    }
                    #pragma omp critical
                    AdTot = AdTot + Ad * ss;// * volumes[0]->phase(p, w, wL * 1);
                }

                AdTot = AdTot / (pdf * sAmt);
            }
            
        }


        Lv = Lv + (AdTot * Tr);



        //}
    }
    *T = Tr;

    return Lv * step;
}


Spectrum
VolumeIntegrator::Transmittance(Ray ray) {

	int numVolumes = mScene->getVolumeCount();
	VolumeRegion **volumes = mScene->getVolumes();

        if(numVolumes < 1) {
            return 1.;
        }

        double t0,t1;
	Vec3 n;

	//if(!volumes[0]->bounds().test_intersect(ray, &t0, &t1, &n)) return 1.f;

	//TODO: NOTE: multiple volumes will need aggregators!
	Spectrum res = volumes[0]->tau(ray, mStepSize * 2, 0.0);
	//printf("res: %f, %f, %f\n", res.r(), res.g(), res.b());

	return Exp(res * -1);
}
