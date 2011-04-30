#include "config.h"
#include "../integrator/integrator.h"


namespace sys {
namespace config {

    string testCommand(char *argvPtr[], int *current, string short_command, string long_command, bool returns_val = true) {
        char *chTmp;
        chTmp = argvPtr[*current];
        string tmpVal = string(chTmp);
        if(!short_command.compare(tmpVal) || !long_command.compare(tmpVal))
        {
            *current = *current + (returns_val ? 2 : 1);
            return string(argvPtr[*current-1]);
        }else{
            return "";
        }
    }

    void parse(char* argv[], int argc) {
        int current = 1;
        string tmpVal;

        while(current < argc) {
            if( !(tmpVal = testCommand(argv, &current, "-O", "--output")).empty() )
            {
                out_file = tmpVal;
            }
            else if( !(tmpVal = testCommand(argv, &current, "-W", "--width")).empty() )
            {
                sscanf(tmpVal.c_str(), "%d", &image_resolution.width);
            }
            else if( !(tmpVal = testCommand(argv, &current, "-H", "--height")).empty() )
            {
                sscanf(tmpVal.c_str(), "%d", &image_resolution.height);
            }
            else if( !(tmpVal = testCommand(argv, &current, "-D", "--depth")).empty() )
            {
                sscanf(tmpVal.c_str(), "%d", &render_depth);
            }
            else if( !(tmpVal = testCommand(argv, &current, "-S", "--single", false)).empty() )
            {
                multi_threading = false;
            }
            else if( !(tmpVal = testCommand(argv, &current, "-A", "--ambient")).empty() )
            {
                if(tmpVal == "FULL")
                {
                    ambience = AMBIENT_FULL;
                }else if(tmpVal == "NONE")
                {
                    ambience = AMBIENT_NONE;
                }else if(tmpVal == "FLAT")
                {
                    ambience = AMBIENT_FLAT;
                }
            }
            else if( !(tmpVal = testCommand(argv, &current, "-T", "--target")).empty() )
            {
                if(tmpVal == "FULL")
                {
                    render_target = TARGET_FULL;
                }else if(tmpVal == "TEST")
                {
                    render_target = TARGET_LIGHT_CACHE_TEST_COUNT;
                }else if(tmpVal == "CACHE")
                {
                    render_target = TARGET_LIGHT_CACHE_RESULT;
                }
            }
            else if( !(tmpVal = testCommand(argv, &current, "-SS", "--surfel-size")).empty() )
            {
                sscanf(tmpVal.c_str(), "%f", &surfel_size);
            }
            else if( !(tmpVal = testCommand(argv, &current, "-LVS", "--lvoxel-size")).empty() )
            {
                sscanf(tmpVal.c_str(), "%f", &lvoxel_size);
            }
            else if( !(tmpVal = testCommand(argv, &current, "-SW", "--sample-resolution")).empty() )
            {
                sscanf(tmpVal.c_str(), "%dx%d", &light_sample_resolution.width, &light_sample_resolution.height);
            }
            else if( !(tmpVal = testCommand(argv, &current, "-X", "--resolution")).empty() )
            {
                sscanf(tmpVal.c_str(), "%dx%d", &image_resolution.width, &image_resolution.height);
            }
            else if( !(tmpVal = testCommand(argv, &current, "-HU", "--hemisphere-u")).empty() )
            {
                sscanf(tmpVal.c_str(), "%d", &hemisphere_u);
            }
            else if( !(tmpVal = testCommand(argv, &current, "-HT", "--hemisphere-t")).empty() )
            {
                sscanf(tmpVal.c_str(), "%d", &hemisphere_t);
            }
            else if( !(tmpVal = testCommand(argv, &current, "-HX", "--hemisphere-res")).empty() )
            {
                sscanf(tmpVal.c_str(), "%dx%d", &hemisphere_u, &hemisphere_t);
            }
            //else if( !(tmpVal = testCommand(argv, &current, "-SPHS", "--sphere_samples")).empty() )
            //{
            //    sscanf(tmpVal.c_str(), "%d", &sphere_samples);
            //}
            else if( !(tmpVal = testCommand(argv, &current, "-C", "--cores")).empty() )
            {
                sscanf(tmpVal.c_str(), "%d", &core_count);
                thread_jobs = core_count * core_thread_count * 2;
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

        if(out_file.empty()) {
            // Parse output file from input file
            out_file = in_file.substr(0, in_file.length() - 3);
            out_file.append(".tga");
        }

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
    Dimension image_resolution = {640, 480};
    antialiasing_t antialiasing = ANTIALIASING_NONE;
    double gamma_correction = 2.2f;
    string in_file = "";
    string out_file = "";
    file_out_t save_option = FILE_INPUT_NAME;

    // Sample options
    sample_t hemisphere_sampler = SAMPLE_STRATEFIED;
    int hemisphere_u = 4;
    int hemisphere_t = 8;
    int sphere_samples = 4;
    Dimension light_sample_resolution = {800, 600};
    float surfel_size = 0.02;
    float surfel_grow = 8.0;
    float vol_tests_per_sample = 3;
    float lvoxel_size = 0.125;

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
