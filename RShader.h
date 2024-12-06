//
// Created by raysh on 9/24/2024.
//

#include "./include/GShader.h"
#include "./include/GBitmap.h"
#include "./include/GPoint.h"
#include "./include/GMatrix.h"
#include <algorithm>

#ifndef RSHADER_H
#define RSHADER_H


class RShader : public GShader {
private:
    const GMatrix mtrix;
    const GBitmap bmap;
    GMatrix inv_ctm;
    const bool op;
    const GTileMode tmode;
    //std::vector<GPoint> vertices;
    //const bool alpha;

    bool checkAlpha();

public:
  //alpha(checkAlpha())
    RShader(const GBitmap& device,const GMatrix& matrix, const GTileMode m) : mtrix(matrix), bmap(device), op(device.isOpaque()), tmode(m)  {
    }

  // Return true iff all of the GPixels that may be returned by this shader will be opaque.
  bool isOpaque() override;

  //bool isAlpha();

  // The draw calls in GCanvas must call this with the CTM before any calls to shadeSpan().
  bool setContext(const GMatrix& ctm) override;

  /**
   *  Given a row of pixels in device space [x, y] ... [x + count - 1, y], return the
   *  corresponding src pixels in row[0...count - 1]. The caller must ensure that row[]
   *  can hold at least [count] entries.
   */
  void shadeRow(int x, int y, int count, GPixel row[]) override;

  void shadeRow_mir(int x, int y, int count, GPixel row[]);

  void shadeRow_rep(int x, int y, int count, GPixel row[]);

  };

#endif //RSHADER_H
