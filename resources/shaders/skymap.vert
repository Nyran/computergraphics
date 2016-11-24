#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 tex_Coord;
layout(location = 3) in vec3 in_Tangent;

//Matrix Uniforms as specified with glUniform
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out vec2 pass_tex_Coord;

void main(void)
{
	mat3 view_mat_rot = mat3(ViewMatrix);
	gl_Position = (ProjectionMatrix  * mat4(view_mat_rot)) * vec4(in_Position, 1.0);
	pass_tex_Coord = tex_Coord;
}
