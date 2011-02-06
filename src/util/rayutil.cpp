#include "rayutil.h"

/*
 * Color to string function
 *----------------------------------------------------------------------------*/
char* Color::str( void ) {
  char *buffer = (char*)calloc(300, sizeof(char));
  sprintf(buffer, "<%.2f, %.2f, %.2f, %.2f>", p_r, p_g, p_b, p_f);
  return buffer;
}

/*
 * Clamp all values to upper/lower doubleing point values
 *----------------------------------------------------------------------------*/
void Color::clamp( double lower, double upper )
{
  if(p_r < 0.0) p_r = 0.0;
  else if(p_r > 1.0) p_r = 1.0;

  if(p_g < 0.0) p_g = 0.0;
  else if(p_g > 1.0) p_g = 1.0;

  if(p_b < 0.0) p_b = 0.0;
  else if(p_b > 1.0) p_b = 1.0;
}

/*
 * Generates integer array of RGB values (all between 0-255)
 *----------------------------------------------------------------------------*/
int *Color::rgbArray( void )
{
  int *array = (int*)malloc(sizeof(int) * 3);
  array[0] = (int)(p_r * 255);
  array[1] = (int)(p_g * 255);
  array[2] = (int)(p_b * 255);

  return array;
}
