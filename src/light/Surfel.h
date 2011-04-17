/* 
 * File:   Surfel.h
 * Author: cgibson
 *
 * Created on March 9, 2011, 10:46 PM
 */

#ifndef SURFEL_H
#define	SURFEL_H

#include "../util/util.h"
#include "../util/rayutil.h"
#include "../scene/Geometry.h"

class LiSample : public SceneObject {
public:
    LiSample(Vec3 pos, float area):_pos(pos), _radius(area){}
    
    inline Vec3 position(){ return _pos; }
    inline float area(){ return _radius; }
    virtual inline TYPE getType( void ){ return UNDEF_SAMPLE; }
    
private:
    float _radius;
    Vec3 _pos;       // Surface position
};

class Surfel : public LiSample{
public:
    Surfel(const Vec3 &position, const Vec3 &normal, const Color &diffuse, float area);
    //Surfel(const Surfel& orig);
    virtual ~Surfel();
    int test_intersect( const Ray &ray, double *t, Vec3 * const n );
    Vec3 get_normal( Vec3 pt );

    inline Vec3 normal(){ return _normal; }
    inline Color diffuse(){ return _diffuse; }
    virtual inline TYPE getType( void ){ return SURFEL; }

    MyMat matrix;

private:

    float _height;
    float _innerRadius;
    float _phiMax;
    Vec3 _normal;    // Surface normal
    Color _diffuse;    // Direct lighting data
};

#endif	/* SURFEL_H */

