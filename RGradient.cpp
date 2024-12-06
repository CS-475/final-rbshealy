//
// Created by raysh on 10/14/2024.
//

#include "RGradient.h"
#include <iostream>

const float RGB_number = 255.0;
const int32_t RGB_int = 255;

/**
*  Return a subclass of GShader that draws the specified gradient of [count] colors between
*  the two points. Color[0] corresponds to p0, and Color[count-1] corresponds to p1, and all
*  intermediate colors are evenly spaced between.
*
*  The gradient colors are GColors, and therefore unpremul. The output colors (in shadeRow)
*  are GPixel, and therefore premul. The gradient has to interpolate between pairs of GColors
*  before "pre" multiplying them into GPixels.
*
*  If count < 1, this should return nullptr.
*/
std::shared_ptr<GShader> GCreateLinearGradient(GPoint p0, GPoint p1, const GColor colors[], int count, GTileMode tMode){
    if (count < 1){
        return nullptr;
    }
    //std::shared_ptr<GShader> gradient = std::make_shared<RGradient>(RGradient(p0, p1, colors, count));
    return std::shared_ptr<GShader>(new RGradient(p0, p1, colors, count, tMode));
}

GMatrix RGradient::initMatrix(){
    float dy = p1.y - p0.y;
    float dx = p1.x - p0.x;
    if(count == 1){
        return GMatrix();
    }
    //nonstd::optional<GMatrix> temp = GMatrix().Scale(count - 1, count - 1).invert();
    //probably wrong
    //assert(temp.has_value());
    GMatrix m =  GMatrix(dx, -dy, p0.x, dy, dx, p0.y);
    return m;
  }


bool RGradient::isOpaque() {
  return op;
  }

bool RGradient::checkAlpha(const GColor* colors){
    for (int i = 0; i < count; i++){
        if(abs(colors[i].a - 1.0f) > 0.0001f){
            return false;
        }
    }
      return true;
  }

// The draw calls in GCanvas must call this with the CTM before any calls to shadeSpan().
bool RGradient::setContext(const GMatrix& ctm){
  nonstd::optional<GMatrix> temp =(ctm * mtrix).invert();
  if (temp.has_value()){
    inv_ctm = *temp;
    return true;
    } else {
      return false;
      }
  }

  float clamper(float value, float min, float max){
    return std::min(std::max(value,min),max);
  }

