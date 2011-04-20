#include <boost/thread/thread.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "raycast.h"
#include "light/brdf.h"

//using namespace light;
using namespace sys;
using sample::HemisphereSampler;

namespace this_thread = boost::this_thread;

#define DEBUG 1
//#define false 0
//#define true 1

/*
 * Default Raycaster Constructor given the current scene
 *----------------------------------------------------------------------------*/
Raycaster::Raycaster( Scene* scene )
{
  mCastMode = PERSPECTIVE;
  _camera = *scene->getCamera();
  Vec3 right = _camera.right;
  Vec3 up = _camera.up;
  double ratio =  right.norm() / up.norm();

  mLeft = -_camera.fov_ratio * ratio;
  mRight = _camera.fov_ratio * ratio;
  mTop = _camera.fov_ratio;
  mBottom = -_camera.fov_ratio;

  mNear = 0.5;
  mScene = scene;
  config::background = Color(0.0, 0.0, 0.0, 1.0);


  _matrix = _camera.perspectiveMatrix();
}

/*
 * Default Raycaster Constructor given the current scene with an overrided camera
 *----------------------------------------------------------------------------*/
Raycaster::Raycaster( Scene* scene, const Camera &cam )
{
  mCastMode = PERSPECTIVE;
  _camera = cam;
  Vec3 right = _camera.right;
  Vec3 up = _camera.up;
  double ratio =  right.norm() / up.norm();

  mLeft = -_camera.fov_ratio * ratio;
  mRight = _camera.fov_ratio * ratio;
  mTop = _camera.fov_ratio;
  mBottom = -_camera.fov_ratio;

  mNear = 0.5;
  mScene = scene;
  config::background = Color(0.0, 0.0, 0.0, 1.0);

  _matrix = _camera.perspectiveMatrix();

}

/*
 * New Sum Lights for a given surface
 * ---------------------------------------------------------------------------*/

Color Raycaster::sumLights( const Surface &surface, const Ray &ray, int specular, int ambient, bool gather ) {
    Color result; // resulting color to be returned

    Vec3 V = ray.direction * -1;

    // Gather indirect light
    result = light::shadeIndirect(surface, gather, ambient);

    // Determine diffuse light
    result = result + light::shadeDiffuse(V, surface, specular);
    
    return result;
}

/*
 * New Handle Intersect Code
 * ---------------------------------------------------------------------------*/
Color Raycaster::handleIntersect( const Ray &ray, int depth )
{
    Color color;

    Surface surface;

    if(depth <= 0) {

        return config::background;
    }

    // Check for intersect.  Throw away ray if no intersect
    if( !mScene->intersect(ray, &surface) ) {
        Color vol_color(0.);
        Color vol_transmittance(1.);

        vol_color = config::volume_integrator->Li( Ray(ray.start, ray.direction, 0.0, INFINITY), &vol_transmittance );

        return (config::background * vol_transmittance) + vol_color;
    }

    const Vec3 D = ray.direction;
    const Vec3 N = surface.n;
    const Vec3 R = ray.direction.reflect(N);
    const Vec3 P = surface.p;
    const Ray ray_reflect = Ray(P, R);

    Color color_reflect;
    Color color_refract;

    if(surface.type == SolidSurface) {



        // Invert backwards normals
        if(surface.objPtr->getType() == TRIANGLE && surface.n.dot(D * -1) < 0) {
            surface.n = surface.n * -1;
        }
        
        color = color + sumLights(surface, ray, 1, 1, true);

        int success;
        Vec3 Drefract;
        // if the object is translucent
        if( surface.finish.refraction && (surface.color.f() > 0.0)) {
            //if the normals are pointing out
            if(D * N < 0) {
                //create refraction array into the air
                Drefract = D.refract(N, 1.0, surface.finish.ior, &success);
                //cos_theta = (V * -1) * N);
            } else {
                //attempt to create internal refracted ray
                Drefract = D.refract(N * -1, surface.finish.ior, 1.0, &success);
            }

            // If refraction is possible
            if(success) {
                // Refract the ray
                Ray ray_refract = Ray(surface.p, Drefract);
                color_refract = handleIntersect(ray_refract, depth - 1);
            } else {
                // It is entirely reflection
                color_refract = Color(0,0,0);
            }
        }

        // Handle reflection
        if( surface.finish.reflection) {

            color_reflect = handleIntersect(ray_reflect, depth - 1);
        }

        Color vol_color(0.);
        Color vol_transmittance(1.);

        vol_color = config::volume_integrator->Li( Ray(ray.start, ray.direction, 0.0, surface.t), &vol_transmittance );
        
        //return color + reflect_color * reflection;
        if(!success && surface.finish.refraction ) {
            color = color * (1 - surface.color.f()) + color_reflect;
        }

        color = color * (1 - surface.color.f()) + (color_reflect * surface.finish.reflection)
               + (color_refract * surface.color.f());

      Color tmp = (color * vol_transmittance) + (vol_color);

        return tmp;
    }
}

