/***
 * Parser for .log and .oxy files.
 * ---------------------------------
 * author: Ryan Schmitt
 * date: 2-19-11
 */

#include "SciDataParser.h"


int
SciDataParser::parseFile(std::string const &data) {

   // read the files into buffers
   char* dataBuf = textFileRead(data.c_str());

   float O2, x, y, z;
   char *tok;
   int line = 0;
   tok = strtok(dataBuf, "\n");

   // instantiate beginning min/max values
   m_minLoc = Vec3(DBL_MAX,DBL_MAX,DBL_MAX);
   m_maxLoc = Vec3(-DBL_MAX,-DBL_MAX,-DBL_MAX);
   m_minO2 = DBL_MAX;
   m_maxO2 = -DBL_MAX;

   while(tok != NULL) {

      // scan in line information.  panic if the data does not match
      if(sscanf(tok, "%f %f %f %f", &x, &y, &z, &O2) <= 0) {
         printf("Error: Malformed file on line %d\n", line);
         exit(1);
      }

      // update minimum location
      if(x < m_minLoc.x()) m_minLoc.x(x);
      if(y < m_minLoc.y()) m_minLoc.y(y);
      if(z < m_minLoc.z()) m_minLoc.z(z);

      // update maximum location
      if(x > m_maxLoc.x()) m_maxLoc.x(x);
      if(y > m_maxLoc.y()) m_maxLoc.y(y);
      if(z > m_maxLoc.z()) m_maxLoc.z(z);

      // update min/max O2 values
      if(O2 < m_minO2) m_minO2 = O2;
      if(O2 > m_maxO2) m_maxO2 = O2;

      // push scidata into list
      m_data.push_back(SciData(x, y, z, O2));

      // move onto next line
      tok = strtok(NULL, "\n");
      line++;
   }

    // return how many data items were read in
    return m_data.size();

}
