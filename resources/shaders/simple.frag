#version 150

uniform sampler2D ColorTex;
uniform sampler2D NormalTex;

in vec3 pass_Normal;
in vec3 pass_Color;
in vec3 lightDir;
in vec3 viewDir;
in vec2 pass_tex_Coord;
in vec3 pass_Tangent;

out vec4 out_Color;

vec4 tex_Color = texture(ColorTex, pass_tex_Coord);
vec4 normal_Color = texture(NormalTex, pass_tex_Coord);

vec4 ambientC = normalize(tex_Color); //pass_Color;
vec4 diffuseC = normalize(tex_Color); //pass_Color;
vec4 specularC = vec4(1.0, 1.0, 1.0, 1.0);

vec4 ambientL = vec4(0.3, 0.3, 0.3, 1.0);
vec4 diffuseL = vec4(1, 1, 0.9, 1.0);
vec4 specularL = vec4(0.5, 0.5, 0.5, 1.0);
float shininess = 16;


void main() {
    vec3 bi_Tangent = normalize(cross(pass_Normal, pass_Tangent));
    mat3 tangent_space_mat = mat3(pass_Tangent, bi_Tangent, pass_Normal);
    vec3 normal = normalize(tangent_space_mat * normal_Color.rgb);

    vec4 ambient = ambientC * ambientL;

    float lambert = max(dot(normal, lightDir), 0);
    vec4 diffuse = diffuseC * diffuseL * lambert;

    float spec = 0.0;

    if (lambert > 0) {
        vec3 halfDir = normalize(viewDir + lightDir);
        float angle = max(dot(normal, halfDir), 0.0);
        spec = pow(angle, shininess);
    }

    vec4 specular = specularC * spec;
    

    vec4 color = ambient + diffuse + specular;

    out_Color = color; //vec4(color, 1.0);
}
