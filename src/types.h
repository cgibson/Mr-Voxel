/* 
 * File:   types.h
 * Author: cgibson
 *
 * Created on March 27, 2011, 2:49 PM
 */

#ifndef TYPES_H
#define	TYPES_H

//#define GLM_SWIZZLE
//#include <glm/glm.hpp>

#include "util/util.h"

class MyMat;
class RGBColor;

/* ENUM TYPES *************************************************************/
// RENDER
enum ambient_render_t { AMBIENT_NONE, AMBIENT_FLAT, AMBIENT_FULL };
enum shading_render_t { SHADING_NONE, SHADING_FLAT, SHADING_SMOOTH };
enum render_target_t { TARGET_FULL, TARGET_DIFFUSE, TARGET_AMBIENT, TARGET_LIGHT_CACHE_RESULT, TARGET_LIGHT_CACHE_TEST_COUNT };

// IMAGE
enum antialiasing_t { ANTIALIASING_NONE, ANTIALIASING_STRATEFIED };
enum file_out_t { FILE_INPUT_NAME, FILE_DATE, FILE_INPUT_NAME_AND_DATE,
                  FILE_NUMBERED, FILE_INPUT_NAME_AND_NUMBERED };

// SAMPLE
enum sample_t { SAMPLE_RAND, SAMPLE_STRATEFIED, SAMPLE_GRID };


/* CLASS DEFINITIONS  *****************************************************/
// IMAGE
typedef struct DIMENSION_T{
    int width, height;
}Dimension;

/* TYPE DEFINITIONS  ******************************************************/
// MATH
typedef Vector3D Vec3;
typedef Vector4D Vec4;
typedef MyMat Matrix;
typedef RGBColor Color;

typedef Color SHCoef;

#endif	/* TYPES_H */

