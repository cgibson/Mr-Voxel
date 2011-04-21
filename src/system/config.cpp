#include "config.h"
#include "../integrator/integrator.h"


namespace sys {
namespace config {

    void parse(char* argv[], int argc) {
        int current = 1;

        while(current < argc) {
            if( strcmp(argv[current], "-I") == 0 ) {
            current++;
            in_file = argv[current];
            current++;
            }
            else if( strcmp(argv[current], "-W") == 0 ) {
            current++;
            sscanf(argv[current], "%d", &image_resolution.width);
            current++;
            }
            else if( strcmp(argv[current], "-H") == 0 ) {
            current++;
            sscanf(argv[current], "%d", &image_resolution.height);
            current++;
            }
            else if( strcmp(argv[current], "-D") == 0 ) {
            current++;
            sscanf(argv[current], "%d", &render_depth);
            current++;
            }
            else if( strcmp(argv[current], "-s") == 0 ) {
            multi_threading = false;
            current++;
            }
            else if( in_file.empty() ) {

                in_file = argv[current];
                current++;

                if(current < argc || in_file.find(".tga") < 1) {
                    printf("Error: arguments after input file\n");
                    exit(1);
                }
            }
            else{
                printf("Usage: lab2 -I [filename] -W width -H height\n");
                exit(1);
            }
        }

        if(in_file.empty()) {
            printf("Error: no input file given\n");
        }

        // Parse output file from input file
        out_file = in_file.substr(0, in_file.length() - 3);
        out_file.append(".tga");

        printf("INFILE: %s\nOUT: %s\n", in_file.c_str(), out_file.c_str());


    }

    // Render options
    ambient_render_t ambience = AMBIENT_FULL;
    bool specular = true;
    shading_render_t shading = SHADING_SMOOTH;
    int render_depth = 3;
    bool reflection = true;
    bool refraction = true;
    Color background = 0.;

    // Render targets
    render_target_t render_target = TARGET_FULL;

    // Image options
    Dimension image_resolution = {800, 600};
    antialiasing_t antialiasing = ANTIALIASING_NONE;
    string in_file = "";
    string out_file = "";
    file_out_t save_option = FILE_INPUT_NAME;

    // Sample options
    sample_t hemisphere_sampler = SAMPLE_STRATEFIED;
    int hemisphere_u = 5;
    int hemisphere_t = 10;
    Dimension light_sample_resolution = {800, 600};
    float surfel_size = 0.02;
    float surfel_grow = 8.0;
    Vec3 vol_sample_size = Vec3(0.1,0.1,0.1);

    // Scene information
    Scene *scenePtr = NULL;
    VolumeIntegrator *volume_integrator = NULL;

    // Multi-threading
    bool multi_threading = true;
    int core_count = 4;
    int core_thread_count = 5;
    int thread_jobs = core_count * core_thread_count * 2;

}
}
