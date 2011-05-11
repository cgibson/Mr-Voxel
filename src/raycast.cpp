#include <boost/thread/thread.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "raycast.h"
#include "light/brdf.h"
#include "integrator/integrator.h"

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
Raycaster::Raycaster()
{
  mCastMode = PERSPECTIVE;
  _camera = *(config::scenePtr->getCamera());
  Vec3 right = _camera.right;
  Vec3 up = _camera.up;
  double ratio =  right.norm() / up.norm();

  mLeft = -_camera.fov_ratio * ratio;
  mRight = _camera.fov_ratio * ratio;
  mTop = _camera.fov_ratio;
  mBottom = -_camera.fov_ratio;

  mNear = 0.5;
  config::background = Color(0.0, 0.0, 0.0, 1.0);


  _matrix = _camera.perspectiveMatrix();
}

/*
 * Default Raycaster Constructor given the current scene with an overrided camera
 *----------------------------------------------------------------------------*/
Raycaster::Raycaster( const Camera &cam )
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
  config::background = Color(0.0, 0.0, 0.0, 1.0);

  _matrix = _camera.perspectiveMatrix();

}

/*
 * New Sum Lights for a given surface
 * ---------------------------------------------------------------------------*/

Color Raycaster::sumLights( const Surface &surface, const Ray &ray, int specular, int ambient, bool gather ) {
    Color result = 0.; // resulting color to be returned

    Vec3 V = ray.direction * -1;

    // Gather indirect light
    if(ambient != 0)
    {
        result = light::shadeIndirect(surface, gather, ambient);
    }
    // Determine diffuse light
    result = result + light::shadeDiffuse(V, surface, specular);
    
    return result;
}

/*
 * New Handle Intersect Code
 * ---------------------------------------------------------------------------*/
