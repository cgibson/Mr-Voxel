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
    Vector3D( const float x, const float y, const float z )
		 : p_x( x ), p_y( y ), p_z( z ) {}
	~Vector3D( void );          // *always* have a destructor

     /*assesors*/
     inline float  x( void ) const { return p_x; }
     inline float& x( void )       { return p_x; }
     inline float  y( void ) const { return p_y; }
     inline float& y( void )       { return p_y; }
     inline float  z( void ) const { return p_z; }
     inline float& z( void )       { return p_z; }

     float get( int i );
     void set(float x, float y, float z);
     void set(Vector3D& v);

     inline void x( float x ){ p_x = x;}
     inline void y( float y ){ p_y = y;}
     inline void z( float z ){ p_z = z;}

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

     inline Vector3D operator+( const Vector3D& v )
       { return Vector3D( p_x + v.p_x, p_y + v.p_y, p_z + v.p_z); }
     
     inline Vector3D operator-( const Vector3D& v )
       { return Vector3D( p_x - v.p_x, p_y - v.p_y, p_z - v.p_z); }

     inline float operator*( const Vector3D& v )
       { return ( p_x*v.p_x + p_y*v.p_y + p_z*v.p_z); }
	
     inline Vector3D operator*(const float f)
       { return Vector3D( p_x*f , p_y*f , p_z*f); }
	
     inline Vector3D operator/(const float f)
       { return Vector3D( p_x/f , p_y/f , p_z/f); }
    
    /*useful to be able to print out using stream operators*/
    friend ostream& operator<<( ostream&, const Vector3D& );
    friend istream& operator>>( istream&, Vector3D& );

    void cross(const Vector3D in, Vector3D *out);

    inline float dot(const Vector3D v2){
	return(p_x*v2.p_x + p_y*v2.p_y + p_z*v2.p_z);
    };
    
    float norm();
    float length();
    char* str();
    Vector3D reflect(Vector3D n);
    Vector3D refract(Vector3D n, float n1, float n2, int *success);

private:
     float p_x, p_y, p_z;
};

class Vector4D{
public:
    Vector4D( void );           // default constructor
    Vector4D( Vector4D* );
     Vector4D( const float x, const float y, const float z , const float t)
         : p_x( x ), p_y( y ), p_z( z ), inter( t) {}
    ~Vector4D( void );          // *always* have a destructor
    Vector4D( const Vector3D v, const float t);

     /*assesors*/
     float  x( void ) const { return p_x; }
     float& x( void )       { return p_x; }
     float  y( void ) const { return p_y; }
     float& y( void )       { return p_y; }
     float  z( void ) const { return p_z; }
     float& z( void )       { return p_z; }
     float  t( void ) const { return inter; }
     float& t( void )       { return inter; }

     void set(float x, float y, float z, float t);
    
    /*useful to be able to print out using stream operators*/
     friend ostream& operator<<( ostream&, const Vector4D& );

     char* str();
private:
     float p_x, p_y, p_z, inter;
};

#endif
