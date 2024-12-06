//
// Created by raysh on 9/21/2024.
//

#include "./include/GMatrix.h"

///////////////////////////////////////////////////////////////////////////////////////////////
// These methods must be implemented by the student.

GMatrix::GMatrix(){
  fMat[0] = 1;    fMat[2] = 0;    fMat[4] = 0;
  fMat[1] = 0;    fMat[3] = 1;    fMat[5] = 0;
  }// initialize to identity
GMatrix GMatrix::Translate(float tx, float ty){
  return GMatrix(1,0,tx,0,1,ty);
  }
GMatrix GMatrix::Scale(float sx, float sy){
  return GMatrix(sx,0,0,0,sy,0);
    // might need to invert the y valuyes later
  }
GMatrix GMatrix::Rotate(float radians){
  return GMatrix(std::cos(radians),-std::sin(radians),0,std::sin(radians),std::cos(radians),0);
  }

/**
 *  Return the product of two matrices: a * b
 */
GMatrix GMatrix::Concat(const GMatrix& a, const GMatrix& b){
  return GMatrix((a[0]*b[0])+(a[2]*b[1]),(a[0]*b[2])+(a[2]*b[3]),(a[0]*b[4]) + (a[2] * b[5]) + a[4],(a[1]*b[0])+(a[3]*b[1]),(a[1]*b[2])+(a[3]*b[3]),(a[1]*b[4]) + (a[3] * b[5]) + a[5]);
  } // might need to change 3rd and 6th value later

  float convert_neg_zero(float f){
  if (f == -0) {
      return 0;
    } else {
      return f;
    }
}

/*
 *  If the inverse exists, return it, else return {} to signal no return value.
 */
nonstd::optional<GMatrix> GMatrix::invert() const{
  GMatrix temp;
  float det = (fMat[0] * fMat[3]) - (fMat[1] * fMat[2]);
  if (det != 0){
    temp = GMatrix(convert_neg_zero(fMat[3] / det),convert_neg_zero(-fMat[2] / det),convert_neg_zero(((fMat[2] * fMat[5]) - (fMat[3] * fMat[4])) / det),convert_neg_zero(-fMat[1] / det),convert_neg_zero(fMat[0] / det),convert_neg_zero(-((fMat[0] * fMat[5]) - (fMat[1] * fMat[4])) / det));
    //temp = GMatrix(convert_neg_zero(fMat[3] / det),convert_neg_zero(-fMat[2] / det),0,convert_neg_zero(-fMat[1] / det),convert_neg_zero(fMat[0] / det),0);
    return temp;
    } else {
      return {};
    }
  }


/**
 *  Transform the set of points in src, storing the resulting points in dst, by applying this
 *  matrix. It is the caller's responsibility to allocate dst to be at least as large as src.
 *
 *  [ a  c  e ] [ x ]     x' = ax + cy + e
 *  [ b  d  f ] [ y ]     y' = bx + dy + f
 *  [ 0  0  1 ] [ 1 ]
 *
 *  Note: It is legal for src and dst to point to the same memory (however, they may not
 *  partially overlap). Thus the following is supported.
 *
 *  GPoint pts[] = { ... };
 *  matrix.mapPoints(pts, pts, count);
 */
void GMatrix::mapPoints(GPoint dst[], const GPoint src[], int count) const{
  for (int i = 0; i < count; i++){
    float x_p = fMat[0] * src[i].x + fMat[2] * src[i].y + fMat[4];
    float y_p = fMat[1] * src[i].x + fMat[3] * src[i].y + fMat[5];
    dst[i].x = x_p;
    dst[i].y = y_p;
    }
  }