Color Raycaster::handleIntersect( const Ray &ray, int depth, bool ambientTest = true, bool attenuateDist = false )
{
    Color color;

    Surface surface;

    if(depth <= 0) {

        return config::background;
    }

    // Check for intersect.  Throw away ray if no intersect
    if( !config::scenePtr->intersect(ray, &surface) ) {
        Color vol_color(0.);
        Color vol_transmittance(1.);

        vol_color = config::volume_integrator->Li( Ray(ray.start, ray.direction, 0.0, INFINITY), &vol_transmittance, ambientTest );

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
        
        color = color + sumLights(surface, ray, 1, ambientTest ? 1 : 0, true);

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

        vol_color = config::volume_integrator->Li( Ray(ray.start, ray.direction, 0.0, surface.t), &vol_transmittance, ambientTest );
        
        //return color + reflect_color * reflection;
        if(!success && surface.finish.refraction ) {
            color = color * (1 - surface.color.f()) + color_reflect;
        }

        color = color * (1 - surface.color.f()) + (color_reflect * surface.finish.reflection)
               + (color_refract * surface.color.f());

        if(attenuateDist)
        {
            float Att = 1. / (config::atten_k * pow(surface.t, 2));
            Att = (Att > 1.0) ? 1.0 : Att;
            color = color * Att;
        }

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
        {
            Color Tr = 1.;
            color = config::scenePtr->lightCache()->gather(ray, &t, &Tr);
        }
            break;
        case TARGET_LIGHT_CACHE_TEST_COUNT:
        {
            int testCount;
            config::scenePtr->lightCache()->getTestCount(ray, &testCount);
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
    if(config::scenePtr->intersect(*ray, surface)) {
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
              color.r(),
              color.g(),
              color.b(),
              color.f()
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

void
Raycaster::addSurfel(const Surface &surf, const Ray &ray)
{
    const Vec3 surfel_p = ray(surf.t);
    const Vec3 surfel_n = surf.n;
    const Color color = sumLights(surf, ray, 0, 0, false);
    const float invvdotn = 1. - abs(surf.n.dot(ray.direction));

    float growth = invvdotn;// + (surf.t / 20.);
    //growth = (growth > 1.0) ? 1.0 : growth;
    

    float size = config::surfel_size + (config::surfel_size * config::surfel_grow * growth);

    shared_ptr<Surfel> surfel_ptr = shared_ptr<Surfel>(new Surfel(surfel_p, surfel_n, color, size));

    config::scenePtr->addSurfel(surfel_ptr);

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

  Ray ray;
  int surfelCount = 0;

  Surface surf;

  config::scenePtr->initCache(Vec3(-15, -15, -15), Vec3(15, 15, 15));

  for( x = 0; x < size.width; x+=step_x ) {

    for( y = 0; y < size.height; y+=step_y ) {

      cam2World(x, y, size.width, size.height, &ray);
      
      color = 0;

      vector<Surface> isectList;

      vector<Surface>::iterator surfit;


      /*
      if(!config::scenePtr->intersections(ray, &isectList))
          continue;
      for(surfit = isectList.begin(); surfit != isectList.end(); surfit++) {
          surf = *surfit;
       //*/
      while(config::scenePtr->intersect(ray, &surf)) {

            // Invert backwards normals
            if(surf.objPtr->getType() == TRIANGLE && surf.n.dot(ray.direction * -1) < 0) {
                surf.n = surf.n * -1;
            }

          addSurfel(surf, ray);
          surfelCount++;
          ray.start = ray(surf.t + 0.1);
          
      }

    }
  }



    const int numVolumes = config::scenePtr->getVolumeCount();
    VolumeRegion **volumes = config::scenePtr->getVolumes();

    VolumeRegion *volume;
    Vec3 min, max;

    float sample_size = config::lvoxel_size;
    float test_count = config::vol_tests_per_sample * config::vol_tests_per_sample * config::vol_tests_per_sample;
    double test_step = (double)sample_size / ((double)config::vol_tests_per_sample);
    assert(test_step > 0.0);
    float sample_size_div_two = sample_size * 0.5;
    float radius = sqrt(sample_size_div_two * sample_size_div_two + sample_size_div_two * sample_size_div_two + sample_size_div_two * sample_size_div_two);
    Color Tr, TrTot;
    Ray shadow_ray;

    LightSource** lights = config::scenePtr->getLightSources();
    int light_count = config::scenePtr->getLightSourceCount();

    LightSource* light;

    Surface surface;

    int lvoxelCount = 0;

    if(numVolumes > 0) {

        // ADD all volume representations to the scene
        for(int i = 0; i < numVolumes; i++) {
            volume = volumes[i];
            min = volume->bounds().min;
            max = volume->bounds().max;
            for(float x = min.x(); x < max.x(); x += sample_size) {
                for(float y = min.y(); y < max.y(); y += sample_size) {
                    for(float z = min.z(); z < max.z(); z += sample_size) {

                        Vec3 p = Vec3(x + sample_size * 0.5,y + sample_size * 0.5,z + sample_size * 0.5);
                        Vec3 L;
                        double l_dist;
                        // Generate shadow ray

                        Color sig_t = 0;
                        Color sig_s = 0;

                        for(float tx = x; tx < x + sample_size; tx += test_step) {
                            for(float ty = y; ty < y + sample_size; ty += test_step) {
                                for(float tz = z; tz < z + sample_size; tz += test_step) {
                                    Vec3 test = Vec3(tx,ty,tz);
                                    sig_t = sig_t + volume->sigma_t(test);
                                    sig_s = sig_s + volume->sigma_s(test);
                                }
                            }
                        }

                        sig_t = sig_t / (float)test_count;
                        sig_s = sig_s / (float)test_count;

                        if(sig_t.toTrans() < 0.0)
                            printf("T below 0.0: %s\n", sig_t.str());
                        if(sig_s.toTrans() < 0.0)
                            printf("S below 0.0: %s\n", sig_s.str());


                        //sig_t.clamp(0.0, 100.0);
                        //sig_s.clamp(0.0, 100.0);


                        if(!sig_s.isBlack() && !sig_t.isBlack())
                        {

                            //printf("Sigma_s: %s\n", sig_s.str());
                            TrTot = 0.;
                            for(int i = 0; i < light_count; i++)
                            {
                                
                                light = lights[i];
                                L = light->position - p;
                                l_dist = L.norm();
                                shadow_ray = Ray(p, L, 0.0, l_dist);

                                if(!config::scenePtr->intersect(shadow_ray, &surface) || (surface.t > l_dist)) {
                                    Tr = config::volume_integrator->Transmittance( shadow_ray );
                                    TrTot = TrTot + (Tr * light->sample(p));
                                }
                            }


                            config::scenePtr->addLVoxel(shared_ptr<LVoxel>(new LVoxel(p, Color(0.1,0.1,0.1), TrTot, sig_t, sig_s, radius)));
                            lvoxelCount++;
                        }
                    }

                }
            }
        }
    }

  printf("Lvoxel Count: %d\n", lvoxelCount);
  printf("surfel Count: %d\n", surfelCount);

  config::scenePtr->lightCache()->postprocess();

  return 0;
}
