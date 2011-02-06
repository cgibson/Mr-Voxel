#include "stdlib.h"
#include "scene.h"
#include "stdio.h"

ObjectList::ObjectList()
{
  mSize = 0;
  mIncrement = 10;
  mMaxSize = mIncrement;
  mData = (SceneObject**)calloc( mMaxSize, sizeof(SceneObject*));
}

void ObjectList::resize()
{
  mMaxSize += mIncrement;
  mData = (SceneObject**)realloc(mData, mMaxSize * sizeof(SceneObject*));
  if(!mData)
  {
    printf("error: realloc issue\n");
    exit(-1);
  }
    
}

void ObjectList::add(SceneObject *node)
{
  if(node)
  {
    if(mSize >= mMaxSize) resize();
    
    mData[mSize] = node;
    mSize++;
  }else{
    cout << "warning: added null object to list" << endl;
  }
}

SceneObject *ObjectList::get(int index)
{
  if(index < 0 || index >= mSize)
    return NULL;
    
  return mData[index];
}

SceneObject *ObjectList::operator[] (unsigned i)
{
  if(mData == NULL)
  {
    printf("error: accessing uninitialized array\n");
    exit(-1);
  }
  if((i >= 0) && (i < mSize))
    return mData[i];
  else
    return NULL;
}

SceneObject *ObjectList::remove(int index)
{
  int i;
  SceneObject *object;
  
  if(index < 0 || index >= mSize)
    return NULL;
  
  object = mData[index];
  
  for(i = index; i < mSize-1; i++)
  {
    mData[i] = mData[i+1];
  }
  mData[mSize - 1] = NULL;
  mSize--;
  return object;
}

BBNode ObjectList::construct_bb()
{
  TYPE type;
  SceneObject *tmp_object;
  BBNode tmp_node, ret_node;
  int bb_built = false;
  int i;
  
  for(i = 0; i < mSize; i++)
  {
    tmp_object = mData[i];
    type = tmp_object->getType();
    if(type == NODE)
    {
      tmp_node = BBNode(((BBNode*)tmp_object)->min, ((BBNode*)tmp_object)->max);
    }else{
      tmp_node = ((GeomObj*)tmp_object)->construct_bb();
    }
    if(!bb_built)
    {
      ret_node = tmp_node;
      bb_built = true;
    }else{
      ret_node = ret_node.combine(tmp_node);
    }
    
  }
  return ret_node;
}
