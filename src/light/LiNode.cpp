/* 
 * File:   Octree.cpp
 * Author: cgibson
 * 
 * Created on March 2, 2011, 7:05 PM
 */

#include "LiNode.h"
#include "../scene/Geometry.h"
#include "Surfel.h"
#include "sh.h"

#include "../system/config.h"

OctreeNode::OctreeNode( const Vec3 &min, const Vec3 &max ): _min(min), _max(max) {

    // Clear all children
    for(int i = 0; i < 8; i++)
        m_children[i] = NULL;
    _hasChildren = false;
}

OctreeNode::OctreeNode(const OctreeNode& orig) {

    // Copy all children
    for(int i = 0; i < 8; i++)
        m_children[i] = orig.m_children[i];
    _hasChildren = false;
}

OctreeNode::~OctreeNode() {
}

void
OctreeNode::cascadeDelete() {

    // Copy all children
    for(int i = 0; i < 8; i++) {
        if(m_children[i] != NULL) {
            m_children[i]->cascadeDelete();
            delete m_children[i];
        }
    }
}

int
LiNode::getTestCount( const Ray &ray, int *testCount ) {
    double tmin = -1;
    double thit;
    int tmp;
    Vec3 n;

    if(!OctreeNode::test_intersect( ray, &thit, &n )){
        *testCount = 1;
        return 0;
    }


    if(hasChildren()) {
        vector<pair<double, int> > ch_hit;
        vector<pair<double, int> >::iterator ch_It;
        double tmp_t;
        Vec3 tmp_n;
        for(int i = 0; i < 8; i++) {
            if(child(i) != NULL && child(i)->test_intersect(ray, &tmp_t, &tmp_n)) {
                ch_hit.push_back(pair<double, int>(tmp_t,i));
            }
        }
/*
        int test_tot = 0;
        int tmp_test = 0;
        tmp = 0;
        for(int i = 0; i < 8; i++) {
            tmp += child(i)->getTestCount( ray, &tmp_test);
            test_tot += tmp_test;
        }

        if(tmp > 0) {
            *testCount = test_tot;
            return 1;
        }else{
            *testCount = test_tot;
            return 0;
        }
        //*/
//*
        sort( ch_hit.begin(), ch_hit.end());

        int test_tot = 0;
        int tmp_test = 0;
        for(ch_It = ch_hit.begin(); ch_It != ch_hit.end(); ch_It++) {
            //printf("testing child %d\n", ch_It->second);
            assert(child(ch_It->second) != NULL);
            tmp = child(ch_It->second)->getTestCount( ray, &tmp_test);
            test_tot += tmp_test;

            if(tmp > 0) {
                *testCount = test_tot;
                return 1;
            }
        }
        *testCount = test_tot+1;
        return 0;
//*/
    }else{
        /**/
        if((_min.x() < ray.start.x() && _max.x() > ray.start.x()) &&
           (_min.y() < ray.start.y() && _max.y() > ray.start.y()) &&
           (_min.z() < ray.start.z() && _max.z() > ray.start.z())) {
           *testCount = 1;
           return 0;
        }
        *testCount = 1;

        for(int i = 0; i < _surfelCount; i++) {

            Ray tmpRay;
            assert(_surfelData[i].get() != NULL);
            shared_ptr<Surfel> s(_surfelData[i]);
            tmpRay.start = s->matrix * Vec4(ray.start, 1);
            tmpRay.direction = s->matrix * Vec4(ray.direction, 0);
            tmpRay.direction.norm();

            // Would be usefull if we wanted to suck at cheating
            //if(s->normal() * (ray.direction * -1) > 0) {
                if(s->test_intersect(tmpRay, &thit, &n)) {
                    if(thit < tmin || tmin < 0.) {
                        return 1;
                    }
                }
            //}
        }
    }

    *testCount = 1;

    return false;
}

int
OctreeNode::test_intersect( const Ray &ray, double *t, Vec3 * const n ) {

    double tmp_t;

    return test_intersect_region(ray, _min, _max, t, &tmp_t);
    
}

