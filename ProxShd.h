//
// Created by raysh on 12/6/2024.
//

#ifndef PROXSHD_H
#define PROXSHD_H

#include "./include/GShader.h"
#include "./include/GBitmap.h"
#include "./include/GPoint.h"
#include "./include/GMatrix.h"
#include <algorithm>
#include "./include/GFinal.h"


class ProxShd: public GShader {
private:
    const bool op;
    GShader* inner_shader;
    bool first_time = true;
    const GColorMatrix cmat;

    bool checkAlpha();

public:
    ProxShd(GShader* shder, const GColorMatrix in_cmat) : op(shder->isOpaque()), inner_shader(shder), cmat(in_cmat)  {
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

private:
    void initMatrix();
};



#endif //PROXSHD_H
