
#include "RCanvas.h"

#include <algorithm>
#include <iostream>
#include <limits>

const float RGB_number = 255.0;
const int32_t RGB_int = 255;

void RCanvas::clear(const GColor& color) {
    GIRect rounded_rect = GIRect::WH(fDevice.width(),fDevice.height());
    GPixel new_pixel = GPixel_PackARGB(to_255_form(color.a,1),to_255_form(color.r,color.a),to_255_form(color.g,color.a),to_255_form(color.b,color.a)); // visit later
    for (int iy = rounded_rect.top; iy < rounded_rect.bottom; iy++){
        GPixel* old_pixel = get_pixel_addr(0, iy);
        for (int ix = rounded_rect.left; ix < rounded_rect.right; ix++){
            old_pixel[ix] = new_pixel;
        }
    }
};

void RCanvas::clipper(GPoint P0, GPoint P1, std::vector<Edge>* list) {
    GPoint P2;
    bool wind;

    if(P1.y - P0.y > 0){
      wind = false;
    } else {
      wind = true;
    }

    if (P0.y > P1.y) {
        GPoint temp = P0;
        P0 = P1;
        P1 = temp;
    }
    if (P0.y < 0 && P1.y < 0) {
        return;
    }
    if (P0.y > fDevice.height() && P1.y > fDevice.height()) {
        return;
    }
    if (P0.y < 0) {
        float w = (0 - P0.y) * (P1.x - P0.x) / (P1.y - P0.y);
        P0.y = 0;
        P0.x = P0.x + w;
    }
    if (P1.y > fDevice.height()) {
        float w = (P1.y - fDevice.height()) * (P1.x - P0.x) / (P1.y - P0.y) ;
        P1.y = fDevice.height();
        P1.x = P1.x - w;
    }

    if (P0.x > P1.x) {
        GPoint temp = P0;
        P0 = P1;
        P1 = temp;
    }

    if (P0.x < 0 && P1.x < 0) {
        P0.x = 0;
        P1.x = 0;
    }
    if (P0.x > fDevice.width() && P1.x > fDevice.width()) {
        P0.x = fDevice.width();
        P1.x = fDevice.width();
    }

    if (P0.x < 0) {
        float h = (0 - P0.x) * (P1.y - P0.y) / (P1.x - P0.x);
        P2.y = P0.y;
        P2.x = 0;
        P0.x = 0;
        P0.y = P0.y + h;

        Edge to_check = RCanvas::create_edge(P2,P0, wind);
        if (is_useful(to_check)) {
            list->push_back(to_check);
        }
    }

    if (P1.x > fDevice.width()) {
        float h = (P1.x - fDevice.width()) * (P1.y - P0.y) / (P1.x - P0.x);
        P2.y = P1.y;
        P2.x = fDevice.width();
        P1.x = fDevice.width();
        P1.y = P1.y - h;

        Edge to_check = RCanvas::create_edge(P2,P1,wind);
        if (is_useful(to_check)) {
            list->push_back(to_check);
        }
    }

    Edge final = RCanvas::create_edge(P0,P1,wind);
    if (is_useful(final)) {
        list->push_back(final);
    }
}

int RCanvas::get_x_intersect(Edge e, float y) {
    float z = e.b + (e.m * y);
    //if(!check_neg_zero(z)) {
    //    z = 0;
    //}
    return GRoundToInt(z + 0.0001f);
}

/**
* returns true if y is within our top and bottom, meaning
            this Y is valid for computing our corresponding X.
*/

bool isValid(Edge e, float y) {
    return y <= e.bottom && y >= e.top;
}


bool RCanvas::is_useful(Edge e) {
    return e.top < e.bottom;
}

Edge RCanvas::create_edge(GPoint P0, GPoint P1, bool wind) {
    Edge e;
    e.top = GRoundToInt(std::min(P0.y, P1.y));
    e.bottom = GRoundToInt(std::max(P0.y, P1.y));
    e.m = (P1.x - P0.x) / (P1.y - P0.y);
    if(wind){
      	e.wind = 1;
    } else {
        e.wind = -1;
    }

    if (e.m == -0) {
        e.m = 0;
    }

    e.b =  P1.x - (e.m * P1.y);
    return e;
}

Edge* clip_vert(Edge A) {
    return NULL;
}
Edge* clip_hor(Edge A) {
    return NULL;
}

//
// Created by raysh on 8/19/2024.
//
bool RCanvas::check_neg_zero(float x) {
    return (x * 0) == 0;
}

GPixel* RCanvas::get_pixel_addr(const int x, const int y){
    char* row = (char*)fDevice.pixels() + y * fDevice.rowBytes();
    return (GPixel*) row + x;
}

unsigned int div255(unsigned int n){
    return ((n + 128) * 257) >> 16;
}

GPixel blend_func_kClear(GPixel src, GPixel dst) {
    return 0;
}

GPixel blend_func_kSrc(GPixel src, GPixel dst) {
    return src;
}

GPixel blend_func_kDst(GPixel src, GPixel dst) {
    return dst;
}


GPixel blend_func_kSrcOver(GPixel src, GPixel dst){
    //        kSrcOver,  //!<     S + (1 - Sa)*D
    int a = GPixel_GetA(src) + div255((GPixel_GetA(dst) * (RGB_int - GPixel_GetA(src))));
    int r = GPixel_GetR(src) + div255((GPixel_GetR(dst) * (RGB_int - GPixel_GetA(src))));
    int g = GPixel_GetG(src) + div255((GPixel_GetG(dst) * (RGB_int - GPixel_GetA(src))));
    int b = GPixel_GetB(src) + div255((GPixel_GetB(dst) * (RGB_int - GPixel_GetA(src))));

    return GPixel_PackARGB(a, r, g, b);
}


GPixel blend_func_kDstOver(GPixel src, GPixel dst){
    //       kDstOver,  //!<     D + (1 - Da)*S
    int a = GPixel_GetA(dst) + div255(GPixel_GetA(src) * (RGB_int - GPixel_GetA(dst)));
    int r = GPixel_GetR(dst) + div255(GPixel_GetR(src) * (RGB_int - GPixel_GetA(dst)));
    int g = GPixel_GetG(dst) + div255(GPixel_GetG(src) * (RGB_int - GPixel_GetA(dst)));
    int b = GPixel_GetB(dst) + div255(GPixel_GetB(src) * (RGB_int - GPixel_GetA(dst)));

    return GPixel_PackARGB(a, r, g, b);
}

