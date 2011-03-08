/**
 * Parser for .log and .oxy files.
 * ---------------------------------
 * author: Ryan Schmitt
 * date: 2-19-11
 */

#ifndef __SCI_DATA_PARSER__
#define __SCI_DATA_PARSER__

#include <vector>
#include <string>
#include <string.h>
#include <float.h>

#include "SciData.h"
#include "textfile.h"
#include "../util/util.h"
using std::vector;


class SciDataParser {
public:

   int parseFile(std::string const &data);
   vector<SciData> getData(){ return m_data; };

   Vector getMinLoc(){ return m_minLoc; }
   Vector getMaxLoc(){ return m_maxLoc; }

   double getMinO2(){ return m_minO2; }
   double getMaxO2(){ return m_maxO2; }

private:
   vector<SciData> m_data;

   Vector m_minLoc, m_maxLoc;
   double m_minO2, m_maxO2;

};

#endif
