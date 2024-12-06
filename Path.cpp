//
// Created by raysh on 10/8/2024.
//
#include "./include/GPath.h"
#include "./include/GPathBuilder.h"
#include "./include/GPoint.h"
#include <limits>
/**
        const GPoint*    fCurrPt;
        const GPathVerb* fCurrVb;
        const GPathVerb* fStopVb;
*/
	GPoint calc_quad(GPoint A, GPoint B, GPoint C, float t){
          float p_x = (A.x * (1.0f-t) * (1.0f-t)) + (2.0f * B.x * t * (1.0f-t)) + (C.x * t * t);
          float p_y = (A.y * (1.0f-t) * (1.0f-t)) + (2.0f * B.y * t * (1.0f-t)) + (C.y * t * t);
          return {p_x,p_y};
          }
    GPoint calc_cubic(GPoint A, GPoint B, GPoint C, GPoint D, float t){
      float p_x = (A.x * std::pow((1.0f-t),3.0f)) + (3.0f * B.x * t * std::pow((1.0f-t),2.0f)) + (3.0f * C.x * std::pow(t,2.0f) * (1.0f-t)) + (D.x * std::pow(t,3.0f));
      float p_y = (A.y * std::pow((1.0f-t),3.0f)) + (3.0f * B.y * t * std::pow((1.0f-t),2.0f)) + (3.0f * C.y * std::pow(t,2.0f) * (1.0f-t)) + (D.y * std::pow(t,3.0f));
      return {p_x,p_y};
      }

	int computeTightQuad(GPoint rtn[], const GPoint pts[]) {
          GPoint A = {pts[0].x, pts[0].y};
          GPoint B = {pts[1].x, pts[1].y};
          GPoint C = {pts[2].x, pts[2].y};
		    float solve_t_x = (A.x - B.x) / (A.x + (-2.0f * B.x) + C.x);
            float check_x = (A.x + (-2.0f * B.x) + C.x);
			float solve_t_y = (A.y - B.y) / (A.y + (-2.0f * B.y)  + C.y);
            float check_y = (A.x + (-2.0f * B.x) + C.x);
			int index = 0;



            //might need to include 0 and 1 in the calculation
            if(solve_t_x > 0.0f and solve_t_x < 1.0f and abs(check_x) > 0.0001f) {
              	rtn[index] = calc_quad(pts[0],pts[1],pts[2],solve_t_x);
        		index++;
            }

            if(solve_t_y > 0.0f and solve_t_y < 1.0f and abs(check_y) > 0.0001f) {
              rtn[index] = calc_quad(pts[0],pts[1],pts[2],solve_t_y);
              index++;
             }

             return index;
          }

    float square(float x){
      return x*x;
    }

    float cube(float x){
      return x*x*x;
    }

    float cubic_quadratic_plus(float A, float B, float C, float D){
      return ((-1.0f * A) + (2.0f * B) + (-1.0f * C) + std::sqrt(square(B) + (-1.0f * D * B) + (-1.0f * B * C) + (D* A) + square(C) + (-1.0f * A * C)))
                              / (D + (3.0f * B) + (-1.0f * A) + (-3.0f * C));
    }

    float cubic_quadratic_minus(float A, float B, float C, float D){
      return ((-1.0f * A) + (2.0f * B) + (-1.0f * C) - std::sqrt(square(B) + (-1.0f * D * B) + (-1.0f * B * C) + (D* A) + square(C) + (-1.0f * A * C)))
                              / (D + (3.0f * B) + (-1.0f * A) + (-3.0f * C));
    }

    float cubic_denominator(float A, float B, float C, float D){
      return (D + (3.0f * B) + (-1.0f * A) + (-3.0f * C));
    }

    float cubic_linear(float A, float B, float C){
      return (A - B) / (2.0f * (A + C + (-2.0f * B)));
      }

    float cubic_b(float A, float B, float C){
      return (-1.0f * A) + (2.0f * B) + (-1.0f * C);
      }

	int computeTightCubic(GPoint rtn[],const GPoint pts[]) {
          GPoint A = {pts[0].x, pts[0].y};
          GPoint B = {pts[1].x, pts[1].y};
          GPoint C = {pts[2].x, pts[2].y};
          GPoint D = {pts[3].x, pts[3].y};
          int index = 0;
          float check_x_a = cubic_denominator(A.x,B.x,C.x,D.x);
          float check_y_a = cubic_denominator(A.y,B.y,C.y,D.y);
          float check_x_b = cubic_b(A.x,B.x,C.x);
          float check_y_b = cubic_b(A.y,B.y,C.y);
          float solve_t_x_1;
          float solve_t_x_2;
		  float solve_t_y_1;
          float solve_t_y_2;

          if(abs(check_x_a) < 0.0001f){
          	if(abs(check_x_b) < 0.0001f){
                  solve_t_x_1 = -1.0f;
          		  solve_t_x_2 = -1.0f;
          	} else {
                  solve_t_x_1 = cubic_linear(A.x,B.x,C.x);
          		  solve_t_x_2 = -1.0f;
            }
          } else{
          solve_t_x_1 = cubic_quadratic_plus(A.x,B.x,C.x,D.x);
          solve_t_x_2 = cubic_quadratic_minus(A.x,B.x,C.x,D.x);
            }

           if(abs(check_y_a) < 0.0001f){
            if(abs(check_y_b) < 0.0001f){
                  solve_t_y_1 = -1.0f;
          		  solve_t_y_2 = -1.0f;
          	} else {
                  solve_t_y_1 = cubic_linear(A.y,B.y,C.y);
          		  solve_t_y_2 = -1.0f;
            }
		  } else{
            solve_t_y_1 = cubic_quadratic_plus(A.y,B.y,C.y,D.y);
            solve_t_y_2 = cubic_quadratic_minus(A.y,B.y,C.y,D.y);
            }

            //might need to include 0 and 1 in the calculation
            if(solve_t_x_1 > 0 and solve_t_x_1 < 1) {
              	rtn[index] = calc_cubic(pts[0],pts[1],pts[2],pts[3],solve_t_x_1);
        		index++;
            }

            if(solve_t_x_2 > 0 and solve_t_x_2 < 1) {
              	rtn[index] = calc_cubic(pts[0],pts[1],pts[2],pts[3],solve_t_x_2);
        		index++;
            }

            if(solve_t_y_1 > 0 and solve_t_y_1 < 1) {
              rtn[index] = calc_cubic(pts[0],pts[1],pts[2],pts[3],solve_t_y_1);
              index++;
             }


           if(solve_t_y_2 > 0 and solve_t_y_2 < 1) {
              rtn[index] = calc_cubic(pts[0],pts[1],pts[2],pts[3],solve_t_y_2);
              index++;
             }

          assert(index <= 4);
          return index;
          }