void RGradient::shadeRow_mir(int x, int y, int count, GPixel row[]){
	GPoint loc = inv_ctm * GPoint{float(x) + 0.5f, float(y)+ 0.5f};
    const auto change = inv_ctm.e0();
    GPixel result = 0;
    GColor c;

    if(this->count == 1){
      c = colors[0];
      result = GPixel_PackARGB(to_255_form(c.a, 1), to_255_form(c.r, c.a), to_255_form(c.g, c.a), to_255_form(c.b, c.a));
      for (int i = 0; i < count; i++){
    		row[i] = result;
	    }
    	return;
    }
	float lox;
    int temp;

    if(this->count == 2){
      if(this->op and GFloorToInt(abs(loc.x)) == GFloorToInt(abs(loc.x + (change.x * count))) and change.x * count < 1){
        lox = abs(loc.x);
        temp = GFloorToInt(lox) % 2;
      	lox = lox - GFloorToInt(lox);
        if (temp == 1){
          lox = 1 - lox;
        }
        c = colors[0] + blends[0] * lox;
        GColor dc = blends[0] * change.x;
      	for (int i = 0; i < count; i++){
      		result = GPixel_PackARGB(RGB_int, GRoundToInt(c.r * RGB_int), GRoundToInt(c.g * RGB_int), GRoundToInt(c.b * RGB_int));
    		row[i] = result;
            c += dc;
	    }
		return;
      }
      else if (this->op){
        for (int i = 0; i < count; i++){
            lox = abs(loc.x);
            temp = GFloorToInt(lox) % 2;
      	    lox = lox - GFloorToInt(lox);
            if (temp == 1){
              lox = 1 - lox;
            }
        	c = colors[0] + blends[0] * lox;
      		result = GPixel_PackARGB(RGB_int, GRoundToInt(c.r * RGB_int), GRoundToInt(c.g * RGB_int), GRoundToInt(c.b * RGB_int));
    		row[i] = result;
            loc.x += change.x;
	    }
		return;
      }else if(GFloorToInt(abs(loc.x)) == GFloorToInt(abs(loc.x + (change.x * count))) and change.x * count < 1){
        lox = abs(loc.x);
        temp = GFloorToInt(lox) % 2;
      	lox = lox - GFloorToInt(lox);
        if (temp == 1){
            lox = 1 - lox;
        }
        c = colors[0] + blends[0] * lox;
        GColor dc = blends[0] * change.x;
      	for (int i = 0; i < count; i++){
      		result = GPixel_PackARGB(to_255_form(c.a, 1), to_255_form(c.r, c.a), to_255_form(c.g, c.a), to_255_form(c.b, c.a));
    		row[i] = result;
            c += dc;
	    }
		return;
      } else {
      for (int i = 0; i < count; i++){
        	lox = abs(loc.x);
            temp = GFloorToInt(lox) % 2;
      		lox = lox - GFloorToInt(lox);
            if (temp == 1){
              lox = 1 - lox;
              }
        	c = colors[0] + blends[0] * lox;
      		result = GPixel_PackARGB(to_255_form(c.a, 1), to_255_form(c.r, c.a), to_255_form(c.g, c.a), to_255_form(c.b, c.a));
    		row[i] = result;
            loc.x += change.x;
	  }
		return;
      }
	}


        int index;
        float t;
        // not gonna work if the colors change in the middle

        for (int i = 0; i < count; i++){
            lox = abs(loc.x);
            temp = GFloorToInt(lox) % 2;
      		lox = lox - GFloorToInt(lox);
            if (temp == 1){
              lox = 1 - lox;
              }
  			lox = lox * (this->count - 1);

  			index = GFloorToInt(lox);
  			t = lox - index;
  			c = colors[index] + (blends[index] * t);
    		row[i] = GPixel_PackARGB(to_255_form(c.a, 1), to_255_form(c.r, c.a), to_255_form(c.g, c.a), to_255_form(c.b, c.a));
            loc.x += change.x;
	    }
  }

