#version 150
#extension GL_ARB_explicit_attrib_location : require

layout (location = 0) in vec2 in_Position;

out vec2 pass_tex_Coord;

void main()
{
    vec4 test = vec4(0,0,0,0).xyzw;
    gl_Position = vec4(in_Position.x, in_Position.y, 0.0, 1.0); 
    pass_tex_Coord = vec2(normalize(in_Position.x+1), normalize(in_Position.y+1));
} 