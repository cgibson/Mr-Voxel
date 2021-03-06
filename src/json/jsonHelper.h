#include <string>
#include <json/json.h>
#include "../util/util.h"
#include "../scene/scene.h"
#include "../mrvoxel/VolumeRegion.h"
#include "../mrvoxel/HomogeneousRegion.h"
#include "../mrvoxel/BrickDensity.h"
#include "include/json/value.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

inline string slurp(istream& is)
{
  stringstream sstr;
  sstr << is.rdbuf();
  return sstr.str();
}

static string readJsonFile( string path )
{
   FILE *file = fopen( path.c_str(), "rb" );
   if ( !file )
      return string("");
   fseek( file, 0, SEEK_END );
   long size = ftell( file );
   fseek( file, 0, SEEK_SET );
   std::string text;
   char *buffer = new char[size+1];
   buffer[size] = 0;
   if ( fread( buffer, 1, size, file ) == (unsigned long)size )
      text = buffer;
   fclose( file );
   delete[] buffer;
   return text;
}

Dimension parseDimension(const Json::Value val)
{
  int index = 0;
  Dimension ret = {val[index].asInt(), val[index + 1].asInt()};
  return ret;
}

Vec3 parseVec3(const Json::Value val)
{
  int index = 0;
  return Vec3(
             val[index].asDouble(),
             val[index+1].asDouble(),
             val[index+2].asDouble()
           );
}

Color parseColor(const Json::Value val)
{
  int index = 0;
  if(val.size() == 3)
    return Color(
               val[index].asDouble(),
               val[index+1].asDouble(),
               val[index+2].asDouble()
             );
  else
    return Color(
               val[index].asDouble(),
               val[index+1].asDouble(),
               val[index+2].asDouble(),
               val[index+3].asDouble()
             );
}

bool parseCamera(Json::Value val, Scene *scene)
{
  Camera *camera = new Camera();
  
  camera->location = parseVec3(val["eye"]);
  camera->up = parseVec3(val["up"]);
  camera->right = parseVec3(val["right"]);
  camera->look_at = parseVec3(val["look_at"]);
  camera->fov = val["fov"].asDouble();
  // translation into radians:

  camera->fov = (camera->fov / 360.) * 2 * PI;

  // we divide the FOV to create the right triangle
  camera->fov_ratio = sin(camera->fov / 2.0);
  
  scene->setCamera(camera);
  return true;
}

void cndSetDouble(Json::Value val, double *ptr)
{
  //if(val != NULL)
  //{
    *ptr = val.asDouble();
  //}
}

Finish parseFinish(Json::Value val)
{
  Finish finish;
  cndSetDouble(val["ambient"], &finish.ambient);
  cndSetDouble(val["diffuse"], &finish.diffuse);
  cndSetDouble(val["specular"], &finish.specular);
  cndSetDouble(val["roughness"], &finish.roughness);
  cndSetDouble(val["reflection"], &finish.reflection);
  cndSetDouble(val["refraction"], &finish.refraction);
  cndSetDouble(val["ior"], &finish.ior);
  
  return finish;
}

Modifier* parseModifier(Json::Value val)
{
  if(val["translate"].size() > 0)
    return new Translation(parseVec3(val["translate"]));
  if(val["scale"].size() > 0)
    return new Scale(parseVec3(val["scale"]));
  if(val["rotate"].size() > 0)
    return new Rotation(parseVec3(val["rotate"]));
}

void parseModifiers(Json::Value modifiers, GeomObj* object)
{
  if(modifiers.size() <= 0)
    return;
  object->modifiers = (Modifier**)malloc(sizeof(Modifier*) * modifiers.size());
  object->modifier_count = modifiers.size();
  
  for ( int index = 0; index < modifiers.size(); ++index )
  {
    object->modifiers[index] = parseModifier( modifiers[index] );
  }
  
  object->generateMatrix();
}

Sphere *parseSphere(Json::Value val)
{
  Sphere *sphere = new Sphere();
  sphere->center = parseVec3(val["center"]);
  sphere->radius = val["radius"].asDouble();
  sphere->pigment.rgbf = parseColor(val["pigment"]);
  sphere->finish = parseFinish(val["finish"]);
  parseModifiers(val["modifiers"], (GeomObj*)sphere);
  return sphere;
}

Box *parseBox(Json::Value val)
{
  Box *box = new Box();
  box->min(parseVec3(val["min"]));
  box->max(parseVec3(val["max"]));
  box->pigment.rgbf = parseColor(val["pigment"]);
  box->finish = parseFinish(val["finish"]);
  parseModifiers(val["modifiers"], (GeomObj*)box);
  return box;
}

vector<GeomObj*> parsePolygon(Json::Value val)
{
    vector<GeomObj*> triangles;
    Json::Value vlist = val["vertices"];

    if(vlist.size() < 2){
        printf("Error: Vertex count invalid for polygon\n");
        exit(1);
    }

    std::vector<Vec3> vertices;

    Color color = parseColor(val["pigment"]);
    Finish finish = parseFinish(val["finish"]);

    int i = 0;

    vertices.push_back(parseVec3(vlist[i])); i++;
    vertices.push_back(parseVec3(vlist[i])); i++;

    Triangle *tri;

    for(i = 2; i < vlist.size(); i++) {
        vertices.push_back(parseVec3(vlist[i]));
        tri = new Triangle(vertices[0], vertices[i-1], vertices[i]);
        parseModifiers(val["modifiers"], (GeomObj*)tri);
        tri->pigment.rgbf = color;
        tri->finish = finish;
        triangles.push_back(tri);
    }
    return triangles;
}

