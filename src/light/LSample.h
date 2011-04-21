/* 
 * File:   LSample.h
 * Author: cgibson
 *
 * Created on April 19, 2011, 10:29 PM
 */

#ifndef LSAMPLE_H
#define	LSAMPLE_H

class LiSample : public SceneObject {
public:
    LiSample(Vec3 pos, Color diffuse, float area):_pos(pos), _diffuse(diffuse), _radius(area){}

    inline Vec3 position(){ return _pos; }
    inline float area(){ return _radius; }
    inline Color diffuse(){ return _diffuse; }
    virtual inline TYPE getType( void ){ printf("ERROR: undef\n"); exit(1); return UNDEF_SAMPLE; }

private:
    float _radius;
    Vec3 _pos;       // Surface position
    Color _diffuse;    // Direct lighting data
};

#endif	/* LSAMPLE_H */