void RGradient::shadeRow_rep(int x, int y, int count, GPixel row[]){
  GPoint loc = inv_ctm * GPoint{float(x) + 0.5f, float(y)+ 0.5f};
    const auto change = inv_ctm.e0();
    GPixel result = 0;
    GColor c;

    if(this->count == 1){
      c = colors[0];
      result = GPixel_PackARGB(to_255_form(c.a, 1), to_255_form(c.r, c.a), to_255_form(c.g, c.a), to_255_form(c.b, c.a));
      for (int i = 0; i < count; i++){
    		row[i] = result;
	    }
    	return;
    }
	float lox;

    if(this->count == 2){
      if(this->op and GFloorToInt(abs(loc.x)) == GFloorToInt(abs(loc.x + (change.x * count))) and change.x * count < 1){
        lox = abs(loc.x);
        lox = lox - GFloorToInt(lox);
        c = colors[0] + blends[0] * lox;
        GColor dc = blends[0] * change.x;
      	for (int i = 0; i < count; i++){
      		result = GPixel_PackARGB(RGB_int, GRoundToInt(c.r * RGB_int), GRoundToInt(c.g * RGB_int), GRoundToInt(c.b * RGB_int));
    		row[i] = result;
            c += dc;
	    }
		return;
      }
      else if (this->op){
        for (int i = 0; i < count; i++){
            lox = abs(loc.x);
            lox = lox - GFloorToInt(lox);
        	c = colors[0] + blends[0] * lox;
      		result = GPixel_PackARGB(RGB_int, GRoundToInt(c.r * RGB_int), GRoundToInt(c.g * RGB_int), GRoundToInt(c.b * RGB_int));
    		row[i] = result;
            loc.x += change.x;
	    }
		return;
      }else if(GFloorToInt(abs(loc.x)) == GFloorToInt(abs(loc.x + (change.x * count))) and change.x * count < 1){
        lox = abs(loc.x);
        lox = lox - GFloorToInt(lox);
        c = colors[0] + blends[0] * lox;
        GColor dc = blends[0] * change.x;
      	for (int i = 0; i < count; i++){
      		result = GPixel_PackARGB(to_255_form(c.a, 1), to_255_form(c.r, c.a), to_255_form(c.g, c.a), to_255_form(c.b, c.a));
    		row[i] = result;
            c += dc;
	    }
		return;
      } else {
      for (int i = 0; i < count; i++){
        	lox = abs(loc.x);
      		lox = lox - GFloorToInt(lox);
        	c = colors[0] + blends[0] * lox;
      		result = GPixel_PackARGB(to_255_form(c.a, 1), to_255_form(c.r, c.a), to_255_form(c.g, c.a), to_255_form(c.b, c.a));
    		row[i] = result;
            loc.x += change.x;
	  }
		return;
      }
   }


        int index;
        float t;
        // not gonna work if the colors change in the middle

        for (int i = 0; i < count; i++){
            lox = abs(loc.x);
            lox = lox- GFloorToInt(lox);
  			lox = lox * (this->count - 1);

  			index = GFloorToInt(lox);
  			t = lox - index;
  			c = colors[index] + (blends[index] * t);
    		row[i] = GPixel_PackARGB(to_255_form(c.a, 1), to_255_form(c.r, c.a), to_255_form(c.g, c.a), to_255_form(c.b, c.a));
            loc.x += change.x;
	    }
  }

/**
 *  Given a row of pixels in device space [x, y] ... [x + count - 1, y], return the
 *  corresponding src pixels in row[0...count - 1]. The caller must ensure that row[]
 *  can hold at least [count] entries.
 */
