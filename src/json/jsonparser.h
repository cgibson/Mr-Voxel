#ifndef _JSON_H_
#define _JSON_H_

#include "../scene/scene.h"

class Parser {
public:
  Parser();
  bool succeeded(){ return !error; }
  virtual Scene *parse_file(char *filename, Dimension *size);

protected:
  bool error;
};

class JSONParser : public Parser{
public:
  JSONParser();
  virtual Scene *parse_file(char *filename, Dimension *size);
};

#endif