/*
 * New Ray Color Cast
 *----------------------------------------------------------------------------*/
Color Raycaster::initialCast( const Ray &ray, int depth )
{
    if(depth <= 0)
        return config::background;

    // Shoot the ray, which starts in spaaaaace!
    return handleIntersect( ray, depth );
}

/*
 * Cast the given 'pixel' into the world
 *----------------------------------------------------------------------------*/
Color Raycaster::cast( int x, int y, int width, int height )
{
    Color color;

    double us = mLeft + ((mRight - mLeft)
        * (double)((double)(x + 0.5f) / width));
    double vs = mBottom + ((mTop - mBottom)
        * (double)((double)(y + 0.5f) / height));
    //cout << "US: " << us << "  VS: " << vs << endl;

    Ray ray;

    ray.start.set(0, 0, 0);
    ray.start = ray.start + Vec3(us, vs, 0);

    ray.direction = Vec3(0,0,-1);
    ray.direction.norm();
    ray.direction = ray.direction + Vec3(us, vs, 0);


    ray.start = _matrix * Vec4(ray.start, 1);
    ray.direction = _matrix * Vec4(ray.direction, 0);
    ray.direction.norm();

    double t;

    switch(config::render_target) {
        case TARGET_LIGHT_CACHE_RESULT:
            color = mScene->lightCache()->gather(ray, &t);
            break;
        case TARGET_LIGHT_CACHE_TEST_COUNT:
        {
            int testCount;
            mScene->lightCache()->getTestCount(ray, &testCount);
            testCount = (testCount < 255) ? testCount : 255;
            //printf("TEST: %d\n", testCount);
            color = (float)testCount / 50.;
        }
            break;
        case TARGET_DIFFUSE:
        case TARGET_AMBIENT:
        case TARGET_FULL:
            color = initialCast(ray, mDepth);
            break;
    }


    color.clamp(0, 1);
    return color;
}

int Raycaster::iterate( Ray *ray, Surface *surface) {
    if(mScene->intersect(*ray, surface)) {
        (*ray).start = (*ray)(surface->t);
        return 1;
    }

    return 0;
}

/*
 * Cast the given 'pixel' into the world
 *----------------------------------------------------------------------------*/
void Raycaster::cam2World( int x, int y, int width, int height, Ray *external_ray )
{

    const double us = mLeft + ((mRight - mLeft)
        * (double)((double)(x + 0.5f) / width));
    const double vs = mBottom + ((mTop - mBottom)
        * (double)((double)(y + 0.5f) / height));
    //cout << "US: " << us << "  VS: " << vs << endl;
    Ray ray = Ray();

    ray.start.set(0, 0, 0);
    ray.start = ray.start + Vec3(us, vs, 0);

    ray.direction = Vec3(0,0,-1);
    ray.direction.norm();
    ray.direction = ray.direction + Vec3(us, vs, 0);

    ray.start = _matrix * Vec4(ray.start, 1);
    ray.direction = _matrix * Vec4(ray.direction, 0);
    ray.direction.norm();

    *external_ray = ray;
}

/*
 * Default Raycast Constructor with given width, height and filename
 *----------------------------------------------------------------------------*/
