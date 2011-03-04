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

	Voxel *operator () (int i, int j, int k)
	{
		//printf("values: %d %d %d\n", i, j, k);
		//printf("\tlocation: %d\n", i + (BRICK_DIM * j) + (BRICK_DIM * BRICK_DIM * k));
		return m_data[i + (BRICK_DIM * j) + (BRICK_DIM * BRICK_DIM * k)];
	}

	Voxel *get(int i, int j, int k)
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

	Voxel *operator () (int i, int j, int k);

	virtual ~BrickGrid();

	int size_x(){ return m_size_x; }
	int size_y(){ return m_size_y; }
	int size_z(){ return m_size_z; }

private:

    Voxel *m_zero_voxel;
    Brick **m_data;
    int m_size_x, m_size_y, m_size_z;
    int m_brick_size_x, m_brick_size_y, m_brick_size_z;

};

#endif /* BRICK_H_ */
