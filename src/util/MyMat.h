#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <iostream>
#include "util.h"
using namespace std;

/* helper type */
typedef double SbMat[4][4];

/* 4x4 Matrix class with only bare bone functions for ray tracing */
class MyMat {
public:

	MyMat(); //public constructor
	MyMat(const double a11, const double a12, const double a13, const double a14, const double a21, const double a22, const double a23, const double a24, const double a31, const double a32, const double a33, const double a34, const double a41, const double a42, const double a43, const double a44) ;
	~MyMat(){} //destructor

    MyMat& multRight(const MyMat &m);
    MyMat& multLeft(const MyMat & m);

    MyMat inverse(void) const;
    MyMat trans(void) const;
    void setValue(const MyMat & m);
    void makeIdentity(void);
    double * operator [](int i);
    const double * operator [](int i) const;
    
    
    Vector operator*( const Vector4& v )
      { return Vector(m_Elem[0][0] * v.x() + m_Elem[0][1] * v.y() + m_Elem[0][2] * v.z() + m_Elem[0][3] * v.t(),
                     m_Elem[1][0] * v.x() + m_Elem[1][1] * v.y() + m_Elem[1][2] * v.z() + m_Elem[1][3] * v.t(),
                     m_Elem[2][0] * v.x() + m_Elem[2][1] * v.y() + m_Elem[2][2] * v.z() + m_Elem[2][3] * v.t()
               );
      }/*
    Vector operator*( const Vector& v )
      { return Vector(m_Elem[0][0] * v.x() + m_Elem[1][0] * v.y() + m_Elem[2][0] * v.z() + m_Elem[3][0],
                     m_Elem[0][1] * v.x() + m_Elem[1][1] * v.y() + m_Elem[2][1] * v.z() + m_Elem[3][1],
                     m_Elem[0][2] * v.x() + m_Elem[1][2] * v.y() + m_Elem[2][2] * v.z() + m_Elem[3][2]
               );
      }
      */


    const SbMat & getValue(void) const;

    friend ostream& operator<<( ostream&, const MyMat& );

private:
	SbMat m_Elem;
};

#endif