int Raycaster::raycast(
        int start_x,
        int start_y,
        int end_x,
        int end_y,
        int width,
        int height,
        int depth,
        ImageWriter *writer)
{
  const int rays = width * height;
  int print_percent = 1;
  int rays_done = 0;
  int percent_done = 0;
  //writer.fill( 0, 0, 0, 0 );
  Color color;
  double low = -1;
  double high = -1;
  int x, y;
  mDepth = depth;

  for( x = start_x; x < end_x; x++ ) {

    //boost::xtime xt;
    //boost::xtime_get(&xt, boost::TIME_UTC);
    //xt.nsec += 10000;
    this_thread::sleep(boost::posix_time::milliseconds(1));

    for( y = start_y; y < end_y; y++ ) {

      if(low == -1) {
          low = color.r();
          high = color.r();
      }
      color = cast(x, y, width, height);

      writer->setPixel(x, y,
              color.r() * 255,
              color.g() * 255,
              color.b() * 255,
              (color.f() == 1.0) ? 255 : 255
      );

      if(color.r() < low) low = color.r();
      if(color.r() > high) high = color.r();
      if(color.g() < low) low = color.g();
      if(color.g() > high) high = color.g();
      if(color.b() < low) low = color.b();
      if(color.b() > high) high = color.b();
      rays_done++;
      percent_done = (int)((double)rays_done/rays * 100);
      if((percent_done % 2 == 0)) {
        if(print_percent) {
          printf(".", percent_done);
          print_percent = 0;
          fflush(stdout);
        }
      }
      else {
        if(!print_percent)
          print_percent = 1;
      }
    }
  }
  return 0;
}

/*
 * Default Raycast Constructor with given width, height and filename
 *----------------------------------------------------------------------------*/
int Raycaster::surfelCast(
        const Dimension &size,
        int step_x,
        int step_y,
        ImageWriter *writer)
{

  //writer.fill( 0, 0, 0, 0 );
  Color color;
  int x, y;

  Surface surf;
  Ray ray;

  float vdotn;

  mScene->initCache(Vec3(-15, -15, -15), Vec3(15, 15, 15));

  for( x = 0; x < size.width; x+=step_x ) {

    for( y = 0; y < size.height; y+=step_y ) {

      cam2World(x, y, size.width, size.height, &ray);
      
      color = 0;

      while(mScene->intersect(ray, &surf)) {

            // Invert backwards normals
            if(surf.objPtr->getType() == TRIANGLE && surf.n.dot(ray.direction * -1) < 0) {
                surf.n = surf.n * -1;
            }

            vdotn = abs(surf.n.dot(ray.direction));

          color = sumLights(surf, ray, 0, 0, false);
          mScene->addSurfel(shared_ptr<Surfel>(new Surfel(ray(surf.t), surf.n, color, config::surfel_size + (config::surfel_size * config::surfel_grow * (1 - vdotn)))));
          ray.start = ray(surf.t + 0.1);
          
      }

    }
  }



    const int numVolumes = mScene->getVolumeCount();
    VolumeRegion **volumes = mScene->getVolumes();

    VolumeRegion *volume;
    Vec3 min, max, sample;

    sample = config::vol_sample_size;

    Color Tr;
    Ray shadow_ray;

    LightSource** lights = config::scenePtr->getLightSources();

    LightSource* light = lights[0];

    Surface surface;

    if(numVolumes > 0) {

        // ADD all volume representations to the scene
        for(int i = 0; i < numVolumes; i++) {
            volume = volumes[i];
            min = volume->bounds().min;
            max = volume->bounds().max;
            for(float x = min.x(); x < max.x(); x += sample.x()) {
                for(float y = min.y(); y < max.y(); y += sample.y()) {
                    for(float z = min.z(); z < max.z(); z += sample.z()) {

                        Vec3 p = Vec3(x + sample.x() * 0.5,y + sample.y() * 0.5,z + sample.z() * 0.5);
                        Vec3 L = light->position - p;
                        double l_dist = L.norm();
                        // Generate shadow ray
                        shadow_ray = Ray(p, L, 0.0, l_dist);

                        if(config::scenePtr->intersect(shadow_ray, &surface) && (surface.t <= l_dist)) {
                            Tr = 0.0;
                        }else{

                            Tr = config::volume_integrator->Transmittance( shadow_ray );
                        }

                        mScene->addLVoxel(shared_ptr<LVoxel>(new LVoxel(p, Color(0.2,0.2,0.2), Tr, sample.length())));
                    }

                }
            }
        }
    }

  mScene->lightCache()->postprocess();

  return 0;
}
