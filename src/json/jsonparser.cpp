#include <stdio.h>
#include <iostream>
#include "jsonparser.h"
#include <json/json.h>
#include "jsonHelper.h"
#include "../util/util.h"

Parser::Parser()
{
  error = false;
}

Scene *Parser::parse_file(string filename)
{
  printf("O NOES\n");
  return NULL;
}

JSONParser::JSONParser()
{
  error = false;
}

Scene * initialize_initial_scene()
{
  // scene
  Scene *scene = new Scene();
  
  // camera
  Camera *camera = new Camera();
  camera->location = Vector(0,5,14);
  camera->look_at = Vector(0,0,0);
  camera->right = Vector(1.3333,0,0);
  camera->up = Vector(0,1,0);
  scene->setCamera(camera);
  
  LightSource *light = new LightSource();
  light->position = Vector(0, 20, 0);
  light->color = Color(1.5, 1.5, 1.5);
  scene->addLightSource(light);
  
  Sphere *sphere = new Sphere();
  sphere->center = Vector(0,0,0);
  sphere->radius = 2;
  sphere->pigment.rgbf = Color(0.2, 0.2, 1.0);
  sphere->finish.ambient = 0.2;
  sphere->finish.diffuse = 0.8;
  sphere->finish.specular = 0.8;
  sphere->finish.roughness = 0.004;
  sphere->finish.reflection = 0.3;
  sphere->finish.refraction = 0.0;
  sphere->finish.ior = 1.0;
  scene->addObject((GeomObj*)sphere);
  
  Plane *plane = new Plane();
  plane->normal = Vector(0,1,0);
  plane->distance = -3;
  plane->pigment.rgbf = Color(1.0, 1.0, 1.0);
  plane->finish.ambient = 0.2;
  plane->finish.diffuse = 0.8;
  plane->finish.specular = 0.8;
  plane->finish.roughness = 0.004;
  plane->finish.reflection = 0.3;
  plane->finish.refraction = 0.0;
  plane->finish.ior = 1.0;
  scene->addObject((GeomObj*)plane);
 
}

Scene *JSONParser::parse_file(string filename)
{
  Scene * scene = new Scene(); //initialize_initial_scene();
 
  Json::Value root;
  Json::Reader reader;
  
  string profile_json;
  
  if(!filename.empty())
    profile_json = readJsonFile(filename);
  else
    profile_json = slurp(cin);
  
  bool parsingSuccessful = reader.parse( profile_json, root );
  if ( !parsingSuccessful )
  {
      std::cout  << "Failed to parse configuration\n"
                 << reader.getFormatedErrorMessages();
      error = true;
      return NULL;
  }
  
  //*size = parseDimension(root["render_size"]);
 
  parseScene(root, scene);
 
  return scene;
}