/**
     *  Return the bounds of all of the control-points in the path.
     *
     *  If there are no points, returns an empty rect (all zeros)
     */
    GRect GPath::bounds() const{
    float left_x = std::numeric_limits<float>::max();
    float right_x = -std::numeric_limits<float>::max();
   	float top_y = std::numeric_limits<float>::max();
    float bot_y = -std::numeric_limits<float>::max();

    std::vector<GPoint> points;
    GPoint temp[4];
    GPoint rtn[4];
    int index;
    GPath::Edger edr(*this);
    while (auto v = edr.next(temp)){
      	switch(v.value()){
          case GPathVerb::kMove:
          	points.push_back(temp[0]);
          break;
          case GPathVerb::kLine:
            points.push_back(temp[1]);
          break;
          case GPathVerb::kQuad:
              points.push_back(temp[2]);
            index = computeTightQuad(rtn, temp);
			for (int i = 0; i < index; i++){
            	points.push_back(rtn[i]);
			}
          break;
          case GPathVerb::kCubic:
          points.push_back(temp[3]);

          index = computeTightCubic(rtn, temp);
          for (int i = 0; i < index; i++){
            	points.push_back(rtn[i]);
			}

          /*
          float N = 1000;
          float t = 1 / N;
          float step = 1 / N;
          GPoint A = {temp[0].x, temp[0].y};
          GPoint B = {temp[1].x, temp[1].y};
          GPoint C = {temp[2].x, temp[2].y};
          GPoint D = {temp[3].x, temp[3].y};
          GPoint res;
          for (int i = 0; i < N - 1; i++) {
				res = calc_cubic(A, B, C, D, t);
				points.push_back(res);
                t += step;
          }

           */
          break;
      }
    }

     for (int i = 0; i < points.size(); ++i) {
       if (points[i].x < left_x) {
            left_x = points[i].x;
        }
        if (points[i].x > right_x) {
            right_x = points[i].x;
        }
        if (points[i].y < top_y) {
            top_y = points[i].y;
        }
        if (points[i].y > bot_y) {
            bot_y = points[i].y;
        }
     }

    	if (std::numeric_limits<float>::max() == left_x) {
    		left_x = 0;
    	}
    	if (-std::numeric_limits<float>::max()  == right_x) {
    		right_x = 0;
    	}
    	if (std::numeric_limits<float>::max() == top_y) {
    		top_y = 0;
    	}
    	if (-std::numeric_limits<float>::max() == bot_y) {
    		bot_y = 0;
    	}


     return GRect::LTRB(left_x,top_y,right_x,bot_y);
     }

