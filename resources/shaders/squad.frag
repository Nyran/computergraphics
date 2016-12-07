#version 150

struct Options
{
    bool grayscale;
    bool mirrorx;
    bool mirrory;
    bool gaussblur;
};

uniform sampler2D squad_texture;
uniform Options opts;

in vec2 pass_tex_Coord;

out vec4 out_Color;


void main()
{
    vec2 tex_coord = pass_tex_Coord;
    if(opts.mirrorx) {
        tex_coord.x = 1-tex_coord.x;
    }
    if(opts.mirrory) {
        tex_coord.y = 1-tex_coord.y;
    }
    vec4 color = texture(squad_texture, tex_coord);
    if(opts.gaussblur) {
        vec2 pxsize = pass_tex_Coord / gl_FragCoord.xy;
        vec2 temp;
        
        // color at original tex_coord
        color = 0.25 * texture(squad_texture, tex_coord);

        // colors at edge-connected pixels
        temp = vec2(tex_coord.x + pxsize.x, tex_coord.y);
        color += 0.125 * texture(squad_texture, temp);
        temp = vec2(tex_coord.x - pxsize.x, tex_coord.y);
        color += 0.125 * texture(squad_texture, temp);
        temp = vec2(tex_coord.x, tex_coord.y + pxsize.y);
        color += 0.125 * texture(squad_texture, temp);
        temp = vec2(tex_coord.x, tex_coord.y - pxsize.y);
        color += 0.125 * texture(squad_texture, temp);

        // colors at vertex-only-connected pixels
        temp = vec2(tex_coord.x + pxsize.x, tex_coord.y + pxsize.y);
        color += 0.0615 * texture(squad_texture, temp);
        temp = vec2(tex_coord.x + pxsize.x, tex_coord.y - pxsize.y);
        color += 0.0615 * texture(squad_texture, temp);
        temp = vec2(tex_coord.x - pxsize.x, tex_coord.y + pxsize.y);
        color += 0.0615 * texture(squad_texture, temp);
        temp = vec2(tex_coord.x - pxsize.x, tex_coord.y - pxsize.y);
        color += 0.0615 * texture(squad_texture, temp);
    }
    if(opts.grayscale) {
        float l = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
        color = vec4(l, l, l, 1.0);
    }
    out_Color = color;
}