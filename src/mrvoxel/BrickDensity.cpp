#include "BrickDensity.h"

BrickDensityRegion::BrickDensityRegion(
		Vector min, Vector max, Spectrum absorbtion,Spectrum scatter,
		double greenstein, Spectrum emitt, double density
		):DensityRegion( min,max,absorbtion,scatter,greenstein,emitt ),m_density_mult(density) {

/*
	int size = 256;

	m_brickData = BrickGrid(size,size,size);

	Voxel *tmp;

	for(int i = 0; i < size; i++) {
		for(int j = 0; j < size; j++) {
			for(int k = 0; k < size; k++) {
				tmp = m_brickData(i,j,k);
				float d_x = ((size/2.) - (float)i) / (size/2.);
				float d_y = ((size/2.) - (float)j) / (size/2.);
				float d_z = ((size/2.) - (float)k) / (size/2.);
				float d = sqrt(pow(d_x, 2) + pow(d_y, 2) + pow(d_z, 2));
				if(d < 1)
                    tmp->set(1, 0.8);

			}(int)
		}
	}
*/
};

void BrickDensityRegion::set(int i, int j, int k, float val) {
    if(i < 0 || i > m_brickData.size_x() ||
       j < 0 || j > m_brickData.size_y() ||
       k < 0 || k > m_brickData.size_z()) {
        printf("Error: setting index out of value [%d, %d, %d]\n", i, j, k);
        exit(1);
    }

    Voxel *tmp = m_brickData(i,j,k);
    tmp->set(0, val);
}

void BrickDensityRegion::add(int i, int j, int k, float val) {
    if(i < 0 || i > m_brickData.size_x() ||
       j < 0 || j > m_brickData.size_y() ||
       k < 0 || k > m_brickData.size_z()) {
        printf("Error: setting index out of value [%d, %d, %d]\n", i, j, k);
        exit(1);
    }

    Voxel *tmp = m_brickData(i,j,k);
    tmp->add(0, val);
}

void BrickDensityRegion::loadCT(string file, Vector file_res, Vector vol_res) {
    loadCT(file, file_res, vol_res, 0, USHRT_MAX);

}

void BrickDensityRegion::loadCT(string file, Vector file_res, Vector vol_res, int iso_min, int iso_max) {

	 m_brickData = BrickGrid(vol_res.x(),vol_res.y(),vol_res.z());

     std::string tmp_str;

     double offset = file_res.y() / vol_res.y();

     int tmp;

     for(int i = 0; i < floor(vol_res.y()); i++) {

        tmp = (int)(i * offset);

        if(tmp == 0)
            tmp = 1;
        std::stringstream out;
        out << tmp;
        tmp_str = out.str();

        loadVolSlice(file + tmp_str, file_res, vol_res, i, iso_min, iso_max);
     }

}

