#ifndef BRICKDENSITY_H_
#define BRICKDENSITY_H_

#include <iostream>
#include <sstream>
#include <fstream>
#include "VolumeRegion.h"
#include "Brick.h"

class BrickDensityRegion : public DensityRegion {
public:
	BrickDensityRegion(Vector min, Vector max, Spectrum absorbtion,
			Spectrum scatter, double greenstein, Spectrum emitt, double density);

	double density( Vector pt );
    float interpolate( float x, float y, float z, VoxVal val );
    void load(string file, Vector file_res, Vector vol_res);
    void set(int i, int j, int k, float val);
    void add(int i, int j, int k, float val);



private:
    unsigned short twoByte2ShortX(char *ptr);
    void loadVolSlice(std::string file, Vector file_res, Vector vol_res, int y_val);

    double m_density_mult;
	BrickGrid m_brickData;
};

#endif /* BRICKDENSITY_H_ */
