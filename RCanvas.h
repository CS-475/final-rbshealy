//
// Created by Ray Shealy on 9/4/2024.
//

#ifndef PA2_RBSHEALY_RCANVAS_H
#define PA2_RBSHEALY_RCANVAS_H

#include "./include/GCanvas.h"
#include "./include/GRect.h"
#include "./include/GColor.h"
#include "./include/GBitmap.h"
#include "./include/GPixel.h"
#include "./include/GMath.h"
#include "./include/GPaint.h"
#include "./include/GPoint.h"
#include "./include/GBlendMode.h"
#include "./include/GPath.h"
#include "RShader.h"
#include <list>
#include <deque>
#include "TriShader.h"
#include "TexShader.h"
#include "CompShader.h"

using BlendProc =GPixel (GPixel, GPixel);

struct Edge {
    float m;
    float b;
    int top;
    int bottom;
    int wind;
};

class RCanvas : public GCanvas {
public:
    RCanvas(const GBitmap& device) : fDevice(device) {

    }
    /**
       *  Fill the entire canvas with the specified color, using SRC porter-duff mode.
       */
    void clear(const GColor&) override;

    /**
        *  Fill the rectangle with the color, using SRC_OVER porter-duff mode.
        *
        *  The affected pixels are those whose centers are "contained" inside the rectangle:
        *      e.g. contained == center > min_edge && center <= max_edge
        *
        *  Any area in the rectangle that is outside of the bounds of the canvas is ignored.
        */

    void drawRect(const GRect&, const GPaint&) override;

    /**
     *  Fill the convex polygon with the color, following the same "containment" rule as
     *  rectangles.
     */
    void drawConvexPolygon(const GPoint[], int count, const GPaint&) override;

    /**
 *  Fill the path with the paint, interpreting the path using winding-fill (non-zero winding).
 */
    void drawPath(const GPath&, const GPaint&) override;


    /**
    *  Save off a copy of the canvas state (CTM), to be later used if the balancing call to
    *  restore() is made. Calls to save/restore can be nested:
    *  save();
    *      save();
    *          concat(...);    // this modifies the CTM
    *          .. draw         // these are drawn with the modified CTM
    *      restore();          // now the CTM is as it was when the 2nd save() call was made
    *      ..
    *  restore();              // now the CTM is as it was when the 1st save() call was made
    */
    void save() override;

    /**
     *  Copy the canvas state (CTM) that was record in the correspnding call to save() back into
     *  the canvas. It is an error to call restore() if there has been no previous call to save().
     */
    void restore() override;

    /**
     *  Modifies the CTM by preconcatenating the specified matrix with the CTM. The canvas
     *  is constructed with an identity CTM.
     *
     *  CTM' = CTM * matrix
     */
    void concat(const GMatrix& matrix) override;

    //GPixel blend_func_kClear(GPixel src, GPixel dst);
    //GPixel blend_func_kSrc(GPixel src, GPixel dst);
    //GPixel blend_func_kDst(GPixel src, GPixel dst);
    //GPixel blend_func_kSrcOver(GPixel src, GPixel dst);
    //GPixel blend_func_kDstOver(GPixel src, GPixel dst);
    //GPixel blend_func_kSrcIn(GPixel src, GPixel dst);
    //GPixel blend_func_kDstIn(GPixel src, GPixel dst);
    //GPixel blend_func_kSrcOut(GPixel src, GPixel dst);
    //GPixel blend_func_kDstOut(GPixel src, GPixel dst);
    //GPixel blend_func_kSrcATop(GPixel src, GPixel dst);
    //GPixel blend_func_kDstATop(GPixel src, GPixel dst);
    //GPixel blend_func_kXor(GPixel src, GPixel dst);

    /**
     *  Draw a mesh of triangles, with optional colors and/or texture-coordinates at each vertex.
     *
     *  The triangles are specified by successive triples of indices.
     *      int n = 0;
     *      for (i = 0; i < count; ++i) {
     *          point0 = vertx[indices[n+0]]
     *          point1 = verts[indices[n+1]]
     *          point2 = verts[indices[n+2]]
     *          ...
     *          n += 3
     *      }
     *
     *  If colors is not null, then each vertex has an associated color, to be interpolated
     *  across the triangle. The colors are referenced in the same way as the verts.
     *          color0 = colors[indices[n+0]]
     *          color1 = colors[indices[n+1]]
     *          color2 = colors[indices[n+2]]
     *
     *  If texs is not null, then each vertex has an associated texture coordinate, to be used
     *  to specify a coordinate in the paint's shader's space. If there is no shader on the
     *  paint, then texs[] should be ignored. It is referenced in the same way as verts and colors.
     *          texs0 = texs[indices[n+0]]
     *          texs1 = texs[indices[n+1]]
     *          texs2 = texs[indices[n+2]]
     *
     *  If both colors and texs[] are specified, then at each pixel their values are multiplied
     *  together, component by component.
     */
    void drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[],
                          int count, const int indices[], const GPaint&) override;

    /**
     *  Draw the quad, with optional color and/or texture coordinate at each corner. Tesselate
     *  the quad based on "level":
     *      level == 0 --> 1 quad  -->  2 triangles
     *      level == 1 --> 4 quads -->  8 triangles
     *      level == 2 --> 9 quads --> 18 triangles
     *      ...
     *  The 4 corners of the quad are specified in this order:
     *      top-left --> top-right --> bottom-right --> bottom-left
     *  Each quad is triangulated on the diagonal top-right --> bottom-left
     *      0---1
     *      |  /|
     *      | / |
     *      |/  |
     *      3---2
     *
     *  colors and/or texs can be null. The resulting triangles should be passed to drawMesh(...).
     *
     *  The order to draw the triangles is as follows: (e.g. levels == 1)
     *   |0 /|2 /|
     *   | / | / |
     *   |/ 1|/ 3|
     *   ---------
     *   |4 /|6 /|
     *   | / | / |
     *   |/ 5|/ 7|
     */
    void drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4],
                          int level, const GPaint&) override;


private:

    GPixel simple_blend(GPixel B, GPixel A);
    GPixel* get_pixel_addr(int x, int y);
    GPixel blend(GPixel dst, GPixel src, GBlendMode mode);
    unsigned int to_255_form(float c, float a);
    bool check_neg_zero(float x);
    //unsigned int div255(unsigned int n);
    void swap(GPoint *a, GPoint *b);
    Edge* clip_vert(Edge A);
    Edge* clip_hor(Edge A);
    Edge create_edge(GPoint P0, GPoint P1, bool is_rev);
    bool is_useful(Edge e);
    void fill_line(GPixel row[], int N, GPixel src, const GPaint& p, BlendProc* bp);
    void fill_line(GPixel row[], int N,GPixel* stor, BlendProc* bp);
    void fill_line_no_blend(GPixel row[], int N, GPixel pixel);
    void fill_line_no_blend(GPixel row[], int N, GPixel *pixels);
    int get_x_intersect(Edge e, float y);
    void clipper(GPoint P0, GPoint P1, std::vector<Edge>* list);
    const GBitmap& fDevice;
    BlendProc* opt_blend(GBlendMode* bm, GPixel src);
    BlendProc* opt_blend(GBlendMode* bm, bool isO);
    std::vector<GMatrix> Mtxs = std::vector<GMatrix>();
    GMatrix ctm = GMatrix();

};

#endif //PA2_RBSHEALY_RCANVAS_H
