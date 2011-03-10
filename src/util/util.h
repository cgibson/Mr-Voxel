#ifndef _UTIL_H_
#define _UTIL_H_

#include <math.h>
#include <iostream>

using namespace std;

int test_intersect_1d(double p, double d,
                      double min, double max,
                      double *result_near, double *result_far);

class Dimension {
public:
  Dimension(): m_x(0), m_y(0){}
  Dimension(int x, int y): m_x(x), m_y(y){}

  int x(){ return m_x;}
  int y(){ return m_y;}
  void setX(int x){ m_x = x;}
  void setY(int y){ m_y = y;}
private:
  double m_x, m_y;
};

/* A vector class for 3D Vectors */
class Vector{
public:
	Vector( void );           // default constructor
	Vector( Vector* );       //alternative constructors 
    Vector( const double x, const double y, const double z )
		 : p_x( x ), p_y( y ), p_z( z ) {}
	~Vector( void );          // *always* have a destructor

     /*assesors*/
     double  x( void ) const { return p_x; }
     double& x( void )       { return p_x; }
     double  y( void ) const { return p_y; }
     double& y( void )       { return p_y; }
     double  z( void ) const { return p_z; }
     double& z( void )       { return p_z; }

     double get( int i );
	 void set(double x, double y, double z);
	 void set(Vector& v);
	 
	 void x( double x ){ p_x = x;}
	 void y( double y ){ p_y = y;}
	 void z( double z ){ p_z = z;}

     /*operator overload for math simplicity */ 
     Vector operator=( const Vector& );

     Vector operator+( const Vector& v )
       { return Vector( p_x + v.p_x, p_y + v.p_y, p_z + v.p_z); }
     
	 Vector operator-( const Vector& v )
       { return Vector( p_x - v.p_x, p_y - v.p_y, p_z - v.p_z); }
     
	 double operator*( const Vector& v )
       { return ( p_x*v.p_x + p_y*v.p_y + p_z*v.p_z); }
	
	 Vector operator*(const double f) 
       { return Vector( p_x*f , p_y*f , p_z*f); }
	
	 Vector operator/(const double f) 
       { return Vector( p_x/f , p_y/f , p_z/f); }
    
    /*useful to be able to print out using stream operators*/
    friend ostream& operator<<( ostream&, const Vector& );
    friend istream& operator>>( istream&, Vector& );

	void cross(const Vector in, Vector *out);
	double dot(const Vector v2);
	double norm();
	double length();
    char* str();
    Vector reflect(Vector n);
    Vector refract(Vector n, double n1, double n2, int *success);

private:
     double p_x, p_y, p_z;
};

class Vector4{
public:
    Vector4( void );           // default constructor
    Vector4( Vector4* );
     Vector4( const double x, const double y, const double z , const double t)
         : p_x( x ), p_y( y ), p_z( z ), inter( t) {}
    ~Vector4( void );          // *always* have a destructor
    Vector4( const Vector v, const double t);

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
     friend ostream& operator<<( ostream&, const Vector4& );

     char* str();
private:
     double p_x, p_y, p_z, inter;
};

#endif
