attribute vec4 position;
attribute vec4 texcoord;
uniform mat4 transform;

varying vec2 texcoord1;

void main()
{	
	gl_Position = transform * position;
	texcoord1 = texcoord.xy;	
}