Plane *parsePlane(Json::Value val)
{
  Plane *plane = new Plane();
  plane->normal = parseVec3(val["normal"]);
  plane->distance = val["distance"].asDouble();
  plane->pigment.rgbf = parseColor(val["pigment"]);
  plane->finish = parseFinish(val["finish"]);
  parseModifiers(val["modifiers"], (GeomObj*)plane);
  return plane;
}

unsigned short twoByte2Short(char *ptr)
{
  unsigned short r = 0;
  r |= *ptr & 0xFF;
  r <<= 8;
  ptr++;
  r |= *ptr & 0xFF;
  return r;
}

VolumeRegion *parseVolume(Json::Value val)
{
  Vec3 min = parseVec3(val["min"]);
  Vec3 max = parseVec3(val["max"]);

  VolumeRegion *region;

  // homogeneous, data, procedural
  string type = val["type"].asString();

  if(!type.compare("brick")) {

      double density = val["density"].asDouble();
      Color absorbtion = parseColor(val["absorbtion"]);
      Color scatter = parseColor(val["scatter"]);
      Color emitt = parseColor(val["emitt"]);
      double greenstein = val["greenstein"].asDouble();

      string file = val["file"].asString();
      Vec3 file_res = parseVec3(val["file_resolution"]);
      Vec3 vol_res = parseVec3(val["volume_resolution"]);

      int iso_min = val["iso_min"].asInt();
      int iso_max = val["iso_max"].asInt();

      region = new BrickDensityRegion(min, max, absorbtion, scatter, greenstein, \
                                             emitt, density);

      (dynamic_cast<BrickDensityRegion*>(region))->loadCT(file, file_res, vol_res, iso_min, iso_max);

  }else if(!type.compare("ocean")) {

      double density = val["density"].asDouble();
      Color absorbtion = parseColor(val["absorbtion"]);
      Color scatter = parseColor(val["scatter"]);
      Color emitt = parseColor(val["emitt"]);
      double greenstein = val["greenstein"].asDouble();

      string file = val["file"].asString();
      Vec3 vol_res = parseVec3(val["volume_resolution"]);

      int iso_min = val["iso_min"].asInt();
      int iso_max = val["iso_max"].asInt();

      region = new BrickDensityRegion(min, max, absorbtion, scatter, greenstein, \
                                      emitt, density);

      (dynamic_cast<BrickDensityRegion*>(region))->loadOceanData(file, vol_res, iso_min, iso_max);
      
  }else if(!type.compare("homogeneous")) {

	  double density = val["density"].asDouble();
	  Color absorbtion = parseColor(val["absorbtion"]);
	  Color scatter = parseColor(val["scatter"]);
	  Color emitt = parseColor(val["emitt"]);
	  double greenstein = val["greenstein"].asDouble();

	  region = new HomogeneousRegion(min, max, absorbtion, scatter, greenstein, \
			                         emitt, density);

  }else{
	  printf("ERROR: Unsupported volume type\n");
	  exit(1);
  }
  

  //Volume *volume = new Volume(min, max, size);
  
  //volume->pigment.rgbf = parseColor(val["pigment"]);
  //volume->finish = parseFinish(val["finish"]);
  //parseModifiers(val["modifiers"], (GeomObj*)volume);

  //loadVolumeData(val["data"], volume, size);

  return region;
}

LightSource *parseLight(Json::Value val)
{
  LightSource *light = new LightSource();
  light->position = parseVec3(val["location"]);
  light->color = parseColor(val["color"]);
  return light;
}

void parseBoxes(Json::Value boxes, Scene *scene)
{
  for ( int index = 0; index < boxes.size(); ++index )
    scene->addObject((GeomObj*)parseBox( boxes[index] ));
}

void parseSpheres(Json::Value spheres, Scene *scene)
{
  for ( int index = 0; index < spheres.size(); ++index )
    scene->addObject((GeomObj*)parseSphere( spheres[index] ));
}

void parsePlanes(Json::Value planes, Scene *scene)
{
  for ( int index = 0; index < planes.size(); ++index )
    scene->addObject((GeomObj*)parsePlane( planes[index] ));
}

void parsePolygons(Json::Value polygons, Scene *scene)
{
  for ( int index = 0; index < polygons.size(); ++index )
    scene->addObjects(parsePolygon( polygons[index] ));
}

void parseVolumes(Json::Value volumes, Scene *scene)
{
  for ( int index = 0; index < volumes.size(); ++index )
    scene->addVolume(parseVolume( volumes[index] ));
}

void parseLights(Json::Value lights, Scene *scene)
{
  for ( int index = 0; index < lights.size(); ++index )
    scene->addLightSource(parseLight( lights[index] ));
}

bool parseWorld(Json::Value val, Scene *scene)
{
  
    parseSpheres(val["spheres"], scene);
    parseBoxes(val["boxes"], scene);
    parsePlanes(val["planes"], scene);
    parsePolygons(val["polygons"], scene);
    parseVolumes(val["volumes"], scene);
    parseLights(val["lights"], scene);
    return true;
}

bool parseVolSampleStep(Json::Value val, Scene *scene)
{
    scene->setVolSampleStep(val.asDouble());
    return true;
}

bool parseScene(Json::Value val, Scene *scene)
{
  bool ret = true;
  
  ret &= parseWorld(val, scene);
  ret &= parseCamera(val["camera"], scene);
  ret &= parseVolSampleStep(val["volSampleStep"], scene);
  return ret;
}
