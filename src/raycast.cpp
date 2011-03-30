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
  Vector right = _camera.right;
  Vector up = _camera.up;
  double ratio =  right.norm() / up.norm();

  mLeft = -_camera.fov_ratio * ratio;
  mRight = _camera.fov_ratio * ratio;
  mTop = _camera.fov_ratio;
  mBottom = -_camera.fov_ratio;

  printf("left: %f, right: %f, up: %f, down: %f\n", mLeft,mRight,mTop,mBottom);

  mNear = 0.5;
  mScene = scene;
  background = Color(0.0, 0.0, 0.0, 1.0);


  Vector w = _camera.look_at - _camera.location;
  Vector u;
  Vector v;
  w.norm();
  w = w * -1;
  _camera.up.cross(w, &u);
  w.cross(u, &v);

  MyMat m1 = MyMat(1, 0, 0, _camera.location.x(),
                   0, 1, 0, _camera.location.y(),
                   0, 0, 1, _camera.location.z(),
                   0, 0, 0, 1);

  //cout << "Camera Loc: " << endl << m1 << endl;

  MyMat m2 = MyMat(u.x(), v.x(), w.x(), 0,
                   u.y(), v.y(), w.y(), 0,
                   u.z(), v.z(), w.z(), 0,
                   0, 0, 0, 1);

  //cout << "UVW: " << endl << m2 << endl;

  _matrix = m1.multRight(m2);
  //cout << "END: " << endl << m3 << endl;
}

/*
 * Default Raycaster Constructor given the current scene with an overrided camera
 *----------------------------------------------------------------------------*/
Raycaster::Raycaster( Scene* scene, Camera cam )
{
  mCastMode = PERSPECTIVE;
  _camera = cam;
  Vector right = _camera.right;
  Vector up = _camera.up;
  double ratio =  right.norm() / up.norm();

  mLeft = -_camera.fov_ratio * ratio;
  mRight = _camera.fov_ratio * ratio;
  mTop = _camera.fov_ratio;
  mBottom = -_camera.fov_ratio;

  printf("left: %f, right: %f, up: %f, down: %f\n", mLeft,mRight,mTop,mBottom);

  mNear = 0.5;
  mScene = scene;
  background = Color(0.0, 0.0, 0.0, 1.0);


  Vector w = _camera.look_at - _camera.location;
  Vector u;
  Vector v;
  w.norm();
  w = w * -1;
  _camera.up.cross(w, &u);
  w.cross(u, &v);

  MyMat m1 = MyMat(1, 0, 0, _camera.location.x(),
                   0, 1, 0, _camera.location.y(),
                   0, 0, 1, _camera.location.z(),
                   0, 0, 0, 1);

  //cout << "Camera Loc: " << endl << m1 << endl;

  MyMat m2 = MyMat(u.x(), v.x(), w.x(), 0,
                   u.y(), v.y(), w.y(), 0,
                   u.z(), v.z(), w.z(), 0,
                   0, 0, 0, 1);

  //cout << "UVW: " << endl << m2 << endl;

  _matrix = m1.multRight(m2);
  //cout << "END: " << endl << m3 << endl;
}

/*
 * New Sum Lights for a given surface
 * ---------------------------------------------------------------------------*/

Color Raycaster::sumLights( Surface surface, Ray ray, int specular, int ambient, bool gather ) {
    Color result; // resulting color to be returned

    Vector D = ray.direction;
    Vector V = D * -1;

    // Invert backwards normals
    if(surface.objPtr->getType() == TRIANGLE && surface.n.dot(V) < 0) {
        surface.n = surface.n * -1;
    }

    result = Color(0);

    // Gather indirect light
    result = light::shadeIndirect(surface, gather, ambient);

    result = result + light::shadeDiffuse(V, surface, specular);
    
    return result;
}

/*
 * New Handle Intersect Code
 * ---------------------------------------------------------------------------*/
