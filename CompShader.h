//
// Created by raysh on 11/15/2024.
//

#ifndef COMPSHADER_H
#define COMPSHADER_H

#include "./include/GShader.h"
#include "./include/GBitmap.h"
#include "./include/GPoint.h"
#include "./include/GMatrix.h"
#include <algorithm>

class CompShader: public GShader {
private:
    const bool op;
    GShader* s_1;
    GShader* s_2;
public:
    CompShader(std::shared_ptr<GShader> shader_1, std::shared_ptr<GShader> shader_2) : op(shader_1->isOpaque() && shader_2->isOpaque()), s_1(shader_1.get()), s_2(shader_2.get())  {
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
};



#endif //COMPSHADER_H
