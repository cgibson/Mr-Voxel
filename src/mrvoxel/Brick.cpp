#include "Brick.h"

Brick::Brick() {

	for(int i = 0; i < BRICK_DIM * BRICK_DIM * BRICK_DIM; i++) {
		//m_data[i] = new Voxel(4, 0.0);
		m_data[i] = new Voxel(4);
	}
}

Brick::~Brick() {
	// TODO Auto-generated destructor stub
}

BrickGrid::BrickGrid(int size_x, int size_y, int size_z)
	:m_size_x(size_x),
	 m_size_y(size_y),
	 m_size_z(size_z) {

	m_brick_size_x = size_x / BRICK_DIM;
	m_brick_size_y = size_y / BRICK_DIM;
	m_brick_size_z = size_z / BRICK_DIM;

	int size = m_brick_size_x * m_brick_size_y * m_brick_size_z;

	m_data = new Brick*[size];

	for(int i = 0; i < size; i++) {
		m_data[i] = new Brick();
	}

    //m_zero_voxel = new Voxel(4, 0.0);
    m_zero_voxel = new Voxel(4);
}

BrickGrid::~BrickGrid() {

}