/**
 *  Append a new contour to this path, made up of the 4 points of the specified rect,
 *  in the specified direction.
 *
 *  In either direction the contour must begin at the top-left corner of the rect.
 */


  //removed optional arg due to error. Might need to readd later
void GPathBuilder::addRect(const GRect& rect, GPathDirection g){
  	if(g == GPathDirection::kCW){
        moveTo(rect.left, rect.top);
        lineTo(rect.right, rect.top);
        lineTo(rect.right, rect.bottom);
        lineTo(rect.left, rect.bottom);
        } else {
        moveTo(rect.left,rect.top);
        lineTo(rect.left, rect.bottom);
        lineTo(rect.right, rect.bottom);
        lineTo(rect.right, rect.top);
        }
  }

/**
 *  Append a new contour to this path with the specified polygon.
 *  Calling this is equivalent to calling moveTo(pts[0]), lineTo(pts[1..count-1]).
 */
void GPathBuilder::addPolygon(const GPoint pts[], int count){
  moveTo(pts[0]);
  for(int i = 1; i < count; i++){
    	lineTo(pts[i]);
    }
  }

// func written by Mike Reed
void tess_circle(GPoint pts[], int count, float cx, float cy, float rad) {
    assert(count >= 3);
    for (int i = 0; i < count; ++i) {
        float angle = i * gFloatPI * 2 / count;
        pts[i] = {cos(angle) * rad + cx, sin(angle) * rad + cy};
    }
}

/**
  *  Append a new contour respecting the Direction. The contour should be an approximate
  *  circle (8 quadratic curves will suffice) with the specified center and radius.
  */
