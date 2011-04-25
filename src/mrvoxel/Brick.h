#ifndef BRICK_H_
#define BRICK_H_

#include <stdlib.h>
#include <stdio.h>
#include "Voxel.h"

#include <stdio.h>

#define BRICK_DIM 8

class Brick {
public:
	Brick();
	virtual ~Brick();

	inline Voxel *operator () (int i, int j, int k) const
	{
		return m_data[i + (BRICK_DIM * j) + (BRICK_DIM * BRICK_DIM * k)];
	}

	inline Voxel *get(int i, int j, int k) const
	{
		return m_data[i + (BRICK_DIM * j) + (BRICK_DIM * BRICK_DIM * k)];
	}

private:
	Voxel *m_data[BRICK_DIM * BRICK_DIM * BRICK_DIM];
};

class BrickGrid {
public:
	BrickGrid(){};
	BrickGrid(int size_x, int size_y, int size_z);

	inline Voxel *operator () (int i, int j, int k) const {

            if(i < 0 || j < 0 || k < 0 || i >= m_size_x || j >= m_size_y || k >= m_size_z)
                return m_zero_voxel;

            const int x = i / BRICK_DIM;
            const int y = j / BRICK_DIM;
            const int z = k / BRICK_DIM;

            const int off_x = i % BRICK_DIM;
            const int off_y = j % BRICK_DIM;
            const int off_z = k % BRICK_DIM;

            const Brick* b = m_data[x + (m_brick_size_y * y) + (m_brick_size_y * m_brick_size_z * z)];

            if( (*(*b)(off_x, off_y, off_z))(DENSITY) < 0.0) {
                printf("ERROR:\n");
                printf("\tx: %d, y: %d, z: %d\n", x, y, z);
                printf("\toff_x: %d, off_y: %d, off_z: %d\n", off_x, off_y, off_z);
            }
            return (*b)(off_x, off_y, off_z);
        }

	virtual ~BrickGrid();

	inline int size_x() const { return m_size_x; }
	inline int size_y() const { return m_size_y; }
	inline int size_z() const { return m_size_z; }

private:

    Voxel *m_zero_voxel;
    Brick **m_data;
    int m_size_x, m_size_y, m_size_z;
    int m_brick_size_x, m_brick_size_y, m_brick_size_z;

};

#endif /* BRICK_H_ */