void RGradient::shadeRow(int x, int y, int count, GPixel row[]) {

  	switch (tmode) {
        case GTileMode::kMirror:
          shadeRow_mir(x, y, count, row);
          return;
        case GTileMode::kRepeat:
          shadeRow_rep(x, y, count, row);
          return;
        case GTileMode::kClamp:
          break;
  	}

   	GPoint loc = inv_ctm * GPoint{float(x) + 0.5f, float(y)+ 0.5f};
    const auto change = inv_ctm.e0();
    GPixel result = 0;
    GColor c;

    if(this->count == 1){
      c = colors[0];
      result = GPixel_PackARGB(to_255_form(c.a, 1), to_255_form(c.r, c.a), to_255_form(c.g, c.a), to_255_form(c.b, c.a));
      for (int i = 0; i < count; i++){
    		row[i] = result;
	    }
    	return;
    }

    if(this->count == 2){
      if(this->op and loc.x == clamper(loc.x, 0, 0.99999f) and loc.x + (change.x * count) == clamper(loc.x + (change.x * count), 0, 0.99999f) and change.x * count < 1){
        c = colors[0] + blends[0] * loc.x;
        GColor dc = blends[0] * change.x;
      	for (int i = 0; i < count; i++){
      		result = GPixel_PackARGB(RGB_int, GRoundToInt(c.r * RGB_int), GRoundToInt(c.g * RGB_int), GRoundToInt(c.b * RGB_int));
    		row[i] = result;
            c += dc;
	    }
		return;
      }
      else if (this->op){
		loc.x = clamper(loc.x,0,0.99999f);
        for (int i = 0; i < count; i++){
        	c = colors[0] + blends[0] * loc.x;
      		result = GPixel_PackARGB(RGB_int, GRoundToInt(c.r * RGB_int), GRoundToInt(c.g * RGB_int), GRoundToInt(c.b * RGB_int));
    		row[i] = result;
            loc.x += change.x;
	    }
		return;
      }else if(loc.x == clamper(loc.x, 0, 0.99999f) and loc.x + (change.x * count) == clamper(loc.x + (change.x * count), 0, 0.99999f) and change.x * count < 1){
        c = colors[0] + blends[0] * loc.x;
        GColor dc = blends[0] * change.x;
      	for (int i = 0; i < count; i++){
      		result = GPixel_PackARGB(to_255_form(c.a, 1), to_255_form(c.r, c.a), to_255_form(c.g, c.a), to_255_form(c.b, c.a));
    		row[i] = result;
            c += dc;
	    }
		return;
		} else {
      loc.x = clamper(loc.x,0,0.99999f);
      for (int i = 0; i < count; i++){
        	c = colors[0] + blends[0] * loc.x;
      		result = GPixel_PackARGB(to_255_form(c.a, 1), to_255_form(c.r, c.a), to_255_form(c.g, c.a), to_255_form(c.b, c.a));
    		row[i] = result;
            loc.x += change.x;
	  }
		return;
    	}
	}


        float lox;
        int index;
        float t;
        // not gonna work if the colors change in the middle

    if (this->op and loc.x == clamper(loc.x, 0, 0.99999f) and loc.x + (change.x * count) == clamper(loc.x + (change.x * count), 0, 0.99999f) and change.x * count < 1 / this->count-1){
		  loc.x = loc.x * (count - 1);
        int index = GFloorToInt(loc.x);
        c = colors[index] + blends[index] * loc.x;
        GColor dc = blends[index] * change.x;
      	for (int i = 0; i < count; i++){
      		result = GPixel_PackARGB(RGB_int, GRoundToInt(c.r * RGB_int), GRoundToInt(c.g * RGB_int), GRoundToInt(c.b * RGB_int));
    		row[i] = result;
            c += dc;
	    }
		return;
    }
    if (this->op){
		for (int i = 0; i < count; i++){
            lox = clamper(loc.x,0,.99999f);
  			lox = lox * (this->count - 1);

  			index = GFloorToInt(lox);
  			t = lox - index;
  			c = colors[index] + (blends[index] * t);
    		row[i] = GPixel_PackARGB(RGB_int, GRoundToInt(c.r * RGB_int), GRoundToInt(c.g * RGB_int), GRoundToInt(c.b * RGB_int));
            loc.x += change.x;
	    }
		return;
    }
    if(loc.x == clamper(loc.x, 0, 0.99999f) and loc.x + (change.x * count) == clamper(loc.x + (change.x * count), 0, 0.99999f) and change.x * count < 1 / this->count-1){
        loc.x = loc.x * (count - 1);
        int index = GFloorToInt(loc.x);
        c = colors[index] + blends[index] * loc.x;
        GColor dc = blends[index] * change.x;
      	for (int i = 0; i < count; i++){
      		result = GPixel_PackARGB(to_255_form(c.a, 1), to_255_form(c.r, c.a), to_255_form(c.g, c.a), to_255_form(c.b, c.a));
    		row[i] = result;
            c += dc;
	    }
		return;
    }

        for (int i = 0; i < count; i++){
            lox = clamper(loc.x,0,.99999f);
  			lox = lox * (this->count - 1);

  			index = GFloorToInt(lox);
  			t = lox - index;
  			c = colors[index] + (blends[index] * t);
    		row[i] = GPixel_PackARGB(to_255_form(c.a, 1), to_255_form(c.r, c.a), to_255_form(c.g, c.a), to_255_form(c.b, c.a));
            loc.x += change.x;
	    }
}

GPixel RGradient::getLocColor(float x){

  x = clamper(x,0,.99999f);
  x = x * (count - 1);

  int index = GFloorToInt(x);
  float t = x - index;
  GColor c = colors[index] + (blends[index] * t);

     return GPixel_PackARGB(to_255_form(c.a, 1), to_255_form(c.r, c.a), to_255_form(c.g, c.a), to_255_form(c.b, c.a));
  }


unsigned int RGradient::to_255_form(float c, float a){
    float c255 = c * RGB_number * a;
    return GRoundToInt(c255);
}
