attribute vec4 position;
attribute vec4 normal;
attribute vec4 texcoord;

uniform mat4 transform;
uniform mat4 modelview;
uniform mat3 normal_mat;
uniform vec4 light_pos;
uniform vec3 eye_pos;

varying vec2 texcoord1;
varying vec3 normal1;

varying vec3 light;
varying vec3 view;


void main() {	
	gl_Position = transform * position;
	texcoord1 = texcoord.xy;	

	normal1 = normal_mat * normal.xyz;	

	vec3 p = (modelview * position).xyz;
	light = normalize( light_pos.xyz - p );
	view = normalize( eye_pos - p );
	
}
