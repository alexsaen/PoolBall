#ifndef TRACKBALL_H
#define TRACKBALL_H

#include "math3d.h"

class Trackball {
public:
				Trackball();
    void        reshape(int width, int height);
    void        mouseDown(int x, int y);
	void        mouseMove(int x, int y);
    void        mouseUp();
    mat4		transform();

private:
	int         width, height;
	float       angle;
	vec3		axis, startpos;
	mat4		matrix;
	bool        tracking;
	vec3		pointToVector(int x, int y);
};


#endif
