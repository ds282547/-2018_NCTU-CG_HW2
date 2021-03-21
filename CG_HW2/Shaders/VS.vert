#version 400


layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoord;

out Data
{
	vec3 Position;
	vec3 Normal;
	vec2 TexCoord;
	vec3 dissolving_color;
	float dissolving_threshold;
} data;

flat out uint draw_mode;

uniform mat3 NM;
uniform mat4 MVP;
uniform mat4 MV;

uniform vec3 dissolving_color;
uniform float dissolving_threshold;
uniform uint _draw_mode;

mat3 mat3_emu(mat4 m4) {
  return mat3(
      m4[0][0], m4[0][1], m4[0][2],
      m4[1][0], m4[1][1], m4[1][2],
      m4[2][0], m4[2][1], m4[2][2]);
}

void main() {
	//NM = transpose(inverse(mat3_emu(MV)));
	gl_Position = MVP * vec4(Position,1.0) ;//gl_position is the preserved vec4 variable defining the vertex position on screen after vertex shader
	data.TexCoord = TexCoord;
	data.Normal = normalize(NM * Normal);
	data.Position = vec3( MV * vec4( Position, 1 ) );
	data.dissolving_color = dissolving_color;
	data.dissolving_threshold = dissolving_threshold;
	draw_mode = _draw_mode;
}