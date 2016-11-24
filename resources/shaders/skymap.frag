#version 150

uniform sampler2D ColorTex;

in vec2 pass_tex_Coord;

out vec4 out_Color;

void main() {
    out_Color = 0.5 * texture(ColorTex, pass_tex_Coord);
}
