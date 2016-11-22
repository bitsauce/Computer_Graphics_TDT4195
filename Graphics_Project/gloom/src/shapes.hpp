#pragma once

#include <math.h>
#include "program.hpp"
#include "gloom/gloom.hpp"

enum Shape
{
	SHAPE_NONE,
	TRIANGLE,
	PARALLELOGRAM,
	ARROW,
	HEXAGON_WHITE,
	HEXAGON_BLACK,
	STAR,
	CAKE,
	SHAPE_COUNT
};

GLuint createShape(const Shape shape);
GLuint createBoard(const bool startWithBlue);
GLuint createMoveMarker();