#ifndef VOXEL_H_
#define VOXEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

enum VoxVal{ DENSITY };

class Voxel {
public:
	Voxel( void );
	Voxel( int size, ... );
	virtual ~Voxel();
	Voxel( Voxel const& v );

	float operator () (VoxVal loc);
	void set( int size, ... );

	float *m_data;
	int m_size;

};

#endif /* VOXEL_H_ */
