/*
 * Helper Matrix class with 4x4 matrix
 * provided by ZJ Wood for CSC 473
 * Heavily built on matrix class from Inventor SbMatrix class
 * available from Coin 3D (http://www.coin3d.org/)
 * modified 1/2010
 *       
 */

#include "MyMat.h"
#include <string.h>
#include <iostream>
#include <cfloat>
#include <math.h>

/* default constructor makes an identity matrix */
MyMat::MyMat(void) {
   int i,j;

   for (i=0;i<4;i++){
       for (j=0;j<4;j++){
         if (i==j)
             m_Elem[i][j]=1.0;
          else
             m_Elem[i][j]=0.0;
       }
   }
}

/* constructor to a specific matrix */
MyMat::MyMat(const double a11, const double a12,
             const double a13, const double a14,
             const double a21, const double a22,
             const double a23, const double a24,
             const double a31, const double a32,
             const double a33, const double a34,
             const double a41, const double a42,
             const double a43, const double a44) {
    m_Elem[0][0] = a11;
    m_Elem[0][1] = a12;
    m_Elem[0][2] = a13;
    m_Elem[0][3] = a14;
    m_Elem[1][0] = a21;
    m_Elem[1][1] = a22;
    m_Elem[1][2] = a23;
    m_Elem[1][3] = a24;
    m_Elem[2][0] = a31;
    m_Elem[2][1] = a32;
    m_Elem[2][2] = a33;
    m_Elem[2][3] = a34;
    m_Elem[3][0] = a41;
    m_Elem[3][1] = a42;
    m_Elem[3][2] = a43;
    m_Elem[3][3] = a44;
}

/* make the matrix the identity  - e.g. reset*/
void MyMat::makeIdentity(void) {
   int i,j;

   for (i=0;i<4;i++){
       for (j=0;j<4;j++){
         if (i==j)
             m_Elem[i][j]=1.0;
          else
             m_Elem[i][j]=0.0;
       }
   }
}

/*   similar to SbMatrix
 *   Return matrix components in the SbMat structure.
 */
const SbMat & MyMat::getValue(void) const
{
      return this->m_Elem;
}

/* set the values of the matrix to match input matrix m */
void MyMat::setValue(const MyMat &m) {
    const SbMat &temp_m = m.getValue();
    m_Elem[0][0] = temp_m[0][0];
    m_Elem[0][1] = temp_m[0][1];
    m_Elem[0][2] = temp_m[0][2];
    m_Elem[0][3] = temp_m[0][3];
    m_Elem[1][0] = temp_m[1][0];
    m_Elem[1][1] = temp_m[1][1];
    m_Elem[1][2] = temp_m[1][2];
    m_Elem[1][3] = temp_m[1][3];
    m_Elem[2][0] = temp_m[2][0];
    m_Elem[2][1] = temp_m[2][1];
    m_Elem[2][2] = temp_m[2][2];
    m_Elem[2][3] = temp_m[2][3];
    m_Elem[3][0] = temp_m[3][0];
    m_Elem[3][1] = temp_m[3][1];
    m_Elem[3][2] = temp_m[3][2];
    m_Elem[3][3] = temp_m[3][3];
}

/*   similar to SbMatrix
 *   Returns pointer to the 4 element array representing a matrix row.
 *   i should be within [0, 3].
 *   given the pointer to the row, [] can be used to access specific component
 */
double * MyMat::operator [](int i)
{
       return this->m_Elem[i];
}

/*   similar to SbMatrix
 *   Returns pointer to the 4 element array representing a matrix row.
 *   i should be within [0, 3].
 *   given the pointer to the row, [] can be used to access specific component
 */
const double * MyMat::operator [](int i) const
{
       return this->m_Elem[i];
}

/*   similar to SbMatrix
 *   Let this matrix be right-multiplied by \a m. Returns reference to
 *     self.
 *
 *       This is the most common multiplication / concatenation operation
 *         when using column-order matrices, as SbMatrix instances are, by
 *           definition.
 *             */
