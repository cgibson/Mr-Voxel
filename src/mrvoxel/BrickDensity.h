#ifndef BRICKDENSITY_H_
#define BRICKDENSITY_H_

#include <iostream>
#include <sstream>
#include <fstream>
#include <limits.h>
#include "VolumeRegion.h"
#include "../ocean/SciDataParser.h"
#include "Brick.h"

class BrickDensityRegion : public DensityRegion {
public:
	BrickDensityRegion(Vec3 min, Vec3 max, Spectrum absorbtion,
			Spectrum scatter, double greenstein, Spectrum emitt, double density);

	double density( Vec3 pt );
    float interpolate( float x, float y, float z, VoxVal val );
    void loadCT(string file, Vec3 file_res, Vec3 vol_res);
    void loadCT(string file, Vec3 file_res, Vec3 vol_res, int iso_min, int iso_max);
    void loadOceanData(string file, Vec3 vol_res, int iso_min, int iso_max);
    void set(int i, int j, int k, float val);
    void add(int i, int j, int k, float val);
    void splat(int i, int j, int k, float val, int splat);



private:
    unsigned short twoByte2ShortX(char *ptr);
    void loadVolSlice(std::string file, Vec3 file_res, Vec3 vol_res, int y_val, int iso_min, int iso_max);

    double m_density_mult;
	BrickGrid m_brickData;
};

#endif /* BRICKDENSITY_H_ */
