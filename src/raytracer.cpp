#include <boost/thread/thread.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <boost/shared_ptr.hpp>
#include <utility>
#include <string.h>
#include <fstream>
#include "util/util.h"
#include "scene/scene.h"
#include "raycast.h"
#include "util/image.h"
#include "json/jsonparser.h"
#include "system/config.h"
#include "integrator/integrator.h"
#include <omp.h>

using namespace std;
using namespace boost;
using namespace sys;

Raycaster *raycaster;

int main(int argc, char* argv[])
{

  Parser * const parser = new JSONParser();

  config::parse(argv, argc);

  config::scenePtr = parser->parse_file(config::in_file);

  if( parser->succeeded() ) {
    printf("Scene was compiled correctly.\n");
  }
  else {
    printf("Was not able to compile scene.  Errors exist.\n");
    return -1;
  }

  config::scenePtr->generateBoxHierarchy();

  config::volume_integrator = new VolumeIntegrator(config::scenePtr);
  
  ImageWriter writer = ImageWriter( config::image_resolution );
  
  raycaster = new Raycaster();

  Dimension img_size = config::image_resolution;

  printf("|0...............................................100|\n|");


    if((config::ambience == AMBIENT_FULL) && config::useCache)
    {


        config::scenePtr->initCache(Vec3(-15, -15, -15), Vec3(15, 15, 15));

        // Hard coding light cast camera for now.
        Camera lightCamera;
        lightCamera.fov = (60. / 360.) * 2 * PI;
        lightCamera.fov_ratio = sin(lightCamera.fov / 2.0);
        lightCamera.location = Vec3(0, 3, -10);
        lightCamera.look_at = Vec3(0, 3, 0);
        //lightCamera.location = Vec3(-4,0,0);
        //lightCamera.look_at = Vec3(5,0,0);
        lightCamera.right = Vec3(1.33, 0, 0);
        lightCamera.up = Vec3(0,1,0);

        Raycaster lightCast(lightCamera);
        lightCast.surfelCast(config::light_sample_resolution, 1, 1, &writer);
    }

  LiNode* root = config::scenePtr->lightCache();

  if(config::ambience == AMBIENT_FULL && config::useCache)
    printf("LIGHT CACHE SIZE: %d\n", root->size_of());

  raycaster->raycast(img_size.width, img_size.height, &writer);

  
  printf("|\n");
  
  //rtrc1();
  writer.write(config::out_file);
}
