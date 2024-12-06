//
// Created by raysh on 9/24/2024.
//


#include "RShader.h"

/**
*  Return a subclass of GShader that draws the specified bitmap and the local matrix.
*  Returns null if the subclass can not be created.
*/
std::shared_ptr<GShader> GCreateBitmapShader(const GBitmap& bm, const GMatrix& localMatrix, GTileMode tMode){
    if (bm.width() <= 0 || bm.height() <= 0){
          return NULL;
          }
GBitmap bitmap = bm;
GMatrix matrix = localMatrix;

  return std::shared_ptr<GShader>(new RShader(bitmap, matrix, tMode));
}

bool RShader::isOpaque() {
  return op;
  }

// The draw calls in GCanvas must call this with the CTM before any calls to shadeSpan().
bool RShader::setContext(const GMatrix& ctm){
  nonstd::optional<GMatrix> temp =(ctm * mtrix).invert();
  if (temp.has_value()){
    inv_ctm = *temp;
    return true;
    } else {
      return false;
      }
  }

  int clamper(int value, int min, int max){
    return value <= min ? min : value >= max ? max : value;
  }

void RShader::shadeRow_mir(int x, int y, int count, GPixel row[]){
  const auto change = inv_ctm.e0();
  GPoint loc = inv_ctm * GPoint{x + 0.5f, y + 0.5f};
  int py;
  int check_x;
  int check_y;
  int lox;

  if (change.y == 0){
    py = abs(GFloorToInt(loc.y) % bmap.height());
    check_y = abs(GFloorToInt(loc.y) % (2 * bmap.height()));
    if(check_y >= bmap.height() - 1){
      py = bmap.height() - 1 - py;
    }
    for (int i = 0; i < count; i++){
      check_x = abs(GFloorToInt(loc.x) % (2 * bmap.width()));
      lox = abs(GFloorToInt(loc.x) % bmap.width());
      if(check_x >= bmap.width() - 1){
        lox = bmap.width() - 1 - lox;
      }
      row[i] = *bmap.getAddr(lox, py);
      loc.x += change.x;
    }
  } else {
    for (int i = 0; i < count; i++){
      py =  abs(GFloorToInt(loc.y) % bmap.height());
      lox = abs(GFloorToInt(loc.x) % bmap.width());
      check_y = abs(GFloorToInt(loc.y) % (2 * bmap.height()));
      check_x = abs(GFloorToInt(loc.x) % (2 * bmap.width()));
      if (check_y >= bmap.height() - 1){
        py = bmap.height() - 1 - py;
      }
      if(check_x >= bmap.width() - 1){
        lox = bmap.width() - 1 - lox;
      }
      row[i] = *bmap.getAddr(lox,py);
      loc += change;
    }
  }
  }

void RShader::shadeRow_rep(int x, int y, int count, GPixel row[]){
  const auto change = inv_ctm.e0();
  GPoint loc = inv_ctm * GPoint{x + 0.5f, y + 0.5f};
  int py;
  int lox;

  if (change.y == 0){
    py = abs(GFloorToInt(loc.y) % bmap.height());
    if (loc.y < 0){
      py = bmap.height() - 1 - abs(py);
    }
    for (int i = 0; i < count; i++){
      lox = GFloorToInt(loc.x) % bmap.width();
      if (loc.x < 0){
        lox = bmap.width() - 1 - abs(lox);
      }
      row[i] = *bmap.getAddr(lox, py);
      loc.x += change.x;
    }
  } else {
    for (int i = 0; i < count; i++){
      lox = GFloorToInt(loc.x) % bmap.width();
      py = GFloorToInt(loc.y) % bmap.height();
      if (loc.x < 0){
        lox = bmap.width() - 1 - abs(lox);
        }
      if (loc.y < 0){
        py = bmap.height() - 1 - abs(py);
      }
      row[i] = *bmap.getAddr(lox, py);
      loc += change;
    }
  }
  }


/**
 *  Given a row of pixels in device space [x, y] ... [x + count - 1, y], return the
 *  corresponding src pixels in row[0...count - 1]. The caller must ensure that row[]
 *  can hold at least [count] entries.
 */
void RShader::shadeRow(int x, int y, int count, GPixel row[]) {

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

    const auto change = inv_ctm.e0();
   	GPoint loc = inv_ctm * GPoint{x + 0.5f, y + 0.5f};
    int py;


    if (change.y == 0){
		py = clamper(GFloorToInt(loc.y),0,bmap.height() - 1);
        for (int i = 0; i < count; i++){
      		row[i] = *bmap.getAddr(clamper(GFloorToInt(loc.x),0,bmap.width() - 1), py);
    		loc.x += change.x;
    	}
	} else {
        for (int i = 0; i < count; i++){
    		row[i] = *bmap.getAddr(clamper(GFloorToInt(loc.x),0,bmap.width() - 1), clamper(GFloorToInt(loc.y),0,bmap.height() - 1));
    		loc += change;
        }
	}
}

    //if (count > vertices.size()){
    //  vertices.reserve(count);
    //}
    //float y_p = y + 0.5f;


	//for (int i = 0; i < count; i++){
    //      vertices[i].x = x + i + 0.5f;
    //      vertices[i].y = y_p;
	//}
    //inv_ctm.mapPoints(vertices.data(),vertices.data(),count);
    //int temp_x;
    //int temp_y;



  bool RShader::checkAlpha(){
    if (isOpaque()){
      return false;
    }

    for (int i = 0; i < bmap.height(); i++){
      GPixel* temp = bmap.getAddr(0, i);
      for (int j = 0; j < bmap.width(); j++){
		if (GPixel_GetA(temp[j]) != 0){
         return false;
        }
      }
    }

    return true;
}

//bool RShader::isAlpha(){
 // return alpha;
 // }