Color
LiNode::gather( const Ray &ray, double *t, Color *Tr ) {
    double tmin = -1;
    double thit;
    double Att;
    Color closest = config::background;
    Color tmp = 0.;
    Vec3 n;

    if(!OctreeNode::test_intersect( ray, &thit, &n )){
        *t = -1;
        return config::background;
    }


    if(hasChildren()) {
        vector<pair<double, int> > ch_hit;
        vector<pair<double, int> >::iterator ch_It;
        double tmp_t;
        Vec3 tmp_n;
        for(int i = 0; i < 8; i++) {
            if(child(i) != NULL && child(i)->test_intersect(ray, &tmp_t, &tmp_n)) {
                ch_hit.push_back(pair<double, int>(tmp_t,i));
            }
        }

        sort( ch_hit.begin(), ch_hit.end());

        for(ch_It = ch_hit.begin(); ch_It != ch_hit.end(); ch_It++) {
            //printf("testing child %d\n", ch_It->second);
            assert(child(ch_It->second) != NULL);
            tmp = tmp + child(ch_It->second)->gather( ray, &thit, Tr);
            /*if(thit < tmin) {
                tmin = thit;
                closest = tmp;
            }*/
            if(thit >= 0) {
                *t = thit;
                
                return tmp;
            }
        }

    }else{
        /**/
        if((_min.x() < ray.start.x() && _max.x() > ray.start.x()) &&
           (_min.y() < ray.start.y() && _max.y() > ray.start.y()) &&
           (_min.z() < ray.start.z() && _max.z() > ray.start.z())) {
           *t = -1;
           return config::background;
        }
        for(int i = 0; i < _surfelCount; i++) {

            Ray tmpRay;
            assert(_surfelData[i].get() != NULL);
            shared_ptr<Surfel> s(_surfelData[i]);
            tmpRay.start = s->matrix * Vec4(ray.start, 1);
            tmpRay.direction = s->matrix * Vec4(ray.direction, 0);
            tmpRay.direction.norm();

            // Would be usefull if we wanted to suck at cheating
            //if(s->normal() * (ray.direction * -1) > 0) {
                if(s->test_intersect(tmpRay, &thit, &n)) {
                    if(thit < tmin || tmin < 0.) {
                        tmin = thit;

                        // This is a horrible horrible hack.  This ensures that
                        // non-manifold surfaces will NOT operate correctly!
                        // At least, it ensures that the backsides of surfels
                        // will not be ignored, but simply blacked out... :-(
                        // But it makes stuff PRETTY :(((
                        if(s->normal() * (ray.direction * -1) > 0) {

                            Att = 1. / (config::atten_k * pow(thit, 2));
                            Att = (Att > 1.0) ? 1.0 : Att;
                            closest = s->diffuse() * (*Tr) * Att;
                        }
                        else
                            closest = 0.;
                    }
                }
            //}
        }

        //*

        // If we didn't hit anything
        float tClosest = (tmin < 0.) ? INFINITY : tmin;

        Color vRet = 0.;
        for(int i = 0; i < _lvoxelCount; i++) {

            Ray tmpRay = ray;
            assert(_lvoxelData[i].get() != NULL);
            shared_ptr<LVoxel> s(_lvoxelData[i]);

            //tmpRay.maxt = tClosest;
            if(s->test_intersect(tmpRay, &thit) && !s->inside(tmpRay.start)) {
                Att = 1. / (config::atten_k * pow(thit, 2));
                Att = (Att > 1.0) ? 1.0 : Att;
                vRet = vRet + s->integrate(Tr) * Att;
                //printf("Tr: %s\nReturned: %s\n", Tr.str(), vRet.str());
            }

        }
        vRet.clamp(0.0,0.99);
        tmp = tmp + vRet;

    }

    *t = tmin;

    closest.clamp(0.0, 10000.0);
    tmp.clamp(0.0, 10000.0);

    return closest + tmp;
}

int
LiNode::subdivide() {

    Vec3 size = (_max - _min) * 0.5;

    _hasChildren = true;

    if(size.length() < 0.00000001) {
        printf("SIZE: %d\n", _surfelCount);
        for(int i = 0; i < _surfelCount; i++) {
            Surfel surf = *_surfelData[i];
            printf("\t%s\n", surf.position().str());
        }
        printf("ERROR\n\tMIN: %s\n\tMAX: %s\n", _min.str(), _max.str());
        exit(1);
    }
    assert(size.length() > 0.00000001);
    Vec3 min, max;
    int x,y,z;

    // Loop over children
    for(int i = 0; i < 8; i++) {
        // Find out which child this is (in terms of what quadrant)
        x = i%2;
        y = (i/2)%2;
        z = (i/4);

        // Define minimum by which quadrant it is in
        min = Vec3(_min.x() + (size.x() * x),
                     _min.y() + (size.y() * y),
                     _min.z() + (size.z() * z));

        // Max is always the minimum plus the size of the node
        max = min + size;

        
        m_children[i] = new LiNode(min, max);
    }

    int surfCount = _surfelCount;
    int lvoxCount = _lvoxelCount;

    //_surfelCount = 0;

    // Iterate over all existing surfel data
    for(int i = 0; i < surfCount; i++) {

        // Re-add the surfel, allowing it to recurse into the children
        add(_surfelData[i]);
    }

    // Iterate over all existing lvoxel data
    for(int i = 0; i < lvoxCount; i++) {

        // Re-add the surfel, allowing it to recurse into the children
        add(_lvoxelData[i]);
    }
    
    // Clear all surfel data in this node
    //clear();

    return 0;
}

void
LiNode::cleanEmpty() {
    
}

