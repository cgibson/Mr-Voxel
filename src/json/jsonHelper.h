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

  if(!val["direction"].empty() && !val["fov_inner"].empty() && !val["fov_outer"].empty()) {
      light->dir = parseVec3(val["direction"]);
      light->dir.norm();
      light->fov_inner = (val["fov_inner"].asDouble() / 360.) * 2 * PI;
      light->fov_outer = (val["fov_outer"].asDouble() / 360.) * 2 * PI;
      light->lightType = LIGHT_SPOTLIGHT;
  }else{
      light->lightType = LIGHT_POINT;
  }
  return light;
}

vector<SmoothTriangle*>
readObj( string path )
{
    FILE *file = fopen( path.c_str(), "rb" );
    if ( !file )
    {
        printf("Failed to load model %s\n", path.c_str());
        exit(1);
    }

    Vec3 tmpVec;
    int v1,v2,v3;
    int n1,n2,n3;
    char buffer[300];

    vector<Vec3> vectors;
    vector<Vec3> normals;
    vector<SmoothTriangle*> tris;
    SmoothTriangle *tri;

    int line = 1;
    
    while(fgets(buffer, 300, file) != NULL)
    {
        
        if(sscanf(buffer, "v %lf %lf %lf\n", &tmpVec.x(), &tmpVec.z(), &tmpVec.y()) == 3)
        {
            vectors.push_back(tmpVec);
        }else if(sscanf(buffer, "vn %lf %lf %lf\n", &tmpVec.x(), &tmpVec.z(), &tmpVec.y()) == 3)
        {
            normals.push_back(tmpVec);
        }else if(sscanf(buffer, "f %d//%d %d//%d %d//%d\n", &v1, &n1, &v2, &n2, &v3, &n3) == 6)
        {
            if((v1-1) < 0 || (v1-1) > (vectors.size()-1) ||
               (v2-1) < 0 || (v2-1) > (vectors.size()-1) ||
               (v3-1) < 0 || (v3-1) > (vectors.size()-1) ||
               (n1-1) < 0 || (n1-1) > (normals.size()-1) ||
               (n2-1) < 0 || (n2-1) > (normals.size()-1) ||
               (n3-1) < 0 || (n3-1) > (normals.size()-1))
            {
                printf("Error: out-of-bounds vector or normal on line %d\n", line);
            }else{
                tri = new SmoothTriangle(vectors[v1-1], normals[n1-1], vectors[v2-1], normals[n2-1], vectors[v3-1], normals[n3-1]);
                tris.push_back(tri);
            }

        }else{
            //printf("Read in unknown on line %d: [%s]\n", line, buffer);
        }
        line++;
    }

    return tris;
}

void parseModel(Json::Value val, Scene *scene)
{
  string file = val["file"].asString();
  double size = val["size"].asDouble();

  Color pigment = parseColor(val["pigment"]);
  Finish finish = parseFinish(val["finish"]);

  //TODO: add modifiers
  //parseModifiers(val["modifiers"], (GeomObj*)plane);

  vector<SmoothTriangle*> tris = readObj(file);

  vector<SmoothTriangle*>::iterator it;

  Vec3 min, max;
  it = tris.begin();

  BBNode tmpNode;
  tmpNode = (*it)->construct_bb();

  min = tmpNode.min;
  max = tmpNode.max;

  // Identify bounds
  for(;it != tris.end(); it++)
  {
      tmpNode = (*it)->construct_bb();
      if(tmpNode.min.x() < min.x())
          min.x(tmpNode.min.x());
      if(tmpNode.min.y() < min.y())
          min.y(tmpNode.min.y());
      if(tmpNode.min.z() < min.z())
          min.z(tmpNode.min.z());

      if(tmpNode.max.x() > max.x())
          max.x(tmpNode.max.x());
      if(tmpNode.max.y() > max.y())
          max.y(tmpNode.max.y());
      if(tmpNode.max.z() > max.z())
          max.z(tmpNode.max.z());
  }

  // Center triangles
  Vec3 offset = (max + min) / 2.;
  min = min - offset;
  max = max - offset;

  float farthest;

  farthest = fabs(min.x());
  if(farthest < fabs(max.x())) farthest = fabs(max.x());

  if(farthest < fabs(max.y())) farthest = fabs(max.y());
  if(farthest < fabs(max.y())) farthest = fabs(max.y());

  if(farthest < fabs(max.z())) farthest = fabs(max.z());
  if(farthest < fabs(max.z())) farthest = fabs(max.z());

  float scaleFactor = size / farthest;

  // Offset/scale triangles
  for(it = tris.begin(); it != tris.end(); it++)
  {
      (*it)->c1(((*it)->c1() - offset) * scaleFactor);
      (*it)->c2(((*it)->c2() - offset) * scaleFactor);
      (*it)->c3(((*it)->c3() - offset) * scaleFactor);
  }

  int count = 0;
  for(it = tris.begin(); it != tris.end(); it++)
  {
      (*it)->finish = finish;
      (*it)->pigment.rgbf = pigment;
      //if(count < 50000)
        scene->addObject((GeomObj*)*it);
      count++;
  }
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

void parseModels(Json::Value models, Scene *scene)
{
  for ( int index = 0; index < models.size(); ++index )
    parseModel( models[index], scene );
}

bool parseWorld(Json::Value val, Scene *scene)
{
  
    parseSpheres(val["spheres"], scene);
    parseBoxes(val["boxes"], scene);
    parsePlanes(val["planes"], scene);
    parsePolygons(val["polygons"], scene);
    parseVolumes(val["volumes"], scene);
    parseModels(val["models"], scene);
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
