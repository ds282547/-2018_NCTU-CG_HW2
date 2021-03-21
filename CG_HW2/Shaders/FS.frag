#version 400
#define LIGHTCOUNT 2

struct LightInfo
{
	vec3 Position;	
	vec3 La;		
	vec3 Ld;		
	vec3 Ls;		
};
 
struct MaterialInfo
{
	vec3 Ka;			
	vec3 Kd;			
	vec3 Ks;			
	float Shininess;	
};

in Data
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoord;
	vec3 dissolving_color;
	float dissolving_threshold;
} data;

flat in uint draw_mode;

out vec4 outColor;

uniform mat3 NM;
uniform mat4 MV;
uniform int light_count;
uniform sampler2D Tex1;
uniform sampler2D Tex2;
uniform sampler2D Tex3;
uniform LightInfo Light[LIGHTCOUNT];
uniform MaterialInfo Material;

void light( int lightIndex, vec3 position, vec3 norm, out vec3 ambient, out vec3 diffuse, out vec3 spec )
{
	vec3 n = normalize( norm );
	vec3 s = normalize( position - Light[lightIndex].Position );
	vec3 v = normalize( position );
	vec3 r = reflect( -s, n );

	float dis = distance(position,Light[lightIndex].Position);
	float intensity = 1/sqrt(dis);
 
	ambient = Light[lightIndex].La * Material.Ka;
 
	float sDotN = max( dot( s, n ), 0.0 );
	diffuse = Light[lightIndex].Ld * Material.Kd * sDotN * intensity;


 
	spec = Light[lightIndex].Ls * Material.Ks * pow( max( dot(r,v) , 0.0 ), Material.Shininess ) * intensity; 
}
void phone(){
	vec3 ambientSum = vec3(0);
	vec3 diffuseSum = vec3(0);
	vec3 specSum = vec3(0);
	vec3 ambient, diffuse, spec;

	for( int i=0; i<light_count; ++i )
	{
		light( i, data.Position, -data.Normal, ambient, diffuse, spec );
		ambientSum += ambient;
		diffuseSum += diffuse;
		specSum += spec;
	}

	ambientSum /= light_count;
 
	vec4 texColor = texture(Tex1, data.TexCoord);
	outColor =  vec4( ambientSum + diffuseSum, 1 ) *  texColor + vec4( specSum, 1 );
}
void dissolve() {
	vec4 dissolve_map = texture(Tex2, data.TexCoord);
	vec4 texColor = texture(Tex1, data.TexCoord);
	const float edge = 0.08;
	if(dissolve_map.r < data.dissolving_threshold)
        discard;    
	
	float dis = dissolve_map.r - data.dissolving_threshold;
	
	if (dis < edge) {
		float f = (dis - edge/2) / (edge/2);
		float m = f<0.0 ? 0.0 : f ;
		outColor = m * texColor + (1-abs(f)) * vec4( data.dissolving_color, 1);
	} else
		outColor = texColor;
}
void ramp() {

	float dis = distance(data.Position,Light[0].Position);
	float intensity = 1/sqrt(dis);

	vec3 lightDir = normalize( Light[0].Position - data.Position );
	float rampCoord = dot( lightDir, data.Normal ) * 0.45 + 0.5;
	outColor = (texture(Tex3, vec2(rampCoord,0)) * 1.2 ) * texture(Tex1, data.TexCoord) *  intensity;

}
void main() {
	switch(draw_mode) {
		case 0:
			phone();
			break;
		case 1:
			dissolve();
			break;
		case 2:
			ramp();
			break;

	}
}