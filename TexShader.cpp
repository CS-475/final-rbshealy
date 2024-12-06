//
// Created by raysh on 11/14/2024.
//

#include "TexShader.h"

const float RGB_number = 255.0;


void TexShader::initMatrix(){
  GPoint v_t = T_points[2] - T_points[0];
  GPoint u_t = T_points[1] - T_points[0];
  //if(u == 1 and v == 1){
  GPoint v_p = P_points[2] - P_points[0];
  GPoint u_p = P_points[1] - P_points[0];
  //}
  GMatrix m_t =  GMatrix(u_t.x, v_t.x, T_points[0].x, u_t.y, v_t.y, T_points[0].y);
  GMatrix m_p =  GMatrix(u_p.x, v_p.x, P_points[0].x, u_p.y, v_p.y, P_points[0].y);

  T = m_t;
  P = m_p;
}


// Return true iff all of the GPixels that may be returned by this shader will be opaque.
bool TexShader::isOpaque(){
  return op;
  }


// The draw calls in GCanvas must call this with the CTM before any calls to shadeSpan().
bool TexShader::setContext(const GMatrix& ctm){
  nonstd::optional<GMatrix> temp;
  if (first_time){
  nonstd::optional<GMatrix> t_inv = T.invert();
  if (!t_inv.has_value()){
    return false;
  }

 //not sure
   temp = ctm * P * t_inv.value();
   first_time = false;
} else {
  temp = ctm;
}

    return inner_shader->setContext(*temp);
  }

/**
 *  Given a row of pixels in device space [x, y] ... [x + count - 1, y], return the
 *  corresponding src pixels in row[0...count - 1]. The caller must ensure that row[]
 *  can hold at least [count] entries.
 */
void TexShader::shadeRow(int x, int y, int count, GPixel row[]){
    inner_shader->shadeRow(x, y, count, row);
  }