GPixel blend_func_kSrcIn(GPixel src, GPixel dst){
    //       kSrcIn,    //!<     Da * S
    int a = div255(GPixel_GetA(dst) * GPixel_GetA(src));
    int r = div255(GPixel_GetA(dst) * GPixel_GetR(src));
    int g = div255(GPixel_GetA(dst) * GPixel_GetG(src));
    int b = div255(GPixel_GetA(dst) * GPixel_GetB(src));

    return GPixel_PackARGB(a, r, g, b);
}
GPixel blend_func_kDstIn(GPixel src, GPixel dst){
    //      kDstIn,    //!<     Sa * D
    int a = div255(GPixel_GetA(src) * GPixel_GetA(dst));
    int r = div255(GPixel_GetA(src) * GPixel_GetR(dst));
    int g = div255(GPixel_GetA(src) * GPixel_GetG(dst));
    int b = div255(GPixel_GetA(src) * GPixel_GetB(dst));

    return GPixel_PackARGB(a, r, g, b);
}

GPixel blend_func_kSrcOut(GPixel src, GPixel dst){
    //      kSrcOut,   //!<     (1 - Da)*S
    int a = div255((RGB_int - GPixel_GetA(dst)) * GPixel_GetA(src));
    int r = div255((RGB_int - GPixel_GetA(dst)) * GPixel_GetR(src));
    int g = div255((RGB_int - GPixel_GetA(dst)) * GPixel_GetG(src));
    int b = div255((RGB_int - GPixel_GetA(dst)) * GPixel_GetB(src));

    return GPixel_PackARGB(a, r, g, b);
}

GPixel blend_func_kDstOut(GPixel src, GPixel dst){
    //      kDstOut,   //!<     (1 - Sa)*D
    int a = div255((RGB_int - GPixel_GetA(src)) * GPixel_GetA(dst));
    int r = div255((RGB_int - GPixel_GetA(src)) * GPixel_GetR(dst));
    int g = div255((RGB_int - GPixel_GetA(src)) * GPixel_GetG(dst));
    int b = div255((RGB_int - GPixel_GetA(src)) * GPixel_GetB(dst));

    return GPixel_PackARGB(a, r, g, b);
}

GPixel blend_func_kSrcATop(GPixel src, GPixel dst){
    //      kSrcATop,  //!<     Da*S + (1 - Sa)*D
    int a = div255((GPixel_GetA(dst) * GPixel_GetA(src)) + (RGB_int - GPixel_GetA(src)) * GPixel_GetA(dst));
    int r = div255((GPixel_GetA(dst) * GPixel_GetR(src)) + (RGB_int - GPixel_GetA(src)) * GPixel_GetR(dst));
    int g = div255((GPixel_GetA(dst) * GPixel_GetG(src)) + (RGB_int - GPixel_GetA(src)) * GPixel_GetG(dst));
    int b = div255((GPixel_GetA(dst) * GPixel_GetB(src)) + (RGB_int - GPixel_GetA(src)) * GPixel_GetB(dst));

    return GPixel_PackARGB(a, r, g, b);
}

GPixel blend_func_kDstATop(GPixel src, GPixel dst){
    //      kDstATop,  //!<     Sa*D + (1 - Da)*S
    int a = div255((GPixel_GetA(src) * GPixel_GetA(dst)) + (RGB_int - GPixel_GetA(dst)) * GPixel_GetA(src));
    int r = div255((GPixel_GetA(src) * GPixel_GetR(dst)) + (RGB_int - GPixel_GetA(dst)) * GPixel_GetR(src));
    int g = div255((GPixel_GetA(src) * GPixel_GetG(dst)) + (RGB_int - GPixel_GetA(dst)) * GPixel_GetG(src));
    int b = div255((GPixel_GetA(src) * GPixel_GetB(dst)) + (RGB_int - GPixel_GetA(dst)) * GPixel_GetB(src));

    return GPixel_PackARGB(a, r, g, b);
}

GPixel blend_func_kXor(GPixel src, GPixel dst){
    //      kXor,      //!<     (1 - Sa)*D + (1 - Da)*S
    int a = div255((RGB_int - GPixel_GetA(src)) * GPixel_GetA(dst) + (RGB_int - GPixel_GetA(dst)) * GPixel_GetA(src));
    int r = div255((RGB_int - GPixel_GetA(src)) * GPixel_GetR(dst) + (RGB_int - GPixel_GetA(dst)) * GPixel_GetR(src));
    int g = div255((RGB_int - GPixel_GetA(src)) * GPixel_GetG(dst) + (RGB_int - GPixel_GetA(dst)) * GPixel_GetG(src));
    int b = div255((RGB_int - GPixel_GetA(src)) * GPixel_GetB(dst) + (RGB_int - GPixel_GetA(dst)) * GPixel_GetB(src));

    return GPixel_PackARGB(a, r, g, b);
}

unsigned int RCanvas::to_255_form(float c, float a){
    float c255 = c * RGB_number * a;
    return GRoundToInt(c255);
}


/**
 *  Implemnt this, returning an instance of your subclass of GCanvas.
 */
std::unique_ptr<GCanvas> GCreateCanvas(const GBitmap& device){
    return std::unique_ptr<GCanvas>(new RCanvas(device));
}




/**
 *  Implement this, drawing into the provided canvas, and returning the title of your artwork.
 */
std::string GDrawSomething(GCanvas* canvas, GISize dim) {
    // as fancy as you like
    // ...
    // canvas->clear(...);
    // canvas->fillRect(...);
    //canvas->clear(GColor::RGBA(.5,0,0,1));
    GBitmap bm;
    bm.readFromFile("apps/spock.png");
    float dx = bm.width() * 0.2f;
    float dy = bm.height() * 0.7f;
    GPoint pts[] = {
        { dx, dx }, { dy, dy }, { dx, bm.height()*1.0f }, { bm.width()*1.0f, dy },
    };
    GRect my_rect = GRect::WH(dim.width,dim.height);
    GRect my_rect_2 = GRect::WH(dim.width/2,dim.height/2);
    canvas->fillRect(my_rect,GColor::RGBA(1,1,0,1));
    canvas->fillRect(my_rect_2,GColor::RGBA(0,1,1,1));
    canvas->fillRect(my_rect_2,GColor::RGBA(0,1,1,1));
    auto shader = GCreateBitmapShader(bm, GMatrix());
    GPaint paint(shader);
    canvas->drawConvexPolygon(pts, 4, paint);
    canvas->translate(-bm.width() / 2, -bm.height() / 2);
    canvas->drawConvexPolygon(pts, 4, paint);
    return "weird spock";
}

