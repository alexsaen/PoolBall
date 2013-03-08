varying vec3 s1;
varying vec3 pos;

uniform vec4 light_pos;
uniform vec3 eye_pos;

void main(void) {
	float v1 = max( (1.0 - dot(s1.xy, s1.xy))*3 , 0.0);
	v1 *= max(1.0 - s1.z * 0.25,  0.0);
    	gl_FragColor = vec4(0, 0, 0, v1*0.5);

}