Color Raycaster::handleIntersect( Ray ray, int depth )
{
    Color color;

    Surface surface;

    if(depth <= 0) {

        return background;
    }

    // Check for intersect.  Throw away ray if no intersect
    if( !mScene->intersect(ray, &surface) ) {
        Color vol_color(0.);
        Color vol_transmittance(1.);
        //vol::integrate_volume( mScene->getVolumeBVH(), ray, surface.t, &vol_color);

        //vol_color = mVolumeIntegrator->Li( Ray(ray.start, ray.direction, 0.0, INFINITY), &vol_transmittance );

        return (background * vol_transmittance) + vol_color;
    }

    Vector D = ray.direction;
    Vector N = surface.n;
    Vector R = ray.direction.reflect(N);
    Vector P = surface.p;
    Ray ray_reflect = Ray(P, R);

    Color color_reflect;
    Color color_refract;

    if(surface.type == SolidSurface) {
        color = color + sumLights(surface, ray, 1, 1, true);

        int success;
        Vector Drefract;
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
        //vol::integrate_volume( mScene->getVolumeBVH(), ray, surface.t, &vol_color);

        //vol_color = mVolumeIntegrator->Li( Ray(ray.start, ray.direction, 0.0, surface.t), &vol_transmittance );

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
Color Raycaster::initialCast( Ray ray, int depth )
{
    if(depth <= 0)
        return background;

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
ray.start = ray.start + Vector(us, vs, 0);

ray.direction = Vector(0,0,-1);
ray.direction.norm();
ray.direction = ray.direction + Vector(us, vs, 0);


  ray.start = _matrix * Vector4(ray.start, 1);
  ray.direction = _matrix * Vector4(ray.direction, 0);
  ray.direction.norm();

  double t;

  if(config::render_target == TARGET_LIGHT_CACHE) {
    color = mScene->lightCache()->gather(ray, &t);
  } else {
    color = initialCast(ray, mDepth);
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

  double us = mLeft + ((mRight - mLeft)
        * (double)((double)(x + 0.5f) / width));
  double vs = mBottom + ((mTop - mBottom)
        * (double)((double)(y + 0.5f) / height));
  //cout << "US: " << us << "  VS: " << vs << endl;
  Ray ray = Ray();

ray.start.set(0, 0, 0);
ray.start = ray.start + Vector(us, vs, 0);

ray.direction = Vector(0,0,-1);
ray.direction.norm();
ray.direction = ray.direction + Vector(us, vs, 0);

  ray.start = _matrix * Vector4(ray.start, 1);
  ray.direction = _matrix * Vector4(ray.direction, 0);
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
  int rays = width * height;
  int print_percent = 1;
  int rays_done = 0;
  int percent_done = 0;
  //writer.fill( 0, 0, 0, 0 );
  Color color;
  double low = -1;
  double high = -1;
  int x, y;
  mDepth = depth;

  mVolumeIntegrator = new VolumeIntegrator(mScene);

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
        int width,
        int height,
        int step_x,
        int step_y,
        ImageWriter *writer)
{

  //writer.fill( 0, 0, 0, 0 );
  Color color;
  int x, y;
  
  mVolumeIntegrator = new VolumeIntegrator(mScene);

  Surface surf;
  Ray ray;

  mScene->initCache(Vector(-15, -15, -15), Vector(15, 15, 15));

  for( x = 0; x < width; x+=step_x ) {

    for( y = 0; y < height; y+=step_y ) {

      cam2World(x, y, width, height, &ray);
      
      color = 0;

      while(mScene->intersect(ray, &surf)) {

          color = sumLights(surf, ray, 0, 0, false);
          mScene->addSurfel(shared_ptr<Surfel>(new Surfel(ray(surf.t), surf.n, color, 0.15)));
          ray.start = ray(surf.t + 0.1);
          
      }

    }
  }

  printf("FINAL: %d\n", mScene->lightCache()->count());

  printf("Size: %d\n", mScene->lightCache()->size_of());

  return 0;
}
