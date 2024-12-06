//
// Created by raysh on 11/6/2024.
//
#include "./include/GShader.h"
#include "./include/GBitmap.h"
#include "./include/GPoint.h"
#include "./include/GMatrix.h"
#include <algorithm>

#ifndef TRISHADER_H
#define TRISHADER_H



class TriShader : public GShader {
private:
    const GMatrix mtrix;
    //const GBitmap bmap;
    GMatrix inv_ctm;
    const bool op;
    //const GTileMode tmode;
    GPoint points[3];
    GPoint p_prime[3];
    GColor colors[3];
    //float b_area;

    bool checkAlpha();

public:
    TriShader(const GPoint pts[3], const GColor cs[3]) : mtrix(initMatrix(pts)), op(checkAlpha(cs))  {
      for (int i = 0; i < 3; i++) {
        points[i] = pts[i];
        colors[i] = cs[i];
        }
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

    //void shadeRow_mir(int x, int y, int count, GPixel row[]);

    //void shadeRow_rep(int x, int y, int count, GPixel row[]);

    private:
        GMatrix initMatrix(const GPoint *pts);
        bool checkAlpha(const GColor* colors);

};



#endif //TRISHADER_H
