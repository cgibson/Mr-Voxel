/* 
 * File:   types.h
 * Author: cgibson
 *
 * Created on March 27, 2011, 2:49 PM
 */

#ifndef TYPES_H
#define	TYPES_H

#ifdef	__cplusplus
extern "C" {
#endif


    /* ENUM TYPES *************************************************************/
    // RENDER
    enum ambient_render_t { AMBIENT_NONE, AMBIENT_FLAT, AMBIENT_FULL };
    enum shading_render_t { SHADING_NONE, SHADING_FLAT, SHADING_SMOOTH };
    enum render_target_t { TARGET_FULL, TARGET_DIFFUSE, TARGET_AMBIENT, TARGET_LIGHT_CACHE };

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
    //typedef



#ifdef	__cplusplus
}
#endif

#endif	/* TYPES_H */