MyMat & MyMat::multRight(const MyMat & m)
{
  SbMat & tfm = this->m_Elem;
  const SbMat & mfm = m.m_Elem;

  SbMat tmp;
  (void)memcpy(tmp, tfm, 4*4*sizeof(double));

   for (int i=0; i < 4; i++) {
     for (int j=0; j < 4; j++) {
       tfm[i][j] =
           tmp[i][0] * mfm[0][j] +
           tmp[i][1] * mfm[1][j] +
           tmp[i][2] * mfm[2][j] +
           tmp[i][3] * mfm[3][j];
       }
    }

    return *this;
}

/*   similar to SbMatrix
 *   Let this matrix be left-multiplied by \a m. Returns reference to
 *     self.
 *
 *       (Be aware that it is more common to use the SbMatrix::multRight()
 *         operation, when doing concatenation of transformations, as SbMatrix
 *           instances are by definition in column-order, and uses
 *             post-multiplication for common geometry operations.)
 *               */
MyMat& MyMat::multLeft(const MyMat & m)
{
  SbMat & tfm = this->m_Elem;
  const SbMat & mfm = m.m_Elem;


  SbMat tmp;
  (void)memcpy(tmp, tfm, 4*4*sizeof(double));

  for (int i=0; i < 4; i++) {
   for (int j=0; j < 4; j++) {
        tfm[i][j] =
                tmp[0][j] * mfm[i][0] +
                tmp[1][j] * mfm[i][1] +
                tmp[2][j] * mfm[i][2] +
                tmp[3][j] * mfm[i][3];
    }
  }
  return *this;
}

MyMat MyMat::trans(void) const
{

  MyMat result;
  
  for(int i = 0; i < 4; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      result.m_Elem[i][j] = m_Elem[j][i];
    }
  }
 
  return result; 
}

/*   from SbMatrix
 *   Return a new matrix which is the inverse matrix of this.
 *
 *     The user is responsible for checking that this is a valid operation
 *       to execute, by first making sure that the result of SbMatrix::det4()
 *         is not equal to zero.
 *          */
