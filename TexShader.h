//
// Created by raysh on 11/14/2024.
//

#include "./include/GShader.h"
#include "./include/GBitmap.h"
#include "./include/GPoint.h"
#include "./include/GMatrix.h"
#include <algorithm>

#ifndef TEXSHADER_H
#define TEXSHADER_H



class TexShader : public GShader {
private:
    const bool op;
    GPoint P_points[3];
    GPoint T_points[3];
    GMatrix T;
    GMatrix P;
    GShader* inner_shader;
    bool first_time = true;

    bool checkAlpha();

public:
    TexShader(GPoint* p_points, GPoint* t_points,GShader* shder) : op(shder->isOpaque()), inner_shader(shder)  {
        for (int i = 0; i < 3; i++) {
            P_points[i] = p_points[i];
            T_points[i] = t_points[i];
        }
        initMatrix();
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



#endif //TEXSHADER_H
