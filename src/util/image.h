#ifndef _IMAGE_H_
#define _IMAGE_H_

class ImageWriter{
public:
  ImageWriter( int width, int height );
  int getWidth( void );
  int getHeight( void );
  void fillBox( int x1, int y1, int x2, int y2, int r, int g, int b, int a );
  void fill( int r, int g, int b, int a );
  void setPixel( int x, int y, int r, int g, int b );
  void setPixel( int x, int y, int r, int g, int b, int a );
  void modPixel( int x, int y, float m, float c );
  void write( char* filename );
  void stretch( float low, float high );
private:
  char* mData;
  int mWidth;
  int mHeight;
};

#endif
