/**
 * Class for storing some interesting scientific data
 * ---------------------------------
 * author: Ryan Schmitt
 * date: 2-19-11
 */

#ifndef __SCI_DATA__
#define __SCI_DATA__

#include <string>
#include <ctime>
#include <exception>

#include "../util/util.h"
#include "../types.h"

/* Some error definitions */
const int BAD_ARGUMENT_EXCEPTION = 1;

/* The main data class */
class SciData {
public:
   SciData() {}
   SciData(double x, double y, double z, double o2conc):m_o2conc(o2conc),m_loc(x,y,z){}
   ~SciData() {}
   void setLocation(double x, double y, double z);
   Vec3 getLocation() const {return m_loc;}
   void setO2Concentration(double concentration) {m_o2conc = concentration;}
   double getO2Concenration() const {return m_o2conc;}

protected:
   Vec3 m_loc;
   double m_o2conc;
};

#endif
