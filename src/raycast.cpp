#include <boost/thread/thread.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "raycast.h"

namespace this_thread = boost::this_thread;

#define DEBUG 1
//#define false 0
//#define true 1

double max(double a, double b)
{
  return (a > b) ? a : b;
}

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
 * Recursively test all objects within the given bounding box hierarchy
 *----------------------------------------------------------------------------*/
int Raycaster::recurse_intersect(Ray ray, Surface *surface, SceneObject *parent)
{
  if(parent == NULL)
    return false;

  Ray tmp_ray;
  Surface col_l, col_r;
  double t = 0;
  Vector n;
  int hit;
  int hit_l = false;
  int hit_r = false;
  double epsilon = 0.001f;
  TYPE type = parent->getType();
  BBNode *node;

  // if the object is a node
  if(type == NODE) {
    // check to make sure it hits
    hit = parent->test_intersect(ray, &t, &n);
    
    // return false if no hit, no need to delve deeper
    if(!hit)
      return false;

    node = (BBNode*)parent;
    if(node == NULL) {
      printf("ERROR: cannot use this node!\n");
      exit(-1);
    }
    else {
      // recurse into the left child
      if(node->has_left())
        hit_l = recurse_intersect(ray, &col_l, node->child_l);
      
      // recurse into the right child
      if(node->has_right())
        hit_r = recurse_intersect(ray, &col_r, node->child_r);
    }
    
    // if both children return a hit
    if(hit_l && hit_r) {
      // compare the hit values and return the closest
      if(col_l.t + epsilon < col_r.t) {
        *surface = col_l;
      }
      else {
        *surface = col_r;
      }
    // if the left child is hit
    }
    else if(hit_l) {
      *surface = col_l;
    // if the right child is hit
    }
    else if(hit_r) {
      *surface = col_r;
    }
    else {
      // no one was hit
      return false;
    }
    return true;
  
  // otherwise, the current node is a scene object
  }
  else if(type == VOLUME) {
	  printf("ERROR: Intersecting volume is deprecated.");
	  exit(-1);
    // create a temporary ray to avoid clipping issues
    GeomObj *obj = (GeomObj*)parent;
    tmp_ray.start = obj->matrix * Vector4(ray.start, 1);
    tmp_ray.direction = obj->matrix * Vector4(ray.direction, 0);

    // test to see if the volume is hit
    hit = parent->test_intersect(tmp_ray, &t, &n);
    
    if(hit && (t > epsilon)) {
      surface->t = t;
      surface->n =  ((GeomObj*)parent)->matrix.trans() * Vector4(n,1);
      surface->n.norm();
      surface->p = ray.start + (ray.direction * t);
      surface->finish = ((GeomObj*)parent)->finish;
      surface->color = ((GeomObj*)parent)->pigment.rgbf;
      surface->type = VolumeSurface;
      surface->objPtr = obj;
      return true;
    }
    return false;
  }
  else {
    // create a temporary ray to avoid clipping issues
    GeomObj *obj = (GeomObj*)parent;
    tmp_ray.start = obj->matrix * Vector4(ray.start, 1);
    tmp_ray.direction = obj->matrix * Vector4(ray.direction, 0);
    
    // test the scene object's intersect
    hit = parent->test_intersect(tmp_ray, &t, &n);
    
    // if the value is closer than the 'closest'
    if(hit && (t > epsilon)) {

      surface->t = t;
      surface->n = ((GeomObj*)parent)->matrix.trans() * Vector4(n,1);
      surface->n.norm();
      surface->p = ray.start + (ray.direction * t);
      surface->finish = ((GeomObj*)parent)->finish;
      surface->color = ((GeomObj*)parent)->pigment.rgbf;
      surface->type = SolidSurface;
      surface->objPtr = obj;
      return true;
    }
    return false;
  }
}

/*
 * Intersect given a ray
 *----------------------------------------------------------------------------*/
int Raycaster::intersect(Ray ray, Surface *surface)
{
	SceneObject *root = mScene->getObjectBVH();
	GeomObj **planes = mScene->getPlanes();
	SceneObject *cur;
	int plane_count = mScene->getPlaneCount();
	double epsilon = 0.001f;

	Surface closest, tmp_surf;
	double closest_dist;

	int hit;
	int has_hit = false;

	ray.start = ray.start + ray.direction * epsilon * 10;

	int i;
	// for every plane
	for(i = 0; i < plane_count; i++) {

		cur = planes[i];

		// check to see if the plane hits
		hit = recurse_intersect(ray, &tmp_surf, cur);
		if(hit) {
			// if the current intersection is the closest (or first)
			if((!closest.isHit()) ||
			(closest.t + epsilon > tmp_surf.t)) {
				// set the intersect information as the 'closest'
				closest = tmp_surf;
				has_hit = true;
			}
		}
	}
    
	// recurse through the root
	hit = recurse_intersect(ray, &tmp_surf, root);
	if(hit) {
	  // if the current intersection is the closest (or first)
	  if((!closest.isHit()) ||
		 (closest.t + epsilon > tmp_surf.t)) {
	// set the intersect information as the 'closest'
		closest = tmp_surf;
		has_hit = true;
	  }
	}

  
  *surface = closest;
  return has_hit;
}