const BlendProc* gProcs[12] = {
    blend_func_kClear,
    blend_func_kSrc,
    blend_func_kDst,
    blend_func_kSrcOver,
    blend_func_kDstOver,
    blend_func_kSrcIn,
    blend_func_kDstIn,
    blend_func_kSrcOut,
    blend_func_kDstOut,
    blend_func_kSrcATop,
    blend_func_kDstATop,
    blend_func_kXor
};

void RCanvas::fill_line(GPixel row[], int N,GPixel src, const GPaint& p, BlendProc* bp) {
    GPixel lastPixel;
    GPixel lastBlend;
    if (N > 0) {
        lastPixel = row[0];
        lastBlend = bp(src, row[0]);
    } else {
        return;
    }
    //BlendProc proc = gProcs[(int)bm];
    for (int i = 0; i < N; i++) {
        if (row[i] == lastPixel) {
            row[i] = lastBlend;
        //} //else if (GPixel_GetA(src) == RGB_int && bm == GBlendMode::kSrcOver) {
           // row[i] = src;
        } else {
            lastPixel = row[i];
            lastBlend = bp(src, row[i]);
            row[i] = lastBlend;
        }
    }
}

void RCanvas::fill_line(GPixel row[], int N,GPixel * stor, BlendProc* bp) {
  GPixel lastPixel;
  GPixel lastShade;
  GPixel lastBlend;
  	if (N > 0) {
    	lastShade = stor[0];
        lastPixel = row[0];
        lastBlend = bp(stor[0], row[0]);
    } else {
        return;
    }
    for (int i = 0; i < N; i++) {
      if (row[i] == lastPixel && stor[i] == lastShade) {
            row[i] = lastBlend;
        } else {
            lastPixel = row[i];
            lastShade = stor[i];
            lastBlend = bp(stor[i], row[i]);
            row[i] = lastBlend;
        }
    }
}

void RCanvas::fill_line_no_blend(GPixel row[], int N, GPixel pixel) {
    for (int i = 0; i < N; i++) {
            row[i] = pixel;
    }
}

void RCanvas::fill_line_no_blend(GPixel row[], int N, GPixel *pixels) {
    for (int i = 0; i < N; i++) {
            row[i] = pixels[i];
    }
}

bool check_dest(BlendProc* bp) {
    return *bp == *gProcs[(int) GBlendMode::kDst];
}

void RCanvas::drawRect(const GRect& rect, const GPaint& paint) {
  GIRect rounded_rect = rect.round();


  	if (ctm != GMatrix()) {
	GPoint p[4];
    p[0].x = rounded_rect.left, p[0].y = rounded_rect.top;
    p[1].x = rounded_rect.right, p[1].y = rounded_rect.top;
    p[2].x = rounded_rect.right, p[2].y = rounded_rect.bottom;
    p[3].x = rounded_rect.left, p[3].y = rounded_rect.bottom;
    drawConvexPolygon(p,4, paint);

  	return;
    //nothing after this works because of return
	}
    GShader* peek_shder = paint.peekShader();
  	bool hasShder = peek_shder != NULL;
    GColor color;
    GPixel new_pixel;
    GBlendMode bm = paint.getBlendMode();
    BlendProc* bp;

    if (rounded_rect.left > fDevice.width() || rounded_rect.top > fDevice.height() || rounded_rect.right < 0 || rounded_rect.bottom < 0) {
        return;
    }
    if (hasShder) {
		if(!peek_shder->setContext(ctm)) return;
        bp = opt_blend(&bm, peek_shder->isOpaque());
  	} else {
		color = paint.getColor();
        new_pixel = GPixel_PackARGB(to_255_form(color.a,1),to_255_form(color.r,color.a),to_255_form(color.g,color.a),to_255_form(color.b,color.a)); // visit later
        bp = opt_blend(&bm, new_pixel);
    }

    //enum GBlendMode bMode = paint.getBlendMode();

    if (check_dest(bp)) {
        return;
    }
    //GPixel lastPixel = 0;
    //GPixel lastBlend = new_pixel;
    if (rounded_rect.left < 0){
        rounded_rect.left = 0;
    }
    if (rounded_rect.top < 0){
        rounded_rect.top = 0;
    }
    if (rounded_rect.bottom > fDevice.height()){
        rounded_rect.bottom = fDevice.height();
    }
    if (rounded_rect.right > fDevice.width()){
        rounded_rect.right = fDevice.width();
    }

    std::vector<GPixel> storage;
    storage.reserve(rounded_rect.right - rounded_rect.left);
    for (int iy = rounded_rect.top; iy < rounded_rect.bottom; iy++){
        GPixel* old_pixel = get_pixel_addr(rounded_rect.left, iy);
        if (hasShder) {
			if (bp == gProcs[(int) GBlendMode::kSrc]) {
                peek_shder->shadeRow(rounded_rect.left,iy,rounded_rect.right - rounded_rect.left,old_pixel);
           	 	//fill_line_no_blend(old_pixel,rounded_rect.right - rounded_rect.left,storage.data());
        	} else if (bp == gProcs[(int) GBlendMode::kClear]) {
           	 	fill_line_no_blend(old_pixel,rounded_rect.right - rounded_rect.left,(GPixel) 0);
        	} else {
           	 	peek_shder->shadeRow(rounded_rect.left,iy,rounded_rect.right - rounded_rect.left,storage.data());
				fill_line(old_pixel, rounded_rect.right - rounded_rect.left, storage.data(),bp);
        	}
        } else {
        	if (bp == gProcs[(int) GBlendMode::kSrc]) {
        	    fill_line_no_blend(old_pixel,rounded_rect.right - rounded_rect.left,new_pixel);
        	} else if (bp == gProcs[(int) GBlendMode::kClear]) {
        	    fill_line_no_blend(old_pixel,rounded_rect.right - rounded_rect.left,(GPixel) 0);
        	} else {
        	    fill_line(old_pixel, rounded_rect.right - rounded_rect.left, new_pixel,paint,bp);
        	}
        }
    }
};


