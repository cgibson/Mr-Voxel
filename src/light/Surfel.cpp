/* 
 * File:   Surfel.cpp
 * Author: cgibson
 * 
 * Created on March 9, 2011, 10:46 PM
 */

#include "Surfel.h"

Surfel::Surfel(Vector position, Vector normal, Color diffuse, float area)
        : m_pos(position), m_normal(normal), m_diffuse(diffuse), m_area(area) {
}

Surfel::Surfel(const Surfel& orig) {
    m_normal = orig.m_normal;
    m_diffuse = orig.m_diffuse;
}

Surfel::~Surfel() {
}

