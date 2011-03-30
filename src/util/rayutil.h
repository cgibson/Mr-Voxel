#ifndef _RAY_UTIL_H_
#define _RAY_UTIL_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../types.h"

#define PI 3.14159265

/*
 * Represents a ray travelling through 3d space
 *----------------------------------------------------------------------------*/
class Ray {
public:
  Ray( void ): mint(0), maxt(INFINITY) {};
  Ray( Vec3 s, Vec3 d, double t0 = 0, double t1 = INFINITY ):start(s), direction(d), mint(t0), maxt(t1) {};
  Vec3 start;
  Vec3 direction;
  
  double mint, maxt;
  

  Vec3 operator() ( double t ){ return start + (direction * t); }
};

/*
 * Represents a RGBColor value
 *----------------------------------------------------------------------------*/
class RGBColor {
public:
  RGBColor( void ):p_r(0),p_g(0),p_b(0),p_f(0.f){};
  RGBColor( double v ):p_r(v),p_g(v),p_b(v),p_f(0.f){};
  RGBColor( double r, double g, double b ):p_r(r),p_g(g),p_b(b),p_f(0.f){};
  RGBColor( double r, double g, double b, double f ):p_r(r),p_g(g),p_b(b),p_f(f){};

  char *str( void );

  int *rgbArray( void );
  void clamp( double lower, double upper );

  void set( double r, double g, double b );
  void set( double r, double g, double b, double f );

  double  r( void ) const { return p_r; }
  double  g( void ) const { return p_g; }
  double  b( void ) const { return p_b; }
  double  f( void ) const { return p_f; }
  
  void r( double r ) { p_r = r; }
  void g( double g ) { p_g = g; }
  void b( double b ) { p_b = b; }
  void f( double f ) { p_f = f; }

  double toTrans(){ return (p_r + p_g + p_b) / 3.0; }
  
  bool isBlack(){ return (p_r < 1e-5) && (p_g < 1e-5) && (p_b < 1e-5); }

  RGBColor operator+( const RGBColor& c )
    { return RGBColor( p_r + c.p_r, p_g + c.p_g, p_b + c.p_b, (p_f < c.p_f) ? p_f : c.p_f); }
  RGBColor operator-( const RGBColor& c )
    { return RGBColor( p_r - c.p_r, p_g - c.p_g, p_b - c.p_b, (p_f < c.p_f) ? p_f : c.p_f); }
  RGBColor operator*( const RGBColor& c )
    { return RGBColor( p_r * c.p_r, p_g * c.p_g, p_b * c.p_b, p_f * c.p_f); }
  RGBColor operator/( const RGBColor& c )
    { return RGBColor( p_r / c.p_r, p_g / c.p_g, p_b / c.p_b, p_f / c.p_f); }
  RGBColor operator*( const double f )
    { return RGBColor( p_r * f, p_g * f, p_b * f, p_f); }
  RGBColor operator/( const double f )
    { return RGBColor( p_r / f, p_g / f, p_b / f, p_f); }

  friend RGBColor Exp(const RGBColor &c) {
     return RGBColor(expf(c.r()), exp(c.g()), exp(c.b()));
  }
    

private:
  double p_r, p_g, p_b, p_f;

};

#endif
