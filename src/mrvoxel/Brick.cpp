#include "Brick.h"

Brick::Brick() {

	for(int i = 0; i < BRICK_DIM * BRICK_DIM * BRICK_DIM; i++) {
		m_data[i] = new Voxel(4, 0.0);
		//printf("getting vox value: %f\n", m_data[i](DENSITY));
	}

	// TODO Auto-generated constructor stub
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

	m_data = (Brick**)malloc(sizeof(Brick*) * size);

	for(int i = 0; i < size; i++) {
		m_data[i] = new Brick();
	}

    m_zero_voxel = new Voxel(4, 0.0);
}

BrickGrid::~BrickGrid() {

}

Voxel *BrickGrid::operator() (int i, int j, int k) {

    if(i < 0 || j < 0 || k < 0 || i >= m_size_x || j >= m_size_y || k >= m_size_z)
        return m_zero_voxel;

	int x = i / BRICK_DIM;
	int y = j / BRICK_DIM;
	int z = k / BRICK_DIM;

	int off_x = i % BRICK_DIM;
	int off_y = j % BRICK_DIM;
	int off_z = k % BRICK_DIM;

/*
	printf("Accessing voxel: %d %d %d\n", i, j, k);
	printf("\tAccessing brick: %d %d %d\n", x, y, z);
	printf("\tBrick offset: %d\n", x + (m_brick_size_y * y) + (m_brick_size_y * m_brick_size_z * z));
	printf("\tAccessing voxel in brick: %d %d %d\n", off_x, off_y, off_z);
*/
	Brick* b = m_data[x + (m_brick_size_y * y) + (m_brick_size_y * m_brick_size_z * z)];

	return (*b)(off_x, off_y, off_z);
}