/*
 * New Sum Lights for a given surface
 * ---------------------------------------------------------------------------*/

Color Raycaster::sumLights( Surface surface, Ray ray, int specular, int ambient, bool gather ) {
    Color Lcolor; // temporary light color
    Color result; // resulting color to be returned

    Vector D = ray.direction;
    Vector V = D * -1;

    Ray shadow_ray;

    result = Color(0);

    // Initialize result to the ambient value


    if(gather) {
        double tt;
        Color amb;
        Vector rnd;
        rnd = Vector((rand() / (float)RAND_MAX) - 0.5f,(rand() / (float)RAND_MAX) - 0.5f,(rand() / (float)RAND_MAX) - 0.5f);
        Color ambs[5];
        int count = 50;
        double rndn;
        for(int i = 0; i < count; i++) {
            while(rnd.dot(surface.n) < 0){
                rnd = Vector((rand() / (float)RAND_MAX) - 0.5f,(rand() / (float)RAND_MAX) - 0.5f,(rand() / (float)RAND_MAX) - 0.5f);
                rnd.norm();
                //printf("%s -- %s -- %f\n", rnd.str(), surface.n.str(), rnd.dot(surface.n));
            }
            rndn = (rnd * surface.n);
            rndn = (rndn > 0.1 ? rndn : 0.1);
            amb = amb + mScene->lightCache()->gather(Ray(ray(surface.t) + (rnd * 0.3), rnd), &tt) * rndn;
            rnd = Vector((rand() / (float)RAND_MAX) - 0.5f,(rand() / (float)RAND_MAX) - 0.5f,(rand() / (float)RAND_MAX) - 0.5f);
        }

        //for(int i = 0; i < 5)



        result = result + amb * surface.finish.ambient * surface.color * (20. / count);

    } else{
        if(ambient)
            result = result + surface.color * surface.finish.ambient;
    }
    // Grab light sources
    LightSource** lights = mScene->getLightSources();

    // Iterate through each light
    for(int i = 0; i < mScene->getLightSourceCount(); i++) {

        // Grab temporary light pointer
        LightSource *light = lights[i];
        // Grab light color
        Lcolor = light->color;
        // Light position, relative to surface
        Vector L = light->position - surface.p;
        double l_dist = L.norm();
        Vector H = V + L;
        H.norm();

        // Generate shadow ray
        shadow_ray = Ray(surface.p, L);

        // invert shininess so it makes sense
        double shininess = 1 / surface.finish.roughness;

        Surface surface2;

        // Beginning transmittance is 1
        Color Tr(1.);

        // If an object is in the way
        if(intersect(shadow_ray, &surface2) && (surface2.t <= l_dist)) {
        	Tr = Tr * 0.0;

        // Otherwise, integrate through all volumes
        }else{
        	Tr = 1.0;//Tr * mVolumeIntegrator->Transmittance( shadow_ray );
        }

		result = result +
		((surface.color * Lcolor * Tr) * surface.finish.diffuse * max(0, surface.n * L));
                
                if(specular && !Tr.isBlack())
                    result = result + ((Lcolor) * surface.finish.specular * Tr * pow(max(0, H * surface.n), shininess));
    }
    
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
    if( !intersect(ray, &surface) ) {
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

  //cout << "RAY: " << ray.start.str() << " " << ray.direction.str() << endl;

  double t;
  //color = mScene->lightCache()->gather(ray, &t);

  color = initialCast(ray, mDepth);


  color.clamp(0, 1);
  return color;
}

int Raycaster::iterate( Ray *ray, Surface *surface) {
    if(intersect(*ray, surface)) {
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

      while(intersect(ray, &surf)) {

          color = sumLights(surf, ray, 0, 0, false);
          mScene->addSurfel(shared_ptr<Surfel>(new Surfel(ray(surf.t), surf.n, color, 0.225)));
          ray.start = ray(surf.t + 0.1);
          
      }

      //printf("YAY: %s\n", ray(surf.t).str());

      
      

      /*for(int i = 0; i < step_x; i++){
          for(int j = 0; j < step_y; j++) {
              writer->setPixel(x+i, y+j,
                      color.r() * 255,
                      color.g() * 255,
                      color.b() * 255,
                      (color.f() == 1.0) ? 255 : 255 );
              }
          }*/


    }
  }

  printf("FINAL: %d\n", mScene->lightCache()->count());

  printf("Size: %d\n", mScene->lightCache()->size_of());

  return 0;
}
