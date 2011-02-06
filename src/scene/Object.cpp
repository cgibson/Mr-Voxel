#include "Object.h"


/*
 * Default Object constructor
 *----------------------------------------------------------------------------*/
Object::Object( void )
{

}

/*
 * Object to string function
 *----------------------------------------------------------------------------*/
char* Object::str( void )
{
  char* buffer = (char*)calloc(9, sizeof(char));
  strcpy(buffer, "[OBJECT]");
  return buffer;
}


/*
 * SceneObject constructor
 *----------------------------------------------------------------------------*/
SceneObject::SceneObject( void )
{
}