MyMat MyMat::inverse(void) const
{

  MyMat result;

 //local pointers for speed
  double (*dst)[4];
  dst = reinterpret_cast<double (*)[4]>(result.m_Elem[0]);
  const double (*src)[4];
  src = reinterpret_cast<const double (*)[4]>(this->m_Elem[0]);

  //check for affine case - from Kevin, WU, 'Fast Matrix Inversion
  if (src[0][3] == 0.0 && src[1][3] == 0.0 &&
              src[2][3] == 0.0 && src[3][3] == 1.0) {
    double det_1;
    double pos, neg, temp;
#define ACCUMULATE     \
        if (temp >= 0.0)  \
            pos += temp;     \
         else               \
            neg += temp

                /*
                 * Calculate the determinant of submatrix A and determine if the
                 * the matrix is singular as limited by floating-point data
                 * representation.
                 */
                 pos = neg = 0.0;
                 temp =  src[0][0] * src[1][1] * src[2][2];
                 ACCUMULATE;
                 temp =  src[0][1] * src[1][2] * src[2][0];
                 ACCUMULATE;
                 temp =  src[0][2] * src[1][0] * src[2][1];
                 ACCUMULATE;
                 temp = -src[0][2] * src[1][1] * src[2][0];
                 ACCUMULATE;
                 temp = -src[0][1] * src[1][0] * src[2][2];
                 ACCUMULATE;
                 temp = -src[0][0] * src[1][2] * src[2][1];
                 ACCUMULATE;
                 det_1 = pos + neg;

#undef ACCUMULATE

    /* Is the submatrix A singular? */
        if ((det_1 == 0.0) || (fabs(det_1 / (pos - neg)) < FLT_EPSILON)) {
            /* Matrix M has no inverse */
                    std::cout <<  "Matrix is singular." << std::endl;
             return *this;
         }
         else {
             det_1 = 1.0 / det_1;
             dst[0][0] = (src[1][1] * src[2][2] -
                          src[1][2] * src[2][1]) * det_1;
             dst[1][0] = - (src[1][0] * src[2][2] -
                            src[1][2] * src[2][0]) * det_1;
             dst[2][0] = (src[1][0] * src[2][1] -
                         src[1][1] * src[2][0]) * det_1;
             dst[0][1] = - (src[0][1] * src[2][2] -
                            src[0][2] * src[2][1]) * det_1;
             dst[1][1] = (src[0][0] * src[2][2] -
                          src[0][2] * src[2][0]) * det_1;
             dst[2][1] = - (src[0][0] * src[2][1] -
                            src[0][1] * src[2][0]) * det_1;
             dst[0][2] =  (src[0][1] * src[1][2] -
                           src[0][2] * src[1][1]) * det_1;
             dst[1][2] = - (src[0][0] * src[1][2] -
                            src[0][2] * src[1][0]) * det_1;
             dst[2][2] =  (src[0][0] * src[1][1] -
                           src[0][1] * src[1][0]) * det_1;
                          
                          /* Calculate -C * inverse(A) */
              dst[3][0] = - (src[3][0] * dst[0][0] +
                             src[3][1] * dst[1][0] +
                             src[3][2] * dst[2][0]);
             dst[3][1] = - (src[3][0] * dst[0][1] +
                            src[3][1] * dst[1][1] +
                            src[3][2] * dst[2][1]);
             dst[3][2] = - (src[3][0] * dst[0][2] +
                            src[3][1] * dst[1][2] +
                            src[3][2] * dst[2][2]);
             /* Fill in last column */
             dst[0][3] = dst[1][3] = dst[2][3] = 0.0;
             dst[3][3] = 1.0;
           }
  } /*else non-affine*/
  else {
      //std::cout << "Non-affine!!!" << std::endl;
    double max, sum, tmp, inv_pivot;
    int p[4];
    int i, j, k;

    // algorithm from: Schwarz, "Numerische Mathematik"
    result = *this;

    for (k = 0; k < 4; k++) {
      max = 0.0;
      p[k] = 0;

      for (i = k; i < 4; i++) {
        sum = 0.0;
        for (j = k; j < 4; j++)
          sum += fabs(dst[i][j]);
        if (sum > 0.0) {
          tmp = fabs(dst[i][k]) / sum;
          if (tmp > max) {
            max = tmp;
            p[k] = i;
          }
        }
      }

      if (max == 0.0) {
        std::cout <<  "ERROR Matrix is singular." << std::endl;
        return *this;
      }

      if (p[k] != k) {
        for (j = 0; j < 4; j++) {
          tmp = dst[k][j];
          dst[k][j] = dst[p[k]][j];
          dst[p[k]][j] = tmp;
        }
      }

      inv_pivot = 1.0 / dst[k][k];
      for (j = 0; j < 4; j++) {
        if (j != k) {
          dst[k][j] = - dst[k][j] * inv_pivot;
          for (i = 0; i < 4; i++) {
            if (i != k) dst[i][j] += dst[i][k] * dst[k][j];
          }
        }
      }

      for (i = 0; i < 4; i++) dst[i][k] *= inv_pivot;
      dst[k][k] = inv_pivot;
    }

    for (k = 2; k >= 0; k--) {
      if (p[k] != k) {
        for (i = 0; i < 4; i++) {
          tmp = dst[i][k];
          dst[i][k] = dst[i][p[k]];
          dst[i][p[k]] = tmp;
        }
      }
    }
  }
  return result;
}

ostream& operator<<( ostream& os, const MyMat& m )
{
   int i,j;

   for (i=0;i<4;i++){
       for (j=0;j<4;j++){
             cout << m.m_Elem[i][j] << " ";
       }
       cout << endl;
   }
   return(os);
}