void GPathBuilder::addCircle(GPoint center, float radius, GPathDirection g){
  		GPoint cPts[8];
        tess_circle(cPts, 8, center.x, center.y, radius);
        moveTo(cPts[0].x, cPts[0].y);
        if(g == GPathDirection::kCW){
        //figure out after chop
			GPoint imm = {cPts[0].x,center.y + radius * std::tan(gFloatPI / 8.0f)};
            quadTo(imm, cPts[1]);

            imm = {center.x + radius * std::tan(gFloatPI  / 8.0f),cPts[2].y};
            quadTo(imm, cPts[2]);

            imm = {center.x - radius * std::tan(gFloatPI  / 8.0f),cPts[2].y};
            quadTo(imm, cPts[3]);

            imm = {cPts[4].x,center.y + radius * std::tan(gFloatPI  / 8.0f)};
            quadTo(imm, cPts[4]);


            //2nd half
            imm = {cPts[4].x,center.y - radius * std::tan(gFloatPI  / 8.0f)};
            quadTo(imm, cPts[5]);

            imm = {center.x - radius * std::tan(gFloatPI  / 8.0f),cPts[6].y};
            quadTo(imm, cPts[6]);

            imm = {center.x + radius * std::tan(gFloatPI  / 8.0f),cPts[6].y};
            quadTo(imm, cPts[7]);

            imm = {cPts[0].x,center.y - radius * std::tan(gFloatPI  / 8.0f)};
            quadTo(imm, cPts[0]);


        } else {
			GPoint imm = {cPts[0].x,center.y - radius * std::tan(gFloatPI  / 8.0f)};
            quadTo(imm, cPts[7]);

            imm = {center.x + radius * std::tan(gFloatPI  / 8.0f),cPts[6].y};
            quadTo(imm, cPts[6]);

            imm = {center.x - radius * std::tan(gFloatPI  / 8.0f),cPts[6].y};
            quadTo(imm, cPts[5]);

            imm = {cPts[4].x,center.y - radius * std::tan(gFloatPI  / 8.0f)};
            quadTo(imm, cPts[4]);


            //2nd half
            imm = {cPts[4].x,center.y + radius * std::tan(gFloatPI  / 8.0f)};
            quadTo(imm, cPts[3]);

            imm = {center.x - radius * std::tan(gFloatPI  / 8.0f),cPts[2].y};
            quadTo(imm, cPts[2]);

            imm = {center.x + radius * std::tan(gFloatPI  / 8.0f),cPts[2].y};
            quadTo(imm, cPts[1]);

            imm = {cPts[0].x,center.y + radius * std::tan(gFloatPI / 8.0f)};
            quadTo(imm, cPts[0]);
        }
  }

/**
*  Given 0 < t < 1, subdivide the src[] quadratic bezier at t into two new quadratics in dst[]
*  such that
*  0...t is stored in dst[0..2]
*  t...1 is stored in dst[2..4]
*/
void GPath::ChopQuadAt(const GPoint src[3], GPoint dst[5], float t){
	GPoint AB_point = {(1-t) * src[0].x + t * src[1].x,(1-t) * src[0].y + t * src[1].y };
    GPoint BC_point = {(1-t) * src[1].x + t * src[2].x,(1-t) * src[1].y + t * src[2].y };
 	GPoint ABC_point = {(1-t) * AB_point.x + t * BC_point.x,(1-t) * AB_point.y + t * BC_point.y };
    dst[0] = src[0];
    dst[1] = AB_point;
    dst[2] = ABC_point;
    dst[3] = BC_point;
    dst[4] = src[2];
  }

/**
 *  Given 0 < t < 1, subdivide the src[] cubic bezier at t into two new cubics in dst[]
 *  such that
 *  0...t is stored in dst[0..3]
 *  t...1 is stored in dst[3..6]
 */
void GPath::ChopCubicAt(const GPoint src[4], GPoint dst[7], float t){
    	GPoint AB_point = {(1-t) * src[0].x + t * src[1].x,(1-t) * src[0].y + t * src[1].y };
    	GPoint BC_point = {(1-t) * src[1].x + t * src[2].x,(1-t) * src[1].y + t * src[2].y };
    	GPoint CD_point = {(1-t) * src[2].x + t * src[3].x,(1-t) * src[2].y + t * src[3].y };

    	GPoint ABC_point = {(1-t) * AB_point.x + t * BC_point.x,(1-t) * AB_point.y + t * BC_point.y };
    	GPoint BCD_point = {(1-t) * BC_point.x + t * CD_point.x,(1-t) * BC_point.y + t * CD_point.y };

    	GPoint ABCD_point = {(1-t) * ABC_point.x + t * BCD_point.x,(1-t) * ABC_point.y + t * BCD_point.y };

        dst[0] = src[0];
        dst[1] = AB_point;
        dst[2] = ABC_point;
        dst[3] = ABCD_point;
        dst[4] = BCD_point;
        dst[5] = CD_point;
        dst[6] = src[3];
 }