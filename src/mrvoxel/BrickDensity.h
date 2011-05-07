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

    inline double density( const Vec3 &pt ) {

        //if(!mBounds.inside(pt))
        //        return 0.0;

        const Vec3 loc = pt - mBounds.min;
        const Vec3 dim = mBounds.max - mBounds.min;

        // find exact floating-point position inside of grid
        const float off_x = (loc.x() / dim.x()) * m_brickData.size_x();
        const float off_y = (loc.y() / dim.y()) * m_brickData.size_y();
        const float off_z = (loc.z() / dim.z()) * m_brickData.size_z();

        return interpolate(off_x, off_y, off_z, DENSITY);

    };

    float interpolate( float x, float y, float z, VoxVal val );
    void loadCT(string file, Vec3 file_res, Vec3 vol_res);
    void loadCT(string file, Vec3 file_res, Vec3 vol_res, int iso_min, int iso_max);
    void loadOceanData(string file, Vec3 vol_res, int iso_min, int iso_max);
    void set(int i, int j, int k, float val);

    void add(int i, int j, int k, float val) {
        if(i < 0 || i > m_brickData.size_x() ||
           j < 0 || j > m_brickData.size_y() ||
           k < 0 || k > m_brickData.size_z()) {
            printf("Error: setting index out of value [%d, %d, %d]\n", i, j, k);
            exit(1);
        }

        Voxel *tmp = m_brickData(i,j,k);
        tmp->add(0, val);
    }
    
    void splat(int i, int j, int k, float val, int splat);



private:

    inline unsigned short twoByte2ShortX(char *ptr) {
        unsigned short r = 0;
        r |= *ptr & 0xFF;
        r <<= 8;
        ptr++;
        r |= *ptr & 0xFF;
        return r;
    }

    void loadVolSlice(const std::string &file1, const std::string &file2, float t, const Vec3 &file_res, const Vec3 &vol_res, int y_val, int iso_min, int iso_max);

    double m_density_mult;
	BrickGrid m_brickData;
};

#endif /* BRICKDENSITY_H_ */
