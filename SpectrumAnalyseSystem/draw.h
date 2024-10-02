#pragma once

#include <cairo.h>

// Draw the line with x1, y1, x2, y2, color, line width
void drawLine(cairo_t* cr, float x1, float y1, float x2, float y2);
// Draw the rectangle with left_top_x, left_top_y, right_bottom_x, right_bottom_y
void drawRectangle2P(cairo_t* cr, float lt_x, float lt_y, float rb_x, float rb_y);
// Draw the rectangle with x, y, width, height
void drawRectangle(cairo_t* cr, float x, float y, float width, float height);