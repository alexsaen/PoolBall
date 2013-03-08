attribute vec4 position;
attribute vec4 texcoord;

uniform mat4 transform;

varying vec3 s1;
varying vec3 pos;


void main()
{	
	gl_Position = transform * position;

	s1 = texcoord.xyz;	
	pos = position.xyz;	
}
