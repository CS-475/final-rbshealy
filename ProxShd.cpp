//
// Created by raysh on 12/6/2024.
//

#include "ProxShd.h"

//
// Created by raysh on 11/14/2024.
//
const float RGB_number = 255.0;


void ProxShd::initMatrix(){

}


// Return true iff all of the GPixels that may be returned by this shader will be opaque.
bool ProxShd::isOpaque(){
    return op;
}


// The draw calls in GCanvas must call this with the CTM before any calls to shadeSpan().
bool ProxShd::setContext(const GMatrix& ctm){
    nonstd::optional<GMatrix> temp;
    temp = ctm;
    return inner_shader->setContext(*temp);
}

/**
 *  Given a row of pixels in device space [x, y] ... [x + count - 1, y], return the
 *  corresponding src pixels in row[0...count - 1]. The caller must ensure that row[]
 *  can hold at least [count] entries.
 */
void ProxShd::shadeRow(int x, int y, int count, GPixel row[]){
    inner_shader->shadeRow(x, y, count, row);
}