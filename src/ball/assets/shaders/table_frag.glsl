varying vec3 normal1;
varying vec3 pos;

uniform vec4 light_pos;
uniform vec3 eye_pos;

float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898,78.233))) * 43758.5453);
}

void main(void) {
	vec3 l = normalize( light_pos.xyz - pos );
	vec3 v = normalize( eye_pos - pos );
	vec3 n = normalize( normal1 );


    	const vec4  diffColor = vec4 ( 0.0, 1.0, 0.0, 1.0 );
    	const float k         = 0.9;

    	float d1 = pow ( max ( dot ( n, l ), 0.0 ), 1.0 + k );
    	float d2 = pow ( 1.1 - dot ( n, v ), 1.0 - k );

    	gl_FragColor = diffColor * (d1 * d2 + rand( pos.xy )*0.02 +0.01 );

}

