#include "BrickDensity.h"

BrickDensityRegion::BrickDensityRegion(
		Vec3 min, Vec3 max, Spectrum absorbtion,Spectrum scatter,
		double greenstein, Spectrum emitt, double density
		):DensityRegion( min,max,absorbtion,scatter,greenstein,emitt ),m_density_mult(density) {

};

void BrickDensityRegion::set(int i, int j, int k, float val) {
    if(i < 0 || i > m_brickData.size_x() ||
       j < 0 || j > m_brickData.size_y() ||
       k < 0 || k > m_brickData.size_z()) {
        printf("Error: setting index out of value [%d, %d, %d]\n", i, j, k);
        exit(1);
    }

    Voxel *tmp = m_brickData(i,j,k);
    assert(false);
    tmp->set(0, val);
}

void BrickDensityRegion::loadCT(string file, Vec3 file_res, Vec3 vol_res) {
    loadCT(file, file_res, vol_res, 0, USHRT_MAX);

}

void BrickDensityRegion::loadCT(string file, Vec3 file_res, Vec3 vol_res, int iso_min, int iso_max) {

	 m_brickData = BrickGrid(vol_res.x(),vol_res.y(),vol_res.z());

     std::string tmp_str1, tmp_str2;

     double offset = file_res.y() / vol_res.y();

     int tmp1, tmp2;
     float tmpt;

     for(int i = 0; i < floor(vol_res.y()); i++) {

        tmp1 = (int)(i * offset);
        tmp2 = tmp1 + 1;
	tmpt = ((float)i * offset) - tmp1;
        if(tmp1 == 0)
            tmp1 = 1;
        if(tmp2 > file_res.y())
            tmp2 = (int)(file_res.y() - 1);
        std::stringstream out1, out2;
        out1 << tmp1;
        out2 << tmp2;
        tmp_str1 = out1.str();
        tmp_str2 = out2.str();

        loadVolSlice(file + tmp_str1, file + tmp_str2, tmpt, file_res, vol_res, i, iso_min, iso_max);
     }

}