void BrickDensityRegion::loadOceanData(string file, Vector vol_res, int iso_min, int iso_max) {

     m_brickData = BrickGrid(vol_res.x(),vol_res.y(),vol_res.z());

     SciDataParser parser = SciDataParser();

     parser.parseFile(file);

     vector<SciData> data = parser.getData();
     
     Vector minLoc = parser.getMinLoc();
     Vector maxLoc = parser.getMaxLoc();
     Vector locDiff = maxLoc - minLoc;

     printf("MIN: <%f, %f, %f>\n", minLoc.x(), minLoc.y(), minLoc.z());

     printf("MAX: <%f, %f, %f>\n", maxLoc.x(), maxLoc.y(), maxLoc.z());

     double minO2 = parser.getMinO2();
     double maxO2 = parser.getMaxO2();
     double O2diff = maxO2 - minO2;

     printf("MIN: %f, MAX: %f\n", minO2, maxO2);

     Vector tmpLoc;
     double tmpO2;
     SciData dPoint;

     int end_x, end_y, end_z;
     double end_density;

     for( int i = 0; i < data.size(); i++ ) {
         dPoint = data.at(i);

         // Normalize location
         tmpLoc = dPoint.getLocation();
         tmpLoc = tmpLoc - minLoc;
         tmpLoc.x( tmpLoc.x() / locDiff.x());
         tmpLoc.y( tmpLoc.y() / locDiff.y());
         tmpLoc.z( tmpLoc.z() / locDiff.z());

         tmpO2 = dPoint.getO2Concenration();
         tmpO2 -= minO2;
         tmpO2 /= O2diff;
/*
         printf("WRITING %d\n", tmpO2 * m_density_mult);
         printf("AT LOCATION <%d, %d, %d>\n", (int)(tmpLoc.x() * vol_res.x()),
                 (int)(tmpLoc.y() * vol_res.y()),
                 (int)(tmpLoc.z() * vol_res.z()));
*/
         end_x = (int)(tmpLoc.x() * vol_res.x());
         end_y = (int)(tmpLoc.y() * vol_res.y());
         end_z = (int)(tmpLoc.z() * vol_res.z());
         end_density = tmpO2 * m_density_mult;

         int splatVal;

         if(tmpO2 < 0.4) {
             splatVal = 0;
         }else if(tmpO2 < 0.6) {
             splatVal = 1;
         }else if(tmpO2 < 0.9) {
             splatVal = 2;
         }else if(tmpO2 < 0.95) {
             splatVal = 3;
         }else{
             splatVal = 4;
         }

         if(end_x > 1 && end_x < (vol_res.x() - 1)) {
             if(end_y > 1 && end_y < (vol_res.y() - 1)) {
                if(end_z > 1 && end_z < (vol_res.z() - 1)) {
                    splat(end_x,end_y,end_z, end_density, splatVal);
                }
            }
         }
     }

}


void
BrickDensityRegion::splat(int x, int y, int z, float val, int splat) {

    double contrib;
    double dist;
    double max = sqrt(splat * splat * 3);

    for(int i = ((x-splat) > 0 ? (x-splat) : 0); i <= ((x+splat) < m_brickData.size_x()-1 ? (x+splat) : m_brickData.size_x()-1); i++) {
        for(int j = ((y-splat) > 0 ? (y-splat) : 0); j <= ((y+splat) < m_brickData.size_y()-1 ? (y+splat) : m_brickData.size_y()-1); j++) {
            for(int k = ((z-splat) > 0 ? (z-splat) : 0); k <= ((z+splat) < m_brickData.size_z()-1 ? (z+splat) : m_brickData.size_z()-1); k++) {
                //printf("adding %d %d %d\n", i, j, k);

                dist = sqrt(pow(x - i, 2) + pow(y - j, 2) + pow(z - k, 2));

                contrib = 1 - (dist / max);
                if(splat == 0) {
                    contrib = 1;
                }

                add(i,j,k,val * contrib * contrib);
            }
        }
    }
}

float BrickDensityRegion::interpolate(float x, float y, float z, VoxVal val) {
    
    // subtracting 0.5 to center
    x -= 0.5;
    y -= 0.5;
    z -= 0.5;

    // integer (base case) for each voxel
    int ix = floor(x);
    int iy = floor(y);
    int iz = floor(z);

    // float value for each voxel
    float tx = x - ix;
    float ty = y - iy;
    float tz = z - iz;

    // Each voxel
    Voxel *v1 = m_brickData(ix, iy, iz);
    Voxel *v2 = m_brickData(ix+1, iy, iz);
    Voxel *v3 = m_brickData(ix, iy, iz+1);
    Voxel *v4 = m_brickData(ix+1, iy, iz+1);
    Voxel *v5 = m_brickData(ix, iy+1, iz);
    Voxel *v6 = m_brickData(ix+1, iy+1, iz);
    Voxel *v7 = m_brickData(ix, iy+1, iz+1);
    Voxel *v8 = m_brickData(ix+1, iy+1, iz+1);

    // Float values for each voxel
    float f1 = (*v1)(val);
    float f2 = (*v2)(val);
    float f3 = (*v3)(val);
    float f4 = (*v4)(val);
    float f5 = (*v5)(val);
    float f6 = (*v6)(val);
    float f7 = (*v7)(val);
    float f8 = (*v8)(val);

    // Find 4 average points interpolated along the y-axis
    float fy1 = (1-ty)*f1 + (ty)*f5;
    float fy2 = (1-ty)*f2 + (ty)*f6;
    float fy3 = (1-ty)*f3 + (ty)*f7;
    float fy4 = (1-ty)*f4 + (ty)*f8;

    // Find 2 average points interpolated along the x-axis
    float fx1 = (1-tx)*fy1 + (tx)*fy2;
    float fx2 = (1-tx)*fy3 + (tx)*fy4;

    // Find final interpolation along z-axis
    float final = (1-tz)*fx1 + (tz)*fx2;

    return final;
}

