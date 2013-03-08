uniform sampler2D 	tex0;
uniform vec4 		color;
varying vec2 		texcoord1;

void main(void) {
	gl_FragColor.a = texture2D(tex0, texcoord1).a * color.a;
	gl_FragColor.rgb = color.rgb;
}

