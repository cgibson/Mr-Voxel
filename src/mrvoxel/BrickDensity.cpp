#include "BrickDensity.h"

BrickDensityRegion::BrickDensityRegion(
		Vector min, Vector max, Spectrum absorbtion,Spectrum scatter,
		double greenstein, Spectrum emitt
		):DensityRegion( min,max,absorbtion,scatter,greenstein,emitt ) {

	int size = 512;

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
						tmp->set(1, 1. - (d * 0.7));

			}
		}
	}
};

double BrickDensityRegion::density(Vector v) {

	if(!mBounds.inside(v))
		return 0.0;

	Vector loc = v - mBounds.min;
	Vector dim = mBounds.max - mBounds.min;

	int off_x = (int)((loc.x() / dim.x()) * m_brickData.size_x());
	int off_y = (int)((loc.y() / dim.y()) * m_brickData.size_y());
	int off_z = (int)((loc.z() / dim.z()) * m_brickData.size_z());

	Voxel *vox = m_brickData(off_x, off_y, off_z);
	return (*vox)(DENSITY);
}
