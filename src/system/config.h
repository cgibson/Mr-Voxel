/* 
 * File:   config.h
 * Author: cgibson
 *
 * Created on March 27, 2011, 12:14 PM
 */

#ifndef CONFIG_H
#define	CONFIG_H

#include "../types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include "../integrator/integrator.h"
#include "../scene/scene.h"

using std::string;

namespace sys {
namespace config {

    extern void parse(char *args[], int argc);
    
    // Render options
    extern ambient_render_t ambience;
    extern bool specular;
    extern shading_render_t shading;
    extern int render_depth;
    extern bool reflection;
    extern bool refraction;
    extern Color background;

    // Render target
    extern render_target_t render_target;

    // Image options
    extern Dimension image_resolution;
    extern antialiasing_t antialiasing;
    extern string in_file;
    extern string out_file;
    extern file_out_t save_option;

    // Sample options
    extern sample_t hemisphere_sampler;
    extern int hemisphere_u;
    extern int hemisphere_t;
    extern Dimension light_sample_resolution;
    extern float surfel_size;
    extern float surfel_grow;

    // Scene information
    extern Scene *scenePtr;
    extern VolumeIntegrator *volume_integrator;

    // Multi-threading
    extern bool multi_threading;
    extern int core_count;
    extern int core_thread_count;
    extern int thread_jobs;

}
}

#endif	/* CONFIG_H */

