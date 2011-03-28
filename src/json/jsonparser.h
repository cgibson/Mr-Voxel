#ifndef _JSON_H_
#define _JSON_H_

#include "../scene/scene.h"
#include "../types.h"
#include <string>

using std::string;

class Parser {
public:
  Parser();
  bool succeeded(){ return !error; }
  virtual Scene *parse_file(string filename);

protected:
  bool error;
};

class JSONParser : public Parser{
public:
  JSONParser();
  virtual Scene *parse_file(string filename);
};

#endif
