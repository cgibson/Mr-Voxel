#include "Voxel.h"

Voxel::Voxel( int size, ... ) {

	int i;
	va_list ap;

	m_size = size;
	m_data = new char[size];

	va_start(ap, size);
	for(i = 0; i < size; i++) {
		m_data[i] = (float)va_arg(ap, double);
	}
	va_end(ap);

}

void Voxel::fill( int size, ... ) {

	if(size != m_size) {
		printf("Inputs do not match voxel format\n");
		exit(1);
	}

	int i;
	va_list ap;
	va_start(ap, size);

	for(i = 0; i < size; i++) {
		m_data[i] = (char)va_arg(ap, int);
	}

	va_end(ap);
}

Voxel::~Voxel( void ) {

	if(m_size)
		delete m_data;

}

Voxel::Voxel( void ) {
	m_data = new char[4];
	m_data[0] = 1;
	m_size = 0;
}


Voxel::Voxel( Voxel const& v ) {
	m_data = new char[v.m_size];
	m_data = (char*)memcpy(m_data, v.m_data, v.m_size);

	m_size = v.m_size;
}

