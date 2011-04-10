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

	inline void set( int offset, float val ) {

            float *f = (float*)(m_data + offset);

            *f = val;
        }

	inline void add( int offset, float val ) {

            float *f = (float*)(m_data + offset);

            *f += val;
        }

	void fill( int size, ... );

	char *m_data;
	int m_size;

        inline float operator () (const VoxVal &loc) {
	if(loc < 0 || loc >= m_size) {
		printf("Error: access out of bounds for voxel [%d]\n", loc);
		exit(1);
	}
        
	return *((float*)m_data);
}

private:
    union fl2Char{
        float f;
        char c[0];
    };

};

#endif /* VOXEL_H_ */
