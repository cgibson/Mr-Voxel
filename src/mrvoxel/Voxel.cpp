#include "Voxel.h"

Voxel::Voxel( int size, ... ) {

	int i;
	va_list ap;

	m_size = size;
	m_data = new float[size];

	va_start(ap, size);
	for(i = 0; i < size; i++) {
		m_data[i] = (float)va_arg(ap, double);
	}
	va_end(ap);

}

void Voxel::set( int size, ... ) {

	if(size != m_size) {
		printf("Inputs do not match voxel format\n");
		exit(1);
	}

	int i;
	va_list ap;
	va_start(ap, size);

	for(i = 0; i < size; i++) {
		m_data[i] = (float)va_arg(ap, double);
	}

	va_end(ap);
}

Voxel::~Voxel( void ) {

	if(m_size)
		delete m_data;

}

Voxel::Voxel( void ) {
	m_data = new float[1];
	m_data[0] = 0.1;
	m_size = 0;
}

float Voxel::operator () (VoxVal loc) {
	if(loc < 0 || loc >= m_size) {
		printf("Error: access out of bounds for voxel [%d]\n", loc);
		exit(1);
	}
	//printf("LOC: %d\n", loc);
	return m_data[loc];
}


Voxel::Voxel( Voxel const& v ) {
	m_data = (float*)malloc(sizeof(float) * v.m_size);
	m_data = (float*)memcpy(m_data, v.m_data, v.m_size * sizeof(float));

	m_size = v.m_size;
	printf("m_data is now %f\n", v.m_data[0]);
}

