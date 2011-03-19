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

using namespace std;
using namespace boost;


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


int main(int argc, char* const argv[])
{

  Parser *parser = new JSONParser();
  Scene *scene;

  int width = -1;
  int height = -1;
  char *filename = NULL;
  int current;
  int depth = 5;
  bool single_thread = false;

  current = 1;
  while(current < argc) {
    if( strcmp(argv[current], "-I") == 0 ) {
      current++;
      //DON'T EVER EVER DO THIS FOR THE LOVE OF GOD
      //Remember to add the '+1' to allow for the null byte
      //filename = (char*)malloc( (strlen(argv[current]) + 1) * sizeof(char) );
      //sprintf(filename, "%s", argv[current]);

      filename = argv[current];
      current++;
    }
    else if( strcmp(argv[current], "-W") == 0 ) {
      current++;
      sscanf(argv[current], "%d", &width);
      current++;
    }
    else if( strcmp(argv[current], "-H") == 0 ) {
      current++;
      sscanf(argv[current], "%d", &height);
      current++;
    }
    else if( strcmp(argv[current], "-D") == 0 ) {
      current++;
      sscanf(argv[current], "%d", &depth);
      current++;
    }
    else if( strcmp(argv[current], "-s") == 0 ) {
      single_thread = true;
      current++;
    }
    else {
      cout << argv[current] << endl;
      printf("Usage: lab2 -I[filename] -Wwidth -Hheight\n");
      exit(1);
    }
  }
  
  Dimension size;
  
  scene = parser->parse_file(filename, &size);
  
  if( (width <= 0) || (height <= 0) ) {
    width = size.x();
    height = size.y();
  }

  char *outfile;

  if(filename != NULL) {
    char *tmp = strtok(filename, ".");
    outfile = (char*)malloc(sizeof(char) * (strlen(tmp) + 5));
    
    if(outfile == NULL) {
      perror("Error: improper filename input");
      exit(-1);
    }
    sprintf(outfile, "%s.tga", tmp);
    
  }
  else {
    outfile = (char*)malloc(sizeof(char) * 30);
    sprintf(outfile, "tmp_render.tga");
  }

  if( parser->succeeded() ) {
    printf("Scene was compiled correctly.\n");
  }
  else {
    printf("Was not able to compile scene.  Errors exist.\n");
    return -1;
  }
  scene->generateBoxHierarchy();
  
  ImageWriter writer = ImageWriter( width, height );
  
  raycaster = new Raycaster( scene );
  
  int cores;
  int corethreadcount;
  int jobs;

  if(single_thread) {
	  cores = 1;
	  corethreadcount = 1;
	  jobs = cores*corethreadcount*1;
  } else {
	  cores = 4;
	  corethreadcount = 5;
	  jobs = cores*corethreadcount*2;
  }
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

  Raycaster lightCast(scene, lightCamera);

  lightCast.surfelCast(width,height,2, 2, &writer);

  jobs = -1;

/*
  while( end < jobs)
  {
	  start = end;
	  end = ((end + cores*corethreadcount) < jobs ? (end + cores*corethreadcount) : jobs);

	  printf("Starting JOBS %d - %d\n", start, end-1);

	  for(int i = start; i < end-1; i++) {
		//printf("CREATED JOB [%d]\n", i);
		rtrcs[i] = new raytracer( 0, i * height / jobs, width,
				(i + 1) * height / jobs, width, height, depth, &writer, i );
		thrds[i] = thread{*rtrcs[i]};
	  }

		rtrcs[end-1] = new raytracer( 0, (end-1) * height / jobs, width,
				(end) * height / jobs, width, height, depth, &writer, end-1 );
	  (*rtrcs[end-1])();

	  for(int i = start; i < end-1; i++) {
		//printf("Thread[%d] prepare\n", i);
		//thrds[i]->interrupt();
		thrds[i].join();
		//printf("Thread[%d] join\n", i);
	  }
  }
 */
  /*
  for(int i = 0; i < threads-1; i++) {
  		rtrcs[i] = new raytracer( 0, i * height / threads, width,
  				(i + 1) * height / threads, width, height, depth, &writer, i );
  		thrds[i] = thread{*rtrcs[i]};
  		printf("Thread created\n");
  	  }

  	  (*rtrcs[threads-1])();

  	  for(int i = 0; i < threads; i++) {
  		//printf("Thread[%d] prepare\n", i);
  		//thrds[i]->interrupt();
  		thrds[i].join();
  		printf("Thread[%d] join\n", i);
  	  }*/
  
  printf("|\n");
  
  //rtrc1();
  writer.write(outfile);
}
