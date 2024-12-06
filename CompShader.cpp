//
// Created by raysh on 11/15/2024.
//

#include "CompShader.h"

bool CompShader::isOpaque(){
  return op;
  }


// The draw calls in GCanvas must call this with the CTM before any calls to shadeSpan().
bool CompShader::setContext(const GMatrix& ctm){
      return s_1->setContext(ctm) && s_2->setContext(ctm);
      }


unsigned int div_255(unsigned int n){
  return ((n + 128) * 257) >> 16;
}

GPixel blend(GPixel pix_1, GPixel pix_2){
  unsigned int a = div_255(GPixel_GetA(pix_1) * GPixel_GetA(pix_2));
  unsigned int r = div_255(GPixel_GetR(pix_1) * GPixel_GetR(pix_2));
  unsigned int g = div_255(GPixel_GetG(pix_1) * GPixel_GetG(pix_2));
  unsigned int b = div_255(GPixel_GetB(pix_1) * GPixel_GetB(pix_2));
  return GPixel_PackARGB(a,r,g,b);
}

/**
 *  Given a row of pixels in device space [x, y] ... [x + count - 1, y], return the
 *  corresponding src pixels in row[0...count - 1]. The caller must ensure that row[]
 *  can hold at least [count] entries.
 */
void CompShader::shadeRow(int x, int y, int count, GPixel row[]){
  GPixel pix_1[count];
  GPixel pix_2[count];

  s_1->shadeRow(x, y, count, pix_1);
  s_2->shadeRow(x, y, count, pix_2);

  for(int i = 0; i < count; i++){
    row[i] = blend(pix_1[i], pix_2[i]);
    }
  }



