#include "modifier.h"

/*
 * Default Modifier constructor
 *----------------------------------------------------------------------------*/
Modifier::Modifier( void )
{

}

/*
 * Return matrix for default modifier (base matrix)
 *----------------------------------------------------------------------------*/
Matrix Modifier::matrix()
{
  return Matrix();
}

/*
 * Default modfier string
 *----------------------------------------------------------------------------*/
char* Modifier::str( void )
{
  char* buffer = (char*)calloc(11, sizeof(char));
  strcpy(buffer, "[MODIFIER]");
  return buffer;
}

/*
 * Default finish constructor
 *----------------------------------------------------------------------------*/
Finish::Finish( void )
{
  ambient = 1.0;
  diffuse = 0.6;
  specular = 0.0;
  roughness = 0.5;
  reflection = 0.0;
  refraction = 0.0;
  ior = 1.0;
}

/*
 * Finish to string function
 *----------------------------------------------------------------------------*/
char* Finish::str( void )
{
  char *buffer = (char*)calloc(300, sizeof(char));
  sprintf(buffer, "[%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, %.2f]", 
          ambient, diffuse, specular, roughness, reflection, 
          refraction, ior);
  return buffer;
}


/*
 * Default Pigment constructor
 *----------------------------------------------------------------------------*/
Pigment::Pigment( void )
{

}

/*
 * Pigment to string function
 *----------------------------------------------------------------------------*/
char* Pigment::str( void )
{
  char *buffer = (char*)calloc(300, sizeof(char));
  sprintf(buffer, "[PIGMENT] %s", rgbf.str());
  return buffer;
}

/*
 * Translation constructor
 *----------------------------------------------------------------------------*/
Translation::Translation( void )
{

}


/*
 * Return the matrix for the translation modifier
 *----------------------------------------------------------------------------*/
Matrix Translation::matrix()
{
  return Matrix(1, 0, 0, translation.x(),
               0, 1, 0, translation.y(),
               0, 0, 1, translation.z(),
               0, 0, 0, 1);
}

/*
 * Translation to string function
 *----------------------------------------------------------------------------*/
char* Translation::str( void )
{
  char *buffer = (char*)calloc(300, sizeof(char));
  sprintf(buffer, "[TRANSLATION] %s", translation.str());
  return buffer;
}

/*
 * Default rotation constructor
 *----------------------------------------------------------------------------*/
Rotation::Rotation( void )
{

}

/*
 * Return the rotation matrix
 *----------------------------------------------------------------------------*/
Matrix Rotation::matrix()
{
  double theta;
  if(rotation.z() != 0)
  {
    theta = (rotation.z() / 360.0) * 2 * PI;
    return Matrix(cos(theta), -sin(theta), 0, 0,
                 sin(theta), cos(theta), 0, 0,
                 0, 0, 1, 0,
                 0, 0, 0, 1);
  }else if(rotation.y() != 0)
  {
    theta = (rotation.y() / 360.0) * 2 * PI;
    return Matrix(cos(theta), 0, sin(theta), 0,
                 0, 1, 0, 0,
                 -sin(theta), 0, cos(theta), 0,
                 0, 0, 0, 1);
  }else{
    theta = (rotation.x() / 360.0) * 2 * PI;
    return Matrix(1, 0, 0, 0,
                 0, cos(theta), -sin(theta), 0,
                 0, sin(theta), cos(theta), 0,
                 0, 0, 0, 1);
  }
}

/*
 * Rotation to string function
 *----------------------------------------------------------------------------*/
char* Rotation::str( void )
{
  char *buffer = (char*)calloc(300, sizeof(char));
  sprintf(buffer, "[ROTATION] %s", rotation.str());
  return buffer;
}

/*
 * Default Scale constructor
 *----------------------------------------------------------------------------*/
Scale::Scale( void )
{

}

/*
 * Return the scale matrix
 *----------------------------------------------------------------------------*/
Matrix Scale::matrix()
{
  return Matrix((double)scale.x(), 0.0, 0.0, 0.0,
               0.0, (double)scale.y(), 0.0, 0.0,
               0.0, 0.0, (double)scale.z(), 0.0,
               0.0, 0.0, 0.0, 1.0);
}

/*
 * Scale to string function
 *----------------------------------------------------------------------------*/
char* Scale::str( void )
{
  char *buffer = (char*)calloc(300, sizeof(char));
  sprintf(buffer, "[SCALE] %s", scale.str());
  return buffer;
}


