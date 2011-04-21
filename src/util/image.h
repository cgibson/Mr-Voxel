#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "../types.h"
#include <string>

using std::string;

class ImageWriter{
public:
  ImageWriter( Dimension size );
  int getWidth( void );
  int getHeight( void );
  //void fillBox( int x1, int y1, int x2, int y2, int r, int g, int b, int a );
  //void fill( int r, int g, int b, int a );
  //void setPixel( int x, int y, int r, int g, int b );
  void setPixel( int x, int y, double r, double g, double b, double a );
  //void modPixel( int x, int y, float m, float c );
  void write( string filename );
  //void stretch( float low, float high );
private:
  double* mData;
  int mWidth;
  int mHeight;
};

#endif