int
LiNode::size_of() {
    int size = 0;

    if(hasChildren()) {
        for(int i = 0; i < 8; i++) {
            if(child(i) != NULL) {
                size += child(i)->size_of();
            }
        }
            size += sizeof(m_children);
            return size;
    }else{
        return sizeof(_surfelData) + (sizeof(shared_ptr<Surfel>) + sizeof(Surfel)) * _surfelCount;
    }

    return size;
}

int
LiNode::count() {
    int size = 0;

    if(hasChildren()) {
        for(int i = 0; i < 8; i++) {
            if(child(i) != NULL) {
                size += child(i)->count();
            }
        }
        return size;
        
    }else{
        return _surfelCount;
    }
}

LiNode::LiNode(Vec3 min, Vec3 max):OctreeNode(min,max), _surfelCount(0), _lvoxelCount(0){
    
    // Allocate data enough for the maximum amount of surfels
    _surfelData = new shared_ptr<Surfel>[MAX_SAMPLE_COUNT];
    _lvoxelData = new shared_ptr<LVoxel>[MAX_SAMPLE_COUNT];
    
}

int
LiNode::add(const shared_ptr<LiSample> obj) {

        //printf("Adding to NODE: %s - %s\n", m_min.str(), m_max.str());
    /*
    if(obj->getType() != SURFEL) {
        return 0;
    }
    //*/

    if(!inside(obj))
        return 0;
    
    // If this is a branch node
    if(hasChildren()) {
        //printf("Recursing\n");
        int res = 0;
        // Try adding it to every child node
        for(int i = 0; i < 8; i++) {
            if(child(i) != NULL) {
                res += child(i)->add(obj);
            }
        }

        // Keep track of how many surfels are underneath a given node
        if(obj->getType() == LVOXEL){
            _lvoxelCount++;
        }
        else
            _surfelCount++;
        
        // Any non-zero failures will be recorded
        return res;

    // Otherwise we are a leaf node
    }else{
        // Check if there are more samples than we can hold
        if(_surfelCount + _lvoxelCount >= MAX_SAMPLE_COUNT) {
            subdivide();
            // Re-add this element.  It will recurse in this time
            return add(obj);
        }else{
            assert(obj != NULL);
            assert(_surfelCount + _lvoxelCount < MAX_SAMPLE_COUNT);
            if(obj->getType() == SURFEL) {
                _surfelData[_surfelCount] = boost::static_pointer_cast<Surfel>(obj);
                _surfelCount++;
            }else{
                _lvoxelData[_lvoxelCount] = boost::static_pointer_cast<LVoxel>(obj);
                _lvoxelCount++;
            }
            //printf("Surfel count: %d\n", _surfelCount);

            return 0;
        }
    }
}


bool
LiNode::inside(const shared_ptr<LiSample> obj) {
    float dmin = 0;

    const Vec3 sphere_pos = obj->position();

    if(sphere_pos.x() > _min.x() && sphere_pos.x() < _max.x() &&
            sphere_pos.y() > _min.y() && sphere_pos.y() < _max.y() &&
            sphere_pos.z() > _min.z() && sphere_pos.z() < _max.z())
        return true;

    // X axis
    if( sphere_pos.x() < _min.x()) dmin += sqrt( sphere_pos.x() - _min.x()); else
    if( sphere_pos.x() > _max.x()) dmin += sqrt( sphere_pos.x() - _max.x());

    // Y axis
    if( sphere_pos.y() < _min.y()) dmin += sqrt( sphere_pos.y() - _min.y()); else
    if( sphere_pos.y() > _max.y()) dmin += sqrt( sphere_pos.y() - _max.y());

    // Z axis
    if( sphere_pos.z() < _min.z()) dmin += sqrt( sphere_pos.z() - _min.z()); else
    if( sphere_pos.z() > _max.z()) dmin += sqrt( sphere_pos.z() - _max.z());

    if( dmin <= (obj->area()) ) return true;

    return false;
}


int
LiNode::clear() {
    if(_surfelData != NULL) {
        delete _surfelData;
    }
    if(_lvoxelData != NULL) {
        delete _lvoxelData;
    }
}

void
LiNode::postprocess() {

    // If a branch
    if(hasChildren()) {
        // Process children first
        for( int i = 0; i < 8; i++) {
            assert(child(i) != NULL);
            if(child(i)->_surfelCount + child(i)->_lvoxelCount == 0) {
                delete child(i);
                m_children[i] = NULL;
            }else{
                child(i)->postprocess();
            }
        }
        // Sum children SH coefficients
        // TODO: Later

    // If a leaf
    }else{
        //printf("SURF: %d\n", _surfelCount);
        assert(_surfelCount + _lvoxelCount > 0);

        // Process lighting using surfel list
        vector<shared_ptr<Surfel> > surf_list;
        for(int i = 0; i < _surfelCount; i++) {
            
            surf_list.push_back(shared_ptr<Surfel>());
            surf_list[i] = _surfelData[i];
        }
        light::sh::SHProject(surf_list, 2, sh_c);

        assert(_surfelData[0] != NULL || _lvoxelData != NULL);
    }
}
