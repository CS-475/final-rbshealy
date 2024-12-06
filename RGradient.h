//
// Created by raysh on 10/14/2024.
//

#ifndef RGRADIENT_H
#define RGRADIENT_H

#include "./include/GShader.h"
#include "./include/GMatrix.h"
#include "./include/GColor.h"
#include "./include/GPixel.h"
#include "./include/GPoint.h"


class RGradient: public GShader {
private:


    GMatrix inv_ctm;


    const GPoint p0;
    const GPoint p1;
    GColor* colors;
    GColor* blends;
    const int count;
    const bool op;
    const GMatrix mtrix;
    const GTileMode tmode;

    GMatrix initMatrix();
    GPixel getLocColor(float x);
    bool checkAlpha(const GColor* cs);
    unsigned int to_255_form(float c, float a);

public:
    RGradient(const GPoint p0,const GPoint p1,const GColor* cs,const int count, const GTileMode m) : p0(p0), p1(p1), count(count),op(checkAlpha(cs)), mtrix(initMatrix()), tmode(m) {
      colors = new GColor[count];
      blends = new GColor[count];
      for (int i = 0; i < count; i++) {
        colors[i] = cs[i];
        }
      for (int i = 0; i < count - 1; i++) {
        blends[i] = cs[i + 1] - cs[i];
      }
    }

    ~RGradient() {
      delete[] colors;
    }

    // Return true iff all of the GPixels that may be returned by this shader will be opaque.
    bool isOpaque() override;

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



#endif //RGRADIENT_H
