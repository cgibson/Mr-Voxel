/* 
 * File:   sh.h
 * Author: cgibson
 *
 * Created on March 30, 2011, 6:31 PM
 */

#ifndef SH_H
#define	SH_H

#include "../util/util.h"
#include "../types.h"
#include <boost/shared_ptr.hpp>
#include <vector>

using std::vector;

using boost::shared_ptr;

class Surfel;

namespace light{
namespace sh{

    extern void SHProject(vector<shared_ptr<Surfel> > surfels, int lmax, SHCoef* coefs);
    extern Color reconstruct(Vec3 w, int lmax, SHCoef* coefs);

}
}

#endif	/* SH_H */

