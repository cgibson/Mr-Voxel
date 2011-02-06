#include "util.h"
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace std;


/*
 * Test intersect for one dimension.  Used for box intersect testing
 *----------------------------------------------------------------------------*/
int test_intersect_1d(double p, double d,
                      double min, double max,
                      double *result_near, double *result_far)
{
  double tmp;
  double tnear = -1000.0;
  double tfar = 1000.0;
  double t1 = (min - p) / d;
  double t2 = (max - p) / d;

  // if the component is zero
  if(d == 0)
  {
    return (p > min) && (p < max);
  }

  // switch if t1 > t2
  if(t1 > t2)
  {
    tmp = t1;
    t1 = t2;
    t2 = tmp;
  }

  if(t1 > tnear) tnear = t1;
  if(t2 < tfar) tfar = t2;

  *result_near = tnear;
  *result_far = tfar;
  return true;
}


/* Vector constructors */
/* default */
Vector::Vector( void) {
    p_x = p_y = p_z = 0;
}

char* Vector::str( void ) {
    char *buffer = (char*)calloc(300, sizeof(char));
    sprintf(buffer, "<%.2f, %.2f, %.2f>", p_x, p_y, p_z);
    return buffer;
}

Vector Vector::reflect(Vector n)
{
    n.norm();
    Vector d = Vector(p_x, p_y, p_z);
    d.norm();
    return d - (n * (2 *(n * d)));
}
/**/
Vector Vector::refract(Vector N, double n1, double n2, int *success)
{
    N.norm();
    Vector D = Vector(p_x, p_y, p_z);
    D.norm();
    //printf("N: %s\n", N.str());
    //printf("D: %s\n", D.str());
    double c1 = D * N;
    //printf("C1: %f\n", c1);
    double n = n1 / n2;
    //printf("n: (%f / %f) = %f\n", n1, n2, n);
    
    double end = 1 - (pow(n, 2) * (1 - pow(c1, 2)));
    //printf("end: %f\n", end);
    if(end < 0.0)
    {
      *success = 0;
      return Vector(0,0,0);
    }
    *success = 1;
    return ((D - (N * c1)) * n) - (N * sqrt(end));
}/**/
/*
Vector Vector::refract(Vector N, double n2, double n1, int *success)
{
    N.norm();
    *success = 1;
    Vector V = Vector(p_x, p_y, p_z);
    V.norm();
    double n = n1 / n2;
    
    double c1 = -(N * V);
    
    double c2 = 1 - (pow(n, 2) * (1 - pow(c1, 2)));
    if(c2 < 0.0)
    {
      *success = 0;
      return Vector(0,0,0);
    }
    Vector Rr = (V * n) + (N * (n * c1 - c2));
    
    return Rr;
}*/

Vector::Vector( Vector *v) {
	p_x = v->p_x; 
  	p_y= v->p_y; 
	p_z= v->p_z;
}

void Vector::set(double x, double y, double z) {
	this->p_x = x; 
  	this->p_y = y; 
	this->p_z = z;
}

void Vector::set(Vector& v) {
	this->p_x = v.p_x; 
  	this->p_y = v.p_y; 
	this->p_z = v.p_z;
}

Vector Vector::operator=( const Vector& v1) {
	this->p_x = v1.x();
	this->p_y = v1.y();
	this->p_z = v1.z();
	return(*this );
}

void Vector::cross(const Vector in, Vector *out) {
	out->p_x = (p_y*in.p_z - p_z*in.p_y); 
  	out->p_y= -(p_x*in.p_z - p_z*in.p_x); 
	out->p_z= (p_x*in.p_y - p_y*in.p_x);
}

double Vector::dot(const Vector v2) {
	return(p_x*v2.p_x + p_y*v2.p_y + p_z*v2.p_z);
}

double Vector::norm() {
	double leng = sqrt(p_x*p_x + p_y*p_y + p_z*p_z);
	p_x = p_x/leng; 
  	p_y= p_y/leng; 
	p_z= p_z/leng;
	return(leng);
}

double Vector::length() {
	double leng = sqrt(p_x*p_x + p_y*p_y + p_z*p_z);
	return leng;
}

ostream& operator<<( ostream& os, const Vector& v )
{
    cout << "vector x: " << v.p_x ;
    cout << " y: " << v.p_y ;
    cout << " z: " << v.p_z << endl;
    return(os);
}

/* destructor */
Vector::~Vector() {}

/* 4D Vector constructors */
/* default */
Vector4::Vector4( void ) {
    p_x = p_y = p_z = inter = 0;
    }    

Vector4::Vector4( Vector4* v) {
    p_x = v->p_x;
    p_y= v->p_y;
    p_z= v->p_z;
    inter= v->inter;
}

Vector4::Vector4( Vector v, double t) {
    p_x = v.x();
    p_y= v.y();
    p_z= v.z();
    inter= t;
}

char* Vector4::str( void ) {
  char *buffer = (char*)calloc(300, sizeof(char));
  sprintf(buffer, "<%.2f, %.2f, %.2f, %.2f>", p_x, p_y, p_z, inter);
  return buffer;
}

void Vector4::set(double x, double y, double z, double t) {
    p_x = x;
    p_y= y;
    p_z= z;
	inter = t;
}

ostream& operator<<( ostream& os, const Vector4& v )
{
    cout << "vector x: " << v.p_x ;
    cout << " y: " << v.p_y ;
    cout << " z: " << v.p_z ;
    cout << " t: " << v.inter << endl;
    return(os);
}

/* destructor */
Vector4::~Vector4( void ) {}