/**
 *  Fill the convex polygon with the color, following the same "containment" rule as
 *  rectangles.
 */
void RCanvas::drawConvexPolygon(const GPoint points[], int count, const GPaint& paint) {
    if (count < 3) {
        return;
    }
    GShader* peek_shder = paint.peekShader();
    bool hasShder = peek_shder != NULL; // not sure

  	if (hasShder) {
		if(!peek_shder->setContext(ctm)) return;
  	}

    float right_x = std::numeric_limits<float>::min();
    float left_x = std::numeric_limits<float>::max();
    float top_y = std::numeric_limits<float>::max();
    float bot_y = std::numeric_limits<float>::min();
    //bool is_circle = false;


    //if (count == 4) {
    //    if (points[1].y - points[0].y == points[2].y - points[3].y && points[1].x - points[0].x == points[2].x - points[3].x) {
    //        GRect rect = GRect::XYWH(left_x,top_y, right_x-left_x,bot_y-top_y);
    //        drawRect(rect,paint);
    //        return;
    //    }
    //}

    //if (bot_y - top_y == right_x - left_x) {
    //    is_circle = true;
    //}
    std::vector<Edge> sorted_edges;

    GPaint p = paint;
    GColor color;
    GPixel new_pixel;
    GBlendMode bm = paint.getBlendMode();
    BlendProc* bp;

    if (!hasShder) {
      color = p.getColor();
      new_pixel = GPixel_PackARGB(to_255_form(color.a,1),to_255_form(color.r,color.a),to_255_form(color.g,color.a),to_255_form(color.b,color.a)); // visit later
      bp = opt_blend(&bm, new_pixel);
    } else {
      	//if (paint.getAlpha() == 0){ //might need to ditch on next pa
        //  return;
        //}
		bp = opt_blend(&bm, peek_shder->isOpaque());
      }


    if (check_dest(bp)) {
        return;
    }

    GPoint temp_pts[2];
    temp_pts[1] = points[0];
    ctm.mapPoints(&temp_pts[1],&temp_pts[1],1);
    GPoint save = temp_pts[1];

    for(int i = 0; i < count - 1; i++) {

        temp_pts[0] = temp_pts[1]; // could make faster
        temp_pts[1] = points[i + 1];
        ctm.mapPoints(&temp_pts[1],&temp_pts[1],1);

        if (i == 0){
          	if (temp_pts[0].x < left_x) {
            	left_x = temp_pts[0].x;
        	}
        	if (temp_pts[0].x > right_x) {
            	right_x = temp_pts[0].x;
        	}
        	if (temp_pts[0].y < top_y) {
            	top_y = temp_pts[0].y;
        	}
        	if (temp_pts[0].y > bot_y) {
            	bot_y = temp_pts[0].y;
        	}
          }

        if (temp_pts[1].x < left_x) {
            left_x = temp_pts[1].x;
        }
        if (temp_pts[1].x > right_x) {
            right_x = temp_pts[1].x;
        }
        if (temp_pts[1].y < top_y) {
            top_y = temp_pts[1].y;
        }
        if (temp_pts[1].y > bot_y) {
            bot_y = temp_pts[1].y;
        }
        clipper(temp_pts[0],temp_pts[1],&sorted_edges);
    }

    temp_pts[0] = temp_pts[1];
    temp_pts[1] = save;
    //ctm.mapPoints(temp_pts,temp_pts,2);
    clipper(temp_pts[0],temp_pts[1],&sorted_edges);

    if (GRoundToInt(right_x) <= 0 || GRoundToInt(top_y) >= fDevice.height() || GRoundToInt(bot_y) <= 0 || GRoundToInt(left_x) >= fDevice.width()) {
        return;
    }
    if (left_x >= right_x) {
        return;
    }
    if (top_y >= bot_y) {
        return;
    }

    if (top_y + 0.5 > bot_y) {
      return;
    }

    auto compare_func = [](Edge& a,Edge& b) {
        return a.top < b.top;
    };
    std::sort(sorted_edges.begin(),sorted_edges.end(),compare_func);
    bool L_expired = false;
    bool R_expired = false;
    bool swap = false;
    Edge edge_L = sorted_edges.front();
    Edge edge_R = *std::next(sorted_edges.begin());
    int top = GRoundToInt(sorted_edges.front().top);
    int bot = GRoundToInt(sorted_edges.back().bottom);
    int left;
    int right;
    std::vector<GPixel> storage;
    storage.reserve(GRoundToInt(right_x) - GRoundToInt(left_x));
	std::list<Edge> s_edges(sorted_edges.begin(), sorted_edges.end());
    for(int y = top; y < bot; y++) {
        const float y_prime = y + 0.5f;
        left = get_x_intersect(edge_L,y_prime);
        right = get_x_intersect(edge_R,y_prime);
        if (left > right) {
            Edge temp = edge_L;
            edge_L = edge_R;
            edge_R = temp;
            int temp2 = left;
            left = right;
            right = temp2;
            swap = !swap;
        }

        if (right - left != 0){
          	GPixel* old_pixel = get_pixel_addr(left, y);
            if (!hasShder) {
              	if (bp == gProcs[(int) GBlendMode::kSrc]) {
           	 		fill_line_no_blend(old_pixel,right - left,new_pixel);
        		} else if (bp == gProcs[(int) GBlendMode::kClear]) {
           	 		fill_line_no_blend(old_pixel,right - left,(GPixel) 0);
        		} else {
           	 		fill_line(old_pixel, right - left, new_pixel,p,bp);
        		}
              } else {
                    if (bp == gProcs[(int) GBlendMode::kSrc]) {
                    	peek_shder->shadeRow(left,y,right - left,old_pixel);
           	 			//fill_line_no_blend(old_pixel,right - left,storage.data());
        			} else if (bp == gProcs[(int) GBlendMode::kClear]) {
           	 			fill_line_no_blend(old_pixel,right - left,(GPixel) 0);
        			} else {
           	 			peek_shder->shadeRow(left,y,right - left,storage.data());
						fill_line(old_pixel, right - left, storage.data(),bp);
        			}
                }
          }

        if (y_prime + 1 > edge_L.bottom) {
            L_expired = true;
        }
        if (y_prime + 1 > edge_R.bottom) {
            R_expired = true;
        }
        if (L_expired && R_expired) {
            if (s_edges.size() < 4) {
                return;
            }
            s_edges.pop_front();
            s_edges.pop_front();
            edge_L = s_edges.front();
            edge_R = *std::next(s_edges.begin());
            swap = false;
            R_expired = false;
            L_expired = false;
        } else if (R_expired) {
            if (s_edges.size() < 3) {
                return;
            }
            if (!swap && s_edges.size() > 1) {
                std::swap(*s_edges.begin(), *next(s_edges.begin()));
                s_edges.pop_front();
            } else {
                s_edges.pop_front();
            }
            edge_R = *std::next(s_edges.begin());
            swap = false;
            R_expired = false;

        } else if (L_expired) {
            if (s_edges.size() < 3) {
                return;
            }
            if (swap && s_edges.size() > 1) {
                std::swap(*s_edges.begin(), *next(s_edges.begin()));
                s_edges.pop_front();
            } else {
                s_edges.pop_front();
            }
            edge_L = *std::next(s_edges.begin());
            swap = true;
            L_expired = false;
        }

    }
};

