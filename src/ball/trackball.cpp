#include "trackball.h"

Trackball::Trackball(): matrix(1.0f), tracking(false), angle(0) { }

void Trackball::reshape(int w, int h) {  
	width=w;  
	height=h; 
}

void Trackball::mouseDown(int x, int y) {
    tracking=true;
	startpos = pointToVector(x, y);
}

void Trackball::mouseUp() {
    tracking=false;
}

mat4 Trackball::transform() {
    if(tracking) {
		if(angle==0)
			return matrix;
        return mat4::get_rotate(-angle, axis) * matrix;
    } 

	if(angle != 0) {
        matrix = mat4::get_rotate(-angle, axis) * matrix;
        angle = 0;
        axis = vec3(0.0f);
    }

    return matrix;
}

vec3 Trackball::pointToVector(int x, int y) {
	vec3 v;
    v.x = (2.0f * x - width) / width;
    v.y = (height - 2.0f * y) / height;
    float d = sqrtf(v.x * v.x + v.y * v.y);
    v.z = cosf(PI_2 * ((d < 1.0f) ? d : 1.0f));
	v.normalize();
	return v;
}

void Trackball::mouseMove(int x, int y) {
    if(!tracking) 
		return;

    vec3 curpos = pointToVector(x, y);
	angle = 180.0f * (curpos - startpos).length();
	axis = cross(curpos, startpos);
}