void BrickDensityRegion::loadOceanData(string file, Vec3 vol_res, int iso_min, int iso_max) {

     m_brickData = BrickGrid(vol_res.x(),vol_res.y(),vol_res.z());

     SciDataParser parser = SciDataParser();

     parser.parseFile(file);

     vector<SciData> data = parser.getData();
     
     Vec3 minLoc = parser.getMinLoc();
     Vec3 maxLoc = parser.getMaxLoc();
     Vec3 locDiff = maxLoc - minLoc;

     printf("MIN: <%f, %f, %f>\n", minLoc.x(), minLoc.y(), minLoc.z());

     printf("MAX: <%f, %f, %f>\n", maxLoc.x(), maxLoc.y(), maxLoc.z());

     double minO2 = parser.getMinO2();
     double maxO2 = parser.getMaxO2();
     double O2diff = maxO2 - minO2;

     printf("MIN: %f, MAX: %f\n", minO2, maxO2);

     Vec3 tmpLoc;
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

float BrickDensityRegion::interpolate(const float x, const float y, const float z, const VoxVal val) {
    
    // subtracting 0.5 to center
    const float cx = x - 0.5f;
    const float cy = y - 0.5f;
    const float cz = z - 0.5f;

    // integer (base case) for each voxel
    const int ix = floor(x);
    const int iy = floor(y);
    const int iz = floor(z);

    // float value for each voxel
    const float tx = x - ix;
    const float ty = y - iy;
    const float tz = z - iz;

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
    const float f1 = (*v1)(val);
    const float f2 = (*v2)(val);
    const float f3 = (*v3)(val);
    const float f4 = (*v4)(val);
    const float f5 = (*v5)(val);
    const float f6 = (*v6)(val);
    const float f7 = (*v7)(val);
    const float f8 = (*v8)(val);

    // Find 4 average points interpolated along the y-axis
    const float fy1 = (1-ty)*f1 + (ty)*f5;
    const float fy2 = (1-ty)*f2 + (ty)*f6;
    const float fy3 = (1-ty)*f3 + (ty)*f7;
    const float fy4 = (1-ty)*f4 + (ty)*f8;

    // Find 2 average points interpolated along the x-axis
    const float fx1 = (1-tx)*fy1 + (tx)*fy2;
    const float fx2 = (1-tx)*fy3 + (tx)*fy4;

    // Find final interpolation along z-axis
    float final = (1-tz)*fx1 + (tz)*fx2;
    if(final < 0.0) {
        printf("WHAT?!...\n");
        printf("f1: %f\n", f1);
        printf("f2: %f\n", f2);
        printf("f3: %f\n", f3);
        printf("f4: %f\n", f4);
        printf("f5: %f\n", f5);
        printf("f6: %f\n", f6);
        printf("f7: %f\n", f7);
        printf("f8: %f\n", f8);
        printf("cx: %f cy: %f cz: %f\n", cx, cy, cz);
        printf("tx: %f ty: %f tz: %f\n", tx, ty, tz);
        printf("ix: %d iy: %d iz: %d\n", ix, iy, iz);
    }
    return final;
}

void BrickDensityRegion::loadVolSlice(const std::string &file1, const std::string &file2, const float t, const Vec3 &file_res, const Vec3 &vol_res, int y_val, int iso_min, int iso_max) {

  if(file_res.x() < vol_res.x() || file_res.z() < vol_res.z()) {
    printf("Error: Volume must be at most file_resolution large.\n");
    exit(1);
  }
  
  const int size_squared = (int)(file_res.x() * file_res.z());
  const float multiply = (vol_res.x() / file_res.x());
  
  char buffer1[size_squared * 2];
  char buffer2[size_squared * 2];
  ifstream inFile1(file1.c_str(), ios::in | ios::binary);
  ifstream inFile2(file2.c_str(), ios::in | ios::binary);
  
  if((!inFile1))
  {
    printf("An error occurred!  Could not read \"%s\"\n", file1.c_str());
    exit(1);
  }

  if((!inFile2))
  {
    printf("An error occurred!  Could not read \"%s\"\n", file2.c_str());
    exit(1);
  }

  if(!inFile1.read(buffer1, size_squared * 2))
  {
    printf("Could not read %d bytes of data\n", size_squared * 2);
    exit(1);
  }

  if(!inFile2.read(buffer2, size_squared * 2))
  {
    printf("Could not read %d bytes of data\n", size_squared * 2);
    exit(1);
  }

  unsigned short max = 0;
  unsigned short min = 50000;
  
  unsigned short bufferShort[size_squared];

  for(int i = 0; i < size_squared; i++)
  {
    const unsigned short t1 = twoByte2ShortX(buffer1 + (2 * i));
    const unsigned short t2 = twoByte2ShortX(buffer2 + (2 * i));
    bufferShort[i] = (unsigned short)((t1 * (1.-t)) + (t2 * t));
    if(bufferShort[i] > max)
      max = bufferShort[i];
    if(bufferShort[i] < min)
      min = bufferShort[i];
    //printf("tmp is huge! %u\n", bufferShort[i]);
  }

  //printf("MIN: %d, MAX: %d\n", min, max);

  float tmp;

  for(int i = 0; i < (int)file_res.x(); i++)
  {
    for(int j = 0; j < (int)file_res.z(); j++)
    {
      tmp = (float)bufferShort[j * (int)file_res.x() + i];
      tmp = (tmp - iso_min > 0) ? tmp : 0;
      tmp = (tmp > iso_max) ? 0 : tmp;

      assert((float)tmp >= 0.0);
      assert((float)max >= 0.0);
      if(tmp > 0.0)
        add((int)(i * multiply), (int)(vol_res.y() - ((int)(y_val) + 1)), (int)(j * multiply), (float)tmp * m_density_mult * multiply / (float)max);
    }
  }
}
