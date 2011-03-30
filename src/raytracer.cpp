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

using namespace std;
using namespace boost;
using namespace sys;

Raycaster *raycaster;

struct raytracer
{
    raytracer(int start_x, int start_y, int end_x, int end_y, int width,
              int height, int depth, ImageWriter *writer, int threadId)
        : m_start_x(start_x)
        , m_start_y(start_y)
        , m_end_x(end_x)
        , m_end_y(end_y)
        , m_width(width)
        , m_height(height)
        , m_depth(depth)
        , m_writer(writer)
    	, m_threadId(threadId)
    {
    }
    
    void operator()()
    {
        //printf("Thread [%d] STARTED\n", m_threadId);
        //printf("\tWith dimensions: [%d,%d] to [%d,%d]\n", m_start_x,m_start_y,m_end_x,m_end_y);
       raycaster->raycast( m_start_x, 
               m_start_y,
               m_end_x,
               m_end_y,
               m_width,
               m_height,
               m_depth,
               m_writer
       );
       //printf("Thread [%d] FINISHED\n", m_threadId);
    }

    int m_start_x, m_start_y;
    int m_end_x, m_end_y;
    int m_width, m_height;
    int m_depth;
    int m_threadId;
    ImageWriter *m_writer;
};


int main(int argc, char* argv[])
{

  Parser *parser = new JSONParser();

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
  
  ImageWriter writer = ImageWriter( config::image_resolution );
  
  raycaster = new Raycaster( config::scenePtr );
  
  int cores = config::core_count;
  int corethreadcount = config::core_thread_count;
  int jobs = config::thread_jobs;
  Dimension img_size = config::image_resolution;

  raytracer *rtrcs[jobs];
  thread thrds[jobs];
  
  int start = 0;
  int end = 0;


  printf("|0...............................................100|\n|");

  // Hard coding light cast camera for now.
  Camera lightCamera;
  lightCamera.fov = (45. / 360.) * 2 * PI;
  lightCamera.fov_ratio = sin(lightCamera.fov / 2.0);
  lightCamera.location = Vector(0, 3, -10);
  lightCamera.look_at = Vector(0, 3, 0);
  lightCamera.right = Vector(1.33, 0, 0);
  lightCamera.up = Vector(0,1,0);

  Raycaster lightCast(config::scenePtr, lightCamera);

  lightCast.surfelCast(1024,768, 1, 1, &writer);

  while( end < jobs)
  {
	  start = end;
	  end = ((end + cores*corethreadcount) < jobs ? (end + cores*corethreadcount) : jobs);

	  printf("Starting JOBS %d - %d\n", start, end-1);

	  for(int i = start; i < end-1; i++) {
		//printf("CREATED JOB [%d]\n", i);
		rtrcs[i] = new raytracer( 0, i * img_size.height / jobs, img_size.width,
				(i + 1) * img_size.height / jobs, img_size.width, img_size.height, config::render_depth, &writer, i );
		thrds[i] = thread{*rtrcs[i]};
	  }

		rtrcs[end-1] = new raytracer( 0, (end-1) * img_size.height / jobs, img_size.width,
				(end) * img_size.height / jobs, img_size.width, img_size.height, config::render_depth, &writer, end-1 );
	  (*rtrcs[end-1])();

	  for(int i = start; i < end-1; i++) {
		//printf("Thread[%d] prepare\n", i);
		//thrds[i]->interrupt();
		thrds[i].join();
		//printf("Thread[%d] join\n", i);
	  }
  }
  
  printf("|\n");
  
  //rtrc1();
  writer.write(config::out_file);
}