BlendProc* RCanvas::opt_blend(GBlendMode* bm, GPixel src) {
    switch (*bm){
        case GBlendMode::kClear:
            // kClear,    //!<     0
                return gProcs[(int) *bm];
        case GBlendMode::kSrc:
            //        kSrc,      //!<     S
                return gProcs[(int) *bm];
        case GBlendMode::kDst:
            //        kDst,      //!<     D
                return gProcs[(int) *bm];
        //check later if dst is 0
        case GBlendMode::kSrcOver:
            //        kSrcOver,  //!<     S + (1 - Sa)*D
                if (GPixel_GetA(src) == RGB_int) {
                    return gProcs[(int) GBlendMode::kSrc];
                } else if (GPixel_GetA(src) == 0) {
                    return gProcs[(int) GBlendMode::kDst];
                }
                return gProcs[(int) *bm];
        case GBlendMode::kDstOver:
            //       kDstOver,  //!<     D + (1 - Da)*S
                if (GPixel_GetA(src) == 0) {
                    return gProcs[(int) GBlendMode::kDst];
                }
                return gProcs[(int) *bm];
        case GBlendMode::kSrcIn:
            //       kSrcIn,    //!<     Da * S
                if (GPixel_GetA(src) == 0) {
                    return gProcs[(int) GBlendMode::kClear];
                }
                return gProcs[(int) *bm];
        case GBlendMode::kDstIn:
            //      kDstIn,    //!<     Sa * D
                if (GPixel_GetA(src) == 0) {
                    return gProcs[(int) GBlendMode::kClear];
                }
                return gProcs[(int) *bm];
        case GBlendMode::kSrcOut:
            //      kSrcOut,   //!<     (1 - Da)*S
                if (GPixel_GetA(src) == 0) {
                    return gProcs[(int) GBlendMode::kClear];
                }
                return gProcs[(int) *bm];
        case GBlendMode::kDstOut:
            //      kDstOut,   //!<     (1 - Sa)*D
                if (GPixel_GetA(src) == RGB_int) {
                    return gProcs[(int) GBlendMode::kClear];
                } else if (GPixel_GetA(src) == 0) {
                    return gProcs[(int) GBlendMode::kDst];
                }
                return gProcs[(int) *bm];
        case GBlendMode::kSrcATop:
            //      kSrcATop,  //!<     Da*S + (1 - Sa)*D
                if (GPixel_GetA(src) == RGB_int) {
                    return gProcs[(int) GBlendMode::kSrcIn];
                } else if (GPixel_GetA(src) == 0) {
                    return gProcs[(int) GBlendMode::kDst];
                }
                return gProcs[(int) *bm];
        case GBlendMode::kDstATop:
            //      kDstATop,  //!<     Sa*D + (1 - Da)*S
                if (GPixel_GetA(src) == RGB_int) {
                    return gProcs[(int) GBlendMode::kDstOver];
                } else if (GPixel_GetA(src) == 0) {
                    return gProcs[(int) GBlendMode::kClear];
                }
                return gProcs[(int) *bm];
        case GBlendMode::kXor:
            //      kXor,      //!<     (1 - Sa)*D + (1 - Da)*S
                if (GPixel_GetA(src) == RGB_int) {
                    return gProcs[(int) GBlendMode::kSrcOut];
                } else if (GPixel_GetA(src) == 0) {
                    return gProcs[(int) GBlendMode::kDst];
                }
                return gProcs[(int) *bm];
    }
    return gProcs[(int) *bm];
}

BlendProc* RCanvas::opt_blend(GBlendMode* bm, bool isO) {
   switch (*bm){
        case GBlendMode::kClear:
            // kClear,    //!<     0
                return gProcs[(int) *bm];
        case GBlendMode::kSrc:
            //        kSrc,      //!<     S
                return gProcs[(int) *bm];
        case GBlendMode::kDst:
            //        kDst,      //!<     D
                return gProcs[(int) *bm];
        //check later if dst is 0
        case GBlendMode::kSrcOver:
            //        kSrcOver,  //!<     S + (1 - Sa)*D
                if (isO) {
                    return gProcs[(int) GBlendMode::kSrc];
                }// else if (isA) {
                //    return gProcs[(int) GBlendMode::kDst];
                //}
                return gProcs[(int) *bm];
        case GBlendMode::kDstOver:
            //       kDstOver,  //!<     D + (1 - Da)*S
                //if (isA) {
                //    return gProcs[(int) GBlendMode::kDst];
                //}
                return gProcs[(int) *bm];
        case GBlendMode::kSrcIn:
            //       kSrcIn,    //!<     Da * S
                //if (isA) {
                //    return gProcs[(int) GBlendMode::kClear];
                //}
                return gProcs[(int) *bm];
        case GBlendMode::kDstIn:
            //      kDstIn,    //!<     Sa * D
                //if (isA) {
                //    return gProcs[(int) GBlendMode::kClear];
                //}
                return gProcs[(int) *bm];
        case GBlendMode::kSrcOut:
            //      kSrcOut,   //!<     (1 - Da)*S
                //if (isA) {
                //    return gProcs[(int) GBlendMode::kClear];
                //}
                return gProcs[(int) *bm];
        case GBlendMode::kDstOut:
            //      kDstOut,   //!<     (1 - Sa)*D
                if (isO) {
                    return gProcs[(int) GBlendMode::kClear];
                } //else if (isA) {
                //    return gProcs[(int) GBlendMode::kDst];
                //}
                return gProcs[(int) *bm];
        case GBlendMode::kSrcATop:
            //      kSrcATop,  //!<     Da*S + (1 - Sa)*D
                if (isO) {
                    return gProcs[(int) GBlendMode::kSrcIn];
                } //else if (isA) {
                //    return gProcs[(int) GBlendMode::kDst];
                //}
                return gProcs[(int) *bm];
        case GBlendMode::kDstATop:
            //      kDstATop,  //!<     Sa*D + (1 - Da)*S
                if (isO) {
                    return gProcs[(int) GBlendMode::kDstOver];
                } //else if (isA) {
                //    return gProcs[(int) GBlendMode::kClear];
                //}
                return gProcs[(int) *bm];
        case GBlendMode::kXor:
            //      kXor,      //!<     (1 - Sa)*D + (1 - Da)*S
                if (isO) {
                    return gProcs[(int) GBlendMode::kSrcOut];
                }// else if (isA) {
                //    return gProcs[(int) GBlendMode::kDst];
                //}
                return gProcs[(int) *bm];
    }
    return gProcs[(int) *bm];
}


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
void RCanvas::save(){
    GMatrix temp = GMatrix(ctm.e0(),ctm.e1(),ctm.origin());
    Mtxs.push_back(temp);
}

