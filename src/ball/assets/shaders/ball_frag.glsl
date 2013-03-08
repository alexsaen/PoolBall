uniform sampler2D 	tex0;
uniform samplerCube	env_map;

varying vec2 texcoord1;
varying vec3 normal1;
varying vec3 light;
varying vec3 view;


void main() {
	const float roughness_val = 0.5;

	vec3 n = normalize(normal1);
	vec3 l = normalize(light);
	vec3 v = normalize(view);
	vec3 r = reflect (-v, n);

	const vec4 diffuse  = texture2D( tex0, texcoord1 );
	const vec4 ambient  = vec4(0.1, 0.1, 0.3, 1.0) * diffuse;
	const vec4 specular = vec4(0.5, 0.5, 0.35, 1.0) * textureCube(env_map, r);
	const vec4 env = textureCube(env_map, r);

   	vec3  h = normalize( v + l );
   	float nl    = max( dot( n, l ), 0.0 );
   	float nv    = max( dot( n, v ), 0.0 );
   	float nh    = max( dot( n, h ), 1.0e-7 );
   	float vh    = max( dot( v, h ), 0.0 );

   	float g = min( 1.0, 2.0 * nh / vh * min(nv, nl) );

   	float nh2 = nh * nh;
   	float nhr = 1.0 / (nh2 * roughness_val * roughness_val);
   	float roughness = exp( (nh2 - 1.0) * nhr ) * nhr / (4.0 * nh2 );

	float fresnel = 0.05 + 0.95 * pow ( 1.0 - max ( nv, 0.0 ), 2.0 );

   	float rs = min(1.0, (fresnel * g * roughness) / (nv * nl + 1.0e-7));
	
	gl_FragColor = (nl + 0.1)  * (diffuse + specular * (rs + 0.1) );

}

