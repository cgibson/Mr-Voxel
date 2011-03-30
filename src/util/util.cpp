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
  float tmp;
  float tnear = -1000.0;
  float tfar = 1000.0;
  float t1 = (min - p) / d;
  float t2 = (max - p) / d;

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


/* Vector3D constructors */
/* default */
Vector3D::Vector3D( void) {
    p_x = p_y = p_z = 0;
}

float
Vector3D::get( int i ) {
    switch(i) {
        case 0: return p_x; break;
        case 1: return p_y; break;
        case 2: return p_z; break;
    }
}

char* Vector3D::str( void ) {
    char *buffer = (char*)calloc(300, sizeof(char));
    sprintf(buffer, "<%.2f, %.2f, %.2f>", p_x, p_y, p_z);
    return buffer;
}

Vector3D Vector3D::reflect(Vector3D n)
{
    n.norm();
    Vector3D d = Vector3D(p_x, p_y, p_z);
    d.norm();
    return d - (n * (2 *(n * d)));
}
/**/
Vector3D Vector3D::refract(Vector3D N, float n1, float n2, int *success)
{
    N.norm();
    Vector3D D = Vector3D(p_x, p_y, p_z);
    D.norm();
    //printf("N: %s\n", N.str());
    //printf("D: %s\n", D.str());
    float c1 = D * N;
    //printf("C1: %f\n", c1);
    float n = n1 / n2;
    //printf("n: (%f / %f) = %f\n", n1, n2, n);
    
    float end = 1 - (pow(n, 2) * (1 - pow(c1, 2)));
    //printf("end: %f\n", end);
    if(end < 0.0)
    {
      *success = 0;
      return Vector3D(0,0,0);
    }
    *success = 1;
    return ((D - (N * c1)) * n) - (N * sqrt(end));
}/**/
/*
Vector3D Vector3D::refract(Vector3D N, float n2, float n1, int *success)
{
    N.norm();
    *success = 1;
    Vector3D V = Vector3D(p_x, p_y, p_z);
    V.norm();
    float n = n1 / n2;
    
    float c1 = -(N * V);
    
    float c2 = 1 - (pow(n, 2) * (1 - pow(c1, 2)));
    if(c2 < 0.0)
    {
      *success = 0;
      return Vector3D(0,0,0);
    }
    Vector3D Rr = (V * n) + (N * (n * c1 - c2));
    
    return Rr;
}*/

Vector3D::Vector3D( Vector3D *v) {
	p_x = v->p_x; 
  	p_y= v->p_y; 
	p_z= v->p_z;
}

void Vector3D::set(float x, float y, float z) {
	this->p_x = x; 
  	this->p_y = y; 
	this->p_z = z;
}

void Vector3D::set(Vector3D& v) {
	this->p_x = v.p_x; 
  	this->p_y = v.p_y; 
	this->p_z = v.p_z;
}

Vector3D Vector3D::operator=( const Vector3D& v1) {
	this->p_x = v1.x();
	this->p_y = v1.y();
	this->p_z = v1.z();
	return(*this );
}

void Vector3D::cross(const Vector3D in, Vector3D *out) {
	out->p_x = (p_y*in.p_z - p_z*in.p_y); 
  	out->p_y= -(p_x*in.p_z - p_z*in.p_x); 
	out->p_z= (p_x*in.p_y - p_y*in.p_x);
}

float Vector3D::dot(const Vector3D v2) {
	return(p_x*v2.p_x + p_y*v2.p_y + p_z*v2.p_z);
}

float Vector3D::norm() {
	float leng = sqrt(p_x*p_x + p_y*p_y + p_z*p_z);
	p_x = p_x/leng; 
  	p_y= p_y/leng; 
	p_z= p_z/leng;
	return(leng);
}

float Vector3D::length() {
	float leng = sqrt(p_x*p_x + p_y*p_y + p_z*p_z);
	return leng;
}

ostream& operator<<( ostream& os, const Vector3D& v )
{
    cout << "Vector3D x: " << v.p_x ;
    cout << " y: " << v.p_y ;
    cout << " z: " << v.p_z << endl;
    return(os);
}

/* destructor */
Vector3D::~Vector3D() {}

/* 4D Vector constructors */
/* default */
Vector4D::Vector4D( void ) {
    p_x = p_y = p_z = inter = 0;
    }    

Vector4D::Vector4D( Vector4D* v) {
    p_x = v->p_x;
    p_y= v->p_y;
    p_z= v->p_z;
    inter= v->inter;
}

Vector4D::Vector4D( Vector3D v, float t) {
    p_x = v.x();
    p_y= v.y();
    p_z= v.z();
    inter= t;
}

char* Vector4D::str( void ) {
  char *buffer = (char*)calloc(300, sizeof(char));
  sprintf(buffer, "<%.2f, %.2f, %.2f, %.2f>", p_x, p_y, p_z, inter);
  return buffer;
}

void Vector4D::set(float x, float y, float z, float t) {
    p_x = x;
    p_y= y;
    p_z= z;
	inter = t;
}

ostream& operator<<( ostream& os, const Vector4D& v )
{
    cout << "vector x: " << v.p_x ;
    cout << " y: " << v.p_y ;
    cout << " z: " << v.p_z ;
    cout << " t: " << v.inter << endl;
    return(os);
}

/* destructor */
Vector4D::~Vector4D( void ) {}
