attribute vec4 position;
attribute vec4 normal;

uniform mat4 transform;

varying vec3 normal1;
varying vec3 pos;


void main()
{	
	gl_Position = transform * position;

	normal1 = normal.xyz;	
	pos = position.xyz;	
}
