#include "rayutil.h"

/*
 * RGBColor to string function
 *----------------------------------------------------------------------------*/
char* RGBColor::str( void ) {
  char *buffer = (char*)calloc(300, sizeof(char));
  sprintf(buffer, "<%.2f, %.2f, %.2f, %.2f>", p_r, p_g, p_b, p_f);
  return buffer;
}

/*
 * Clamp all values to upper/lower doubleing point values
 *----------------------------------------------------------------------------*/
void RGBColor::clamp( double lower, double upper )
{
    assert(p_r >= -0.001);
    assert(p_g >= -0.001);
    assert(p_b >= -0.001);
  if(p_r < lower) p_r = lower;
  else if(p_r > upper) p_r = upper;

  if(p_g < lower) p_g = lower;
  else if(p_g > upper) p_g = upper;

  if(p_b < lower) p_b = lower;
  else if(p_b > upper) p_b = upper;
}

/*
 * Generates integer array of RGB values (all between 0-255)
 *----------------------------------------------------------------------------*/
int *RGBColor::rgbArray( void )
{
  int *array = (int*)malloc(sizeof(int) * 3);
  array[0] = (int)(p_r * 255);
  array[1] = (int)(p_g * 255);
  array[2] = (int)(p_b * 255);

  return array;
}
