#ifndef _UTIL_H_
#define _UTIL_H_

#include <math.h>
#include <iostream>

using namespace std;

int test_intersect_1d(double p, double d,
                      double min, double max,
                      double *result_near, double *result_far);

/* A Vector3D class for 3D Vector3Ds */
class Vector3D{
public:
    Vector3D( void );           // default constructor
    Vector3D( Vector3D* );       //alternative constructors
    Vector3D( const double x, const double y, const double z )
		 : p_x( x ), p_y( y ), p_z( z ) {}
	~Vector3D( void );          // *always* have a destructor

     /*assesors*/
     inline double  x( void ) const { return p_x; }
     inline double& x( void )       { return p_x; }
     inline double  y( void ) const { return p_y; }
     inline double& y( void )       { return p_y; }
     inline double  z( void ) const { return p_z; }
     inline double& z( void )       { return p_z; }

     double get( int i ) const;
     
    inline void set(double x, double y, double z) {
	this->p_x = x;
  	this->p_y = y;
	this->p_z = z;
    }

    inline void set(Vector3D& v) {
	this->p_x = v.p_x;
  	this->p_y = v.p_y;
	this->p_z = v.p_z;
    }

     inline void x( double x ){ p_x = x;}
     inline void y( double y ){ p_y = y;}
     inline void z( double z ){ p_z = z;}

     inline bool isNormalized(){ return length() > .995 && length() < 1.005f; }

     /*operator overload for math simplicity */ 
     inline Vector3D operator=( const Vector3D& v) {
	this->p_x = v.x();
	this->p_y = v.y();
	this->p_z = v.z();
	return(*this );
    }

     inline Vector3D operator+( const Vector3D& v ) const
       { return Vector3D( p_x + v.p_x, p_y + v.p_y, p_z + v.p_z); }
     
     inline Vector3D operator-( const Vector3D& v ) const
       { return Vector3D( p_x - v.p_x, p_y - v.p_y, p_z - v.p_z); }

     inline double operator*( const Vector3D& v ) const
       { return ( p_x*v.p_x + p_y*v.p_y + p_z*v.p_z); }
	
     inline Vector3D operator*(const double f) const
       { return Vector3D( p_x*f , p_y*f , p_z*f); }
	
     inline Vector3D operator/(const double f) const
       { return Vector3D( p_x/f , p_y/f , p_z/f); }
    
    /*useful to be able to print out using stream operators*/
    friend ostream& operator<<( ostream&, const Vector3D& );
    friend istream& operator>>( istream&, Vector3D& );

    void cross(const Vector3D in, Vector3D *out);

    inline double dot(const Vector3D v2){
	return(p_x*v2.p_x + p_y*v2.p_y + p_z*v2.p_z);
    };
    
    double norm();
    
    double length() const {
	return sqrt(p_x*p_x + p_y*p_y + p_z*p_z);
    }

    char* str() const;
    Vector3D reflect(Vector3D n);
    Vector3D refract(Vector3D n, double n1, double n2, int *success);

private:
     double p_x, p_y, p_z;
};

class Vector4D{
public:
    Vector4D( void );           // default constructor
    Vector4D( Vector4D* );
     Vector4D( const double x, const double y, const double z , const double t)
         : p_x( x ), p_y( y ), p_z( z ), inter( t) {}
    ~Vector4D( void );          // *always* have a destructor
    Vector4D( const Vector3D v, const double t);

     /*assesors*/
     double  x( void ) const { return p_x; }
     double& x( void )       { return p_x; }
     double  y( void ) const { return p_y; }
     double& y( void )       { return p_y; }
     double  z( void ) const { return p_z; }
     double& z( void )       { return p_z; }
     double  t( void ) const { return inter; }
     double& t( void )       { return inter; }

     void set(double x, double y, double z, double t);
    
    /*useful to be able to print out using stream operators*/
     friend ostream& operator<<( ostream&, const Vector4D& );

     char* str();
private:
     double p_x, p_y, p_z, inter;
};

#endif