double BrickDensityRegion::density(Vector v) {

	if(!mBounds.inside(v))
		return 0.0;

	Vector loc = v - mBounds.min;
	Vector dim = mBounds.max - mBounds.min;

    // find exact floating-point position inside of grid
	float off_x = (loc.x() / dim.x()) * m_brickData.size_x();
	float off_y = (loc.y() / dim.y()) * m_brickData.size_y();
	float off_z = (loc.z() / dim.z()) * m_brickData.size_z();

    // retrieve surrounding voxels
    // Voxel *vox = m_brickData((int)off_x, (int)off_y, (int)off_z);

    return interpolate(off_x, off_y, off_z, DENSITY);

    // sum and interpolate voxel density values
    // interpolate(i, j, k, DENSITY)
    //	return (*vox)(DENSITY);
}

unsigned short BrickDensityRegion::twoByte2ShortX(char *ptr)
{
  unsigned short r = 0;
  r |= *ptr & 0xFF;
  r <<= 8;
  ptr++;
  r |= *ptr & 0xFF;
  return r;
}

void BrickDensityRegion::loadVolSlice(std::string file, Vector file_res, Vector vol_res, int y_val, int iso_min, int iso_max) {
  int size_squared = (int)(file_res.x() * file_res.z());

  //cout << "READING: " << file << endl;

  if(file_res.x() < vol_res.x() || file_res.z() < vol_res.z()) {
    printf("Error: Volume must be at most file_resolution large.\n");
    exit(1);
  }

  float multiply = (vol_res.x() / file_res.x());

  char buffer[size_squared * 2];

  ifstream inFile(file.c_str(), ios::in | ios::binary);
  if((!inFile))
  {
    printf("An error occurred!  Could not read \"%s\"\n", file.c_str());
    exit(1);
  }

  if(!inFile.read(buffer, size_squared * 2))
  {
    printf("Could not read %d bytes of data\n", size_squared * 2);
    exit(1);
  }

  unsigned short max = 0;
  unsigned short min = 50000;
  unsigned short bufferShort[size_squared];

  for(int i = 0; i < size_squared; i++)
  {
    bufferShort[i] = twoByte2ShortX(buffer + (2 * i));
    if(bufferShort[i] > max)
      max = bufferShort[i];
    if(bufferShort[i] < min)
      min = bufferShort[i];
    //printf("tmp is huge! %u\n", bufferShort[i]);
  }

  //printf("MIN: %d, MAX: %d\n", min, max);

  unsigned short tmp;
  float tmp_data;

  for(int i = 0; i < (int)file_res.x(); i++)
  {
    for(int j = 0; j < (int)file_res.z(); j++)
    {
      tmp = bufferShort[j * (int)file_res.x() + i];
      //printf("tmp is huge! %d\n", tmp);
      tmp = (int(tmp) - iso_min > 0) ? tmp : 0;
      tmp = (tmp > iso_max) ? 0 : tmp;

      //printf("Voxel %d %d %d -> %f\n", (int)(i * multiply), (int)((file_res.y() - (y_val + 1)) * multiply), (int)(j * multiply), (float)tmp / (float)max);
      add((int)(i * multiply), (int)(vol_res.y() - ((int)(y_val) + 1)), (int)(j * multiply), (float)tmp * m_density_mult * multiply / (float)max);
    }
  }
}
