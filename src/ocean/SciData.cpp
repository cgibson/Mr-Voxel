/**
 * Class for storing some interesting scientific data
 * ---------------------------------
 * author: Ryan Schmitt
 * date: 2-19-11
 */

#include "SciData.h"

void SciData::setLocation(double x, double y, double z) {
   m_loc.x(x);
   m_loc.y(y);
   m_loc.z(z);
}