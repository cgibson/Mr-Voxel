#include "image.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Base ImageWriter constructor
 *----------------------------------------------------------------------------*/
ImageWriter::ImageWriter( Dimension size )
{
  mWidth = size.width;
  mHeight = size.height;
  // allocate all image data
  mData = (double*)calloc(size.width * size.height * 4, sizeof(double));
}

/*
 * Returns the image's width
 *----------------------------------------------------------------------------*/
int ImageWriter::getWidth( void )
{
  return mWidth;
}

/*
 * Returns the image's height
 *----------------------------------------------------------------------------*/
int ImageWriter::getHeight( void )
{
  return mHeight;
}

/*
 * Fills a specified box in the image data with a given color
 *----------------------------------------------------------------------------*/
/*
void ImageWriter::fillBox(int x1, int y1, int x2, int y2, int r, int g, int b, int a )
{
  int x, y;
    for( x = x1; x < x2; x++ )
    {
      for( y = y1; y < y2; y++ )
      {
        setPixel(x, y, r, g, b, a);
      }
    }
}
*/

/*
 * Stretches so all values based on given low and high floating point numbers
 *----------------------------------------------------------------------------*/
/*
void ImageWriter::stretch( float low, float high )
{
  float d = high - low;
  float m = 1 / d;
  float c = low * 255;
  
  printf("high: %f, low %f, m= %f c= %f\n", high, low, m, c);
  int x, y;
    for( x = 0; x < mWidth; x++ )
    {
      for( y = 0; y < mHeight; y++ )
      {
        modPixel(x, y, m, c);
      }
    }
}
*/

/*
 * Fills the entire screen with a specified color
 *----------------------------------------------------------------------------*/
/*
void ImageWriter::fill( int r, int g, int b, int a )
{
  fillBox( 0, 0, mWidth, mHeight, (char)r, (char)g, (char)b, (char)a );
}
*/

/*
 * Set a pixel in the image data
 *----------------------------------------------------------------------------*/
void ImageWriter::setPixel( int x, int y, double r, double g, double b, double a )
{
  // offset represents the 2d location in a single dimension array
	int offset = ((y * mWidth) + x) * 4;

  // data in TGA file is represented by BGR rather than RGB
  mData[offset + 3] = 1.0;//a;
  mData[offset + 2] = r;
  mData[offset + 1] = g;
  mData[offset + 0] = b;
}

/*
 * Modifies pixel given multiply and translate values
 *----------------------------------------------------------------------------*/
/*
void ImageWriter::modPixel( int x, int y, float m, float c )
{
  // offset represents the 2d location in a single dimension array
  int offset = ((y * mWidth) + x) * 4;

  // data in TGA file is represented by BGR rather than RGB
  mData[offset + 2] = (double)((float)mData[offset + 2] * m - c);
  mData[offset + 1] = (double)((float)mData[offset + 1] * m - c);
  mData[offset + 0] = (double)((float)mData[offset + 0] * m - c);
}
*/

/*
 * Write the given image data to a given file name
 *----------------------------------------------------------------------------*/
void ImageWriter::write( string filename )
{
  FILE *fp;
  int offset;

  // attempt to open the file
  fp = fopen(filename.c_str(), "w");

  // if the file does not example
  if(fp == NULL) {
      printf("Error: failed to create image %s\n", filename.c_str());
      exit(-1);
  }

  // header information
  putc(0,fp);
  putc(0,fp);
  putc(2,fp);
  putc(0,fp); putc(0,fp);
  putc(0,fp); putc(0,fp);
  putc(0,fp);
  putc(0,fp); putc(0,fp);
  putc(0,fp); putc(0,fp);
  putc((mWidth & 0x00FF),fp);
  putc((mWidth & 0xFF00) / 256,fp);
  putc((mHeight & 0x00FF),fp);
  putc((mHeight & 0xFF00) / 256,fp);
  putc(32,fp);
  putc(0,fp);

  char *chData = (char*)malloc(mWidth * mHeight * 4 * sizeof(char));

  for(int i = 0; i < mWidth; i++ )
  {
    for(int j = 0; j < mHeight; j++)
    {
      int offset = ((j * mWidth) + i) * 4;
      chData[offset + 3] = (char)(pow(mData[offset + 3], 1.0 / sys::config::gamma_correction) * 255);
      chData[offset + 2] = (char)(pow(mData[offset + 2], 1.0 / sys::config::gamma_correction) * 255);
      chData[offset + 1] = (char)(pow(mData[offset + 1], 1.0 / sys::config::gamma_correction) * 255);
      chData[offset + 0] = (char)(pow(mData[offset + 0], 1.0 / sys::config::gamma_correction) * 255);

    }
  }

  // write data as a large chunk
  fwrite(chData, sizeof(char), (mWidth * mHeight * 4), fp);

  free(chData);

  // close the file pointer
  fclose(fp);
}
