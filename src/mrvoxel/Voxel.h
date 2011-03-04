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
	void set( int size, float val );
	void add( int size, float val );
	void fill( int size, ... );

	char *m_data;
	int m_size;

private:
    union fl2Char{
        float f;
        char c[0];
    };

};

#endif /* VOXEL_H_ */
