#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;

//Matrix Uniforms as specified with glUniform
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;
uniform vec3 Color;

out vec3 pass_Normal;
out vec3 pass_Color;
out vec3 lightDir;
out vec3 viewDir;


vec3 lightPos = vec3(0.0, 0.0, 0.0);

void main(void)
{
	vec3 light = (ViewMatrix * vec4(lightPos, 1.0)).xyz;
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	pass_Normal = normalize(NormalMatrix * vec4(in_Normal, 0.0)).xyz;
	// hopefully position of vertex in world space
	vec3 temp = ((ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0)).xyz;
	viewDir = normalize(-temp);
	lightDir = normalize(light - temp);
	pass_Color = Color;
}
