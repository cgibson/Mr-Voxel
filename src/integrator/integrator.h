#ifndef INTEGRATOR_H_
#define INTEGRATOR_H_

#include "../scene/scene.h"

class Integrator {
public:
	Integrator(){}
	virtual void Preprocess(Camera *camera) = 0; //TODO: add renderer

};

class VolumeIntegrator : public Integrator {
public:
	VolumeIntegrator(Scene *scene):Integrator(),mScene(scene){mStepSize = mScene->getVolSampleStep();}
	virtual void Preprocess(Camera *camera){};
	virtual Spectrum Li(Ray ray, Spectrum *transmittence);
	virtual Spectrum Transmittance(Ray ray);
	double mStepSize;
	Scene *mScene;
};

#endif /* INTEGRATOR_H_ */