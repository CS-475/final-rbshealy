//
// Created by raysh on 11/6/2024.
//

#include "TriShader.h"

const float RGB_number = 255.0;

bool TriShader::checkAlpha(const GColor* cs){
  for (int i = 0; i < 3; i++){
    if(abs(cs[i].a - 1.0f) > 0.0001f){
      return false;
    }
  }
  return true;
}

GMatrix TriShader::initMatrix(const GPoint *pts){
  GPoint v = pts[2] - pts[0];
  GPoint u = pts[1] - pts[0];
  //if(u == 1 and v == 1){
  //  return GMatrix();
  //}
  GMatrix m =  GMatrix(u.x, v.x, pts[0].x, u.y, v.y, pts[0].y);
  return m;
}


// Return true iff all of the GPixels that may be returned by this shader will be opaque.
bool TriShader::isOpaque(){
  return op;
  }


// The draw calls in GCanvas must call this with the CTM before any calls to shadeSpan().
bool TriShader::setContext(const GMatrix& ctm){
  nonstd::optional<GMatrix> temp =(ctm * mtrix).invert();

  if (temp.has_value()){
    for (int i = 0; i < 3; i++){
    p_prime[i] = *temp * points[i];
  }
    inv_ctm = *temp;
    return true;
  } else {
    return false;
  }
  }

unsigned int to_255_form(float c, float a){
    float c255 = c * RGB_number * a;
    return GRoundToInt(c255);
}

/**
 *  Given a row of pixels in device space [x, y] ... [x + count - 1, y], return the
 *  corresponding src pixels in row[0...count - 1]. The caller must ensure that row[]
 *  can hold at least [count] entries.
 */
void TriShader::shadeRow(int x, int y, int count, GPixel row[]){
  	const auto change = inv_ctm.e0();
   	GPoint loc = inv_ctm * GPoint{x + 0.5f, y + 0.5f};
    GColor c20 = colors[2] - colors[0];
    GColor c10 = colors[1] - colors[0];
    GColor del_c = (change.x * c10) + (change.y * c20);
    GColor col = (loc.x * c10) + (loc.y * c20) + colors[0];

    //det color
    for (int i = 0; i < count; i++){
    row[i] = GPixel_PackARGB(to_255_form(col.a, 1), to_255_form(col.r, col.a), to_255_form(col.g, col.a), to_255_form(col.b, col.a));
    col += del_c;
    }

  }



//void TriShader::shadeRow_mir(int x, int y, int count, GPixel row[]){
 // }

//void TriShader::shadeRow_rep(int x, int y, int count, GPixel row[]){
 // }