/**
 *  Copy the canvas state (CTM) that was record in the correspnding call to save() back into
 *  the canvas. It is an error to call restore() if there has been no previous call to save().
 */
void RCanvas::restore() {
    GMatrix temp = Mtxs.back();
    ctm = GMatrix(temp.e0(),temp.e1(),temp.origin());
	Mtxs.pop_back();
 }

/**
 *  Modifies the CTM by preconcatenating the specified matrix with the CTM. The canvas
 *  is constructed with an identity CTM.
 *
 *  CTM' = CTM * matrix
 */
void RCanvas::concat(const GMatrix& matrix){
    ctm = ctm * matrix;
 }

GPoint compute_quad(GPoint A, GPoint B, GPoint C, float t){
		float x = (((A.x * t) + B.x) * t) + C.x;
     	float y = (((A.y * t) + B.y) * t) + C.y;
     	return {x, y};
     }


GPoint compute_cubic(GPoint A, GPoint B, GPoint C, GPoint D,float t){
  		float x = (((((A.x * t) + B.x) * t) + C.x) * t) + D.x;
     	float y = (((((A.y * t) + B.y) * t) + C.y) * t) + D.y;
     	return {x, y};
  	}

float clamp(float value, float min, float max){
    return std::min(std::max(value,min),max);
  }

/**
*  Fill the path with the paint, interpreting the path using winding-fill (non-zero winding).
*/
void RCanvas::drawPath(const GPath& path, const GPaint& paint){
  	size_t count = path.countPoints();
  	if (count < 3) {
        return;
  	}
	GShader* peek_shder = paint.peekShader();
    bool hasShder = peek_shder != NULL; // not sure

  	if (hasShder) {
		if(!peek_shder->setContext(ctm)) return;
  	}



    std::vector<Edge> sorted_edges;

    GPaint p = paint;
    GColor color;
    GPixel new_pixel;
    GBlendMode bm = paint.getBlendMode();
    BlendProc* bp;

    if (!hasShder) {
      color = p.getColor();
      new_pixel = GPixel_PackARGB(to_255_form(color.a,1),to_255_form(color.r,color.a),to_255_form(color.g,color.a),to_255_form(color.b,color.a)); // visit later
      bp = opt_blend(&bm, new_pixel);
    } else {
		bp = opt_blend(&bm, peek_shder->isOpaque());
      }


    if (check_dest(bp)) {
        return;
    }

    std::shared_ptr<GPath> p_prime = path.transform(ctm);

    GRect bound_rect = p_prime->bounds();
    float right_x = bound_rect.right;
    float left_x = bound_rect.left;
    float top_y = bound_rect.top;
    float bot_y = bound_rect.bottom;
    bool no_need = false;

    if (GRoundToInt(right_x) <= 0 || GRoundToInt(top_y) >= fDevice.height() || GRoundToInt(bot_y) <= 0 || GRoundToInt(left_x) >= fDevice.width()) {
        return;
    }
    if (left_x >= right_x) {
        return;
    }
    if (top_y >= bot_y) {
        return;
    }
    if (GRoundToInt(right_x) == clamp(GRoundToInt(right_x),0,fDevice.width() - 1) and GRoundToInt(top_y) == clamp(GRoundToInt(top_y),0,fDevice.height() - 1) and GRoundToInt(bot_y) == clamp(GRoundToInt(bot_y),0,fDevice.height() - 1) and GRoundToInt(left_x) == clamp(GRoundToInt(left_x),0,fDevice.width() - 1)) {
        no_need = true;
    }


    GPoint pts[GPath::kMaxNextPoints];
    GPath::Edger edr(*p_prime);
    GPoint E0;
    GPoint E1;
    GPoint E;
    float length_e;
    int num_segs;
    GPoint A;
    GPoint B;
    GPoint C;
    GPoint D;
	GPoint prev;
    GPoint last;
    GPoint res;
    float start;
    float step;
    Edge final;
    while (auto v = edr.next(pts)){
      	switch(v.value()){
          case GPathVerb::kLine:
            if (no_need){
              final = RCanvas::create_edge(pts[0],pts[1],pts[1].y - pts[0].y <= 0);
              if (is_useful(final)) {
                sorted_edges.push_back(final);
              }
              } else {
          	clipper(pts[0],pts[1],&sorted_edges);
            }
            break;
          case GPathVerb::kQuad:
            E = (pts[0] + (-2.0f * pts[1]) + pts[2]);
            A = E;
            B = (-2.0f * pts[0]) + (2.0 * pts[1]);
 			C = pts[0];
            length_e = std::sqrt(std::pow(E.x / 4.0f,2.0f) + std::pow(E.y / 4.0f,2.0f));
            num_segs = (int) std::ceil(std::sqrt(length_e * 4.0f));
            prev = pts[0];
            last = pts[2];
			start = 1.0f / num_segs;
            step = start;
            for (int i = 0; i < num_segs - 1; i++){
			  res = compute_quad(A, B, C, start);
              if (no_need){
              final = RCanvas::create_edge(prev,res,res.y - prev.y <= 0);
                if (is_useful(final)) {
                  sorted_edges.push_back(final);
                }
              } else {
                clipper(prev,res,&sorted_edges);
              }
              prev = res;
              start += step;
            }
            if (no_need){
              final = RCanvas::create_edge(prev,last,last.y - prev.y <= 0);
              if (is_useful(final)) {
                sorted_edges.push_back(final);
              }
              } else {
              clipper(prev,last,&sorted_edges);
              }
            break;
          case GPathVerb::kCubic:
            E0 = (pts[0] + (-2.0f * pts[1]) + pts[2]);
            E1 = (pts[1] + (-2.0f * pts[2]) + pts[3]);
            E.x = std::max(abs(E0.x), abs(E1.x));
            E.y = std::max(abs(E0.y), abs(E1.y));
            length_e = std::sqrt(std::pow(E.x / 4.0f,2.0f) + std::pow(E.y / 4.0f,2.0f));
            num_segs = (int) std::ceil(std::sqrt(3.0 * length_e));
            A = pts[3] + (3.0f * pts[1]) + (-1.0f * pts[0]) + (-3.0f * pts[2]);
            B = (3.0f * pts[2]) + (-6.0f * pts[1]) + (3.0f * pts[0]);
 			C = (3.0f * pts[1]) + (-3.0f * pts[0]);
            D = pts[0];
            prev = pts[0];
            last = pts[3];
			start = 1.0f / num_segs;
            step = start;
            for (int i = 0; i < num_segs - 1; i++){
			  res = compute_cubic(A, B, C, D, start);
              if (no_need){
              final = RCanvas::create_edge(prev,res,res.y - prev.y <= 0);
                if (is_useful(final)) {
                  sorted_edges.push_back(final);
                }
              } else {
              clipper(prev,res,&sorted_edges);
              }
              prev = res;
              start += step;
            }
             if (no_need){
              final = RCanvas::create_edge(prev,last,last.y - prev.y <= 0);
                if (is_useful(final)) {
                  sorted_edges.push_back(final);
                }
              } else {
              clipper(prev,last,&sorted_edges);
              }
            break;
          }
      }

    /**
	*
	GPoint pts[GPath::kMaxNextPoints];
     *   GPath::Edger edger(path);
     *   while (auto v = edger.next(pts)) {
     *       switch (v.value()) {
     *           case GPath::kLine:
     *  }
	*/

    auto compare_func = [this](Edge& a,Edge& b) {
      	if(a.top == b.top){
          //probably a bug
          	return get_x_intersect(a,GRoundToInt(a.top) + 0.5f) < get_x_intersect(b,GRoundToInt(b.top) + 0.5f);
          } else {
            return a.top < b.top;
            }
    };

    std::sort(sorted_edges.begin(),sorted_edges.end(),compare_func);
    int top = GFloorToInt(top_y - 5); //fixes the bug but still dont know how to fully fix it
    int bot = GCeilToInt(bot_y);
    int w = 0;

    std::vector<GPixel> storage;
    storage.reserve(GRoundToInt(right_x) - GRoundToInt(left_x));
    std::list<Edge> to_sort(sorted_edges.begin(), sorted_edges.end());
    std::vector<Edge> dumb;
    for(int y = top; y < bot; y++) {
      	w = 0;
        const float y_prime = y + 0.5f;
        int left = GRoundToInt(left_x);
        auto it = to_sort.begin();
        // can try while (isValid(*iter, y_prime)) later
        while (it != to_sort.end() and isValid(*it, y_prime)) {
              int x = get_x_intersect(*it,y_prime);
              if(w == 0){
                  left = x;
              }
              w += it->wind;
              if (w == 0){
                  if (x - left != 0){
                      GPixel* old_pixel = get_pixel_addr(left, y);
                      if (!hasShder) {
                          if (bp == gProcs[(int) GBlendMode::kSrc]) {
                              fill_line_no_blend(old_pixel,x - left,new_pixel);
                          } else if (bp == gProcs[(int) GBlendMode::kClear]) {
                              fill_line_no_blend(old_pixel,x - left,(GPixel) 0);
                          } else {
                              fill_line(old_pixel, x - left, new_pixel,p,bp);
                          }
                      } else {
                          if (bp == gProcs[(int) GBlendMode::kSrc]) {
                              peek_shder->shadeRow(left,y,x - left,old_pixel);
                          } else if (bp == gProcs[(int) GBlendMode::kClear]) {
                              fill_line_no_blend(old_pixel,x - left,(GPixel) 0);
                          } else {
                              peek_shder->shadeRow(left,y,x - left,storage.data());
                              fill_line(old_pixel, x - left, storage.data(),bp);
                          }
                      }
                  }
              }
              if(!isValid(*it, y_prime + 1)) {
                  it = to_sort.erase(it);
              } else {
                //std::move(it,std::next(it),std::back_inserter(dumb));
                dumb.push_back(*it);
                it++;
              }
        }

    //auto newEnd = std::remove_if(to_sort.begin(), to_sort.end(),
    //                              [&it, &y_prime](Edge e) { bool b = isValid(e,y_prime) and !isValid(e,y_prime + 1);
    //                                                        if (b){
    //                                                          it--;}
    //                                                          return b; });
    //to_sort.erase(newEnd, to_sort.end());

    auto compare_func_x = [this, &y_prime](Edge& a,Edge& b) {
          return get_x_intersect(a,y_prime + 1) < get_x_intersect(b,y_prime + 1);
    };

    if(to_sort.size() <= 0){
      break;
     }

    while (it != to_sort.end() and isValid(*it, y_prime + 1)) {
      dumb.push_back(*it);
      //std::move(it,std::next(it),std::back_inserter(dumb));
      it++;
    }


	std::sort(dumb.begin(),dumb.end(),compare_func_x);
    std::move(dumb.begin(), dumb.end(), to_sort.begin());
    dumb.clear();

  	}
}


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
    void RCanvas::drawMesh(const GPoint verts[], const GColor colors[], const GPoint texs[],
                          int count, const int indices[], const GPaint& paint){
      GPoint temp_pts[3];
      GColor temp_cs[3];
      GPoint temp_texs[3];
      int index;
      GPaint temp_p = paint;
      GShader* shder = paint.peekShader();

	  if (colors != NULL and texs != NULL){
	        std::shared_ptr<TriShader> s_1;
            std::shared_ptr<TexShader> s_2;
            std::shared_ptr<CompShader> ts;
      for(int i = 0; i < count; i++){
         index = i * 3;
         temp_pts[0] = verts[indices[index]];
         temp_pts[1] = verts[indices[index + 1]];
         temp_pts[2] = verts[indices[index + 2]];
         temp_cs[0] = colors[indices[index]];
         temp_cs[1] = colors[indices[index + 1]];
         temp_cs[2] = colors[indices[index + 2]];
         temp_texs[0] = texs[indices[index]];
         temp_texs[1] = texs[indices[index + 1]];
         temp_texs[2] = texs[indices[index + 2]];
         s_1 = std::make_shared<TriShader>(temp_pts, temp_cs);
         s_2 = std::make_shared<TexShader>(temp_pts,temp_texs,shder);
         ts = std::make_shared<CompShader>(s_1,s_2);
         temp_p.setShader(ts); //might not need to do this
         drawConvexPolygon(temp_pts, 3, temp_p);
	  }
      }
      else if (colors != NULL) {
      std::shared_ptr<TriShader> ts;
      for(int i = 0; i < count; i++){
         index = i * 3;
         temp_pts[0] = verts[indices[index]];
         temp_pts[1] = verts[indices[index + 1]];
         temp_pts[2] = verts[indices[index + 2]];
         temp_cs[0] = colors[indices[index]];
         temp_cs[1] = colors[indices[index + 1]];
         temp_cs[2] = colors[indices[index + 2]];
         ts = std::make_shared<TriShader>(temp_pts, temp_cs);
         temp_p.setShader(ts); //might not need to do this
         drawConvexPolygon(temp_pts, 3, temp_p);
        }
	  } else if (texs != NULL) {
            std::shared_ptr<TexShader> ts;
		for(int i = 0; i < count; i++){
         index = i * 3;
         temp_pts[0] = verts[indices[index]];
         temp_pts[1] = verts[indices[index + 1]];
         temp_pts[2] = verts[indices[index + 2]];
         temp_texs[0] = texs[indices[index]];
         temp_texs[1] = texs[indices[index + 1]];
         temp_texs[2] = texs[indices[index + 2]];
         ts = std::make_shared<TexShader>(temp_pts,temp_texs,shder);
         temp_p.setShader(ts); //might not need to do this
         drawConvexPolygon(temp_pts, 3, temp_p);
        }
      }
      }

    GPoint quadLerp(const float u, const float v,const GPoint A,const GPoint B,const GPoint C,const GPoint D){
      float x = ((1 - u) * (1 - v) * A.x) + (u * (1-v) * B.x) + ((1-u) * v * D.x) + (u * v * C.x);
      float y = ((1 - u) * (1 - v) * A.y) + (u * (1-v) * B.y) + ((1-u) * v * D.y) + (u * v * C.y);
      return {x, y};
    }

    GColor quadLerp(const float u,const float v,const GColor A,const GColor B,const GColor C,const GColor D){
      GColor result = ((1 - u) * (1 - v) * A) + (u * (1-v) * B) + ((1-u) * v * D) + (u * v * C);
      return result;
    }

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
    void RCanvas::drawQuad(const GPoint verts[4], const GColor colors[4], const GPoint texs[4],
                          int level, const GPaint& p){

      float step = 1.0f / (level + 1.0f);
      int base = level + 2;
      int arr_size = base * base;
      int tri_count = 2 * (level + 1) * (level + 1);
      GPoint pts[arr_size];
      GColor cs[arr_size];
      GPoint txtrs[arr_size];
      int indices[tri_count * 3];

      if (colors != NULL){
        float v = 0;
        int i = 0;
        while (i < base){
        	int z = 0;
        	float u = 0;
        	while(z < base){
				pts[(i * base) + z] = quadLerp(u,v,verts[0],verts[1],verts[2],verts[3]);
        	    cs[(i * base) + z] = quadLerp(u,v,colors[0],colors[1],colors[2],colors[3]);
        	    z++;
        	    u = u + step;
        	}
            i++;
        	v = v + step;
        }

        int y = 0;
        i = 0;
        int i_1, i_2, i_3, i_4;
        while (i < base - 1){
        	int z = 0;
        	while(z < base - 1){
                i_1 = (i * base) + z;
                i_2 = (i * base) + z + 1;
                i_3 = ((i + 1) * base) + z;
                i_4 = ((i + 1) * base) + z + 1;

				indices[y] = i_1;
                indices[y + 1] = i_2;
        		indices[y + 2] = i_3;
                indices[y + 3] = i_3;
                indices[y + 4] = i_4;
            	indices[y + 5] = i_2;
                y += 6;
        	    z++;
        	}
            i++;
        }
        drawMesh(pts, cs, NULL,
                          tri_count, indices,p);
      } else if (texs != NULL){
        float v = 0;
        int i = 0;
        while (i < base){
        	int z = 0;
        	float u = 0;
        	while(z < base){
				pts[(i * base) + z] = quadLerp(u,v,verts[0],verts[1],verts[2],verts[3]);
        	    txtrs[(i * base) + z] = quadLerp(u,v,texs[0],texs[1],texs[2],texs[3]);
        	    z++;
        	    u = u + step;
        	}
            i++;
        	v = v + step;
        }

        int y = 0;
        i = 0;
        int i_1, i_2, i_3, i_4;
        while (i < base - 1){
        	int z = 0;
        	while(z < base - 1){
                i_1 = (i * base) + z;
                i_2 = (i * base) + z + 1;
                i_3 = ((i + 1) * base) + z;
                i_4 = ((i + 1) * base) + z + 1;

				indices[y] = i_1;
                indices[y + 1] = i_2;
        		indices[y + 2] = i_3;
                indices[y + 3] = i_3;
                indices[y + 4] = i_4;
            	indices[y + 5] = i_2;
                y += 6;
        	    z++;
        	}
            i++;
        }
        drawMesh(pts, NULL, txtrs,
                          tri_count, indices,p);
      }

      }
