#version 150

in vec3 pass_Normal;
in vec3 pass_Color;
in vec3 lightDir;
in vec3 viewDir;

out vec4 out_Color;

vec3 ambientC = pass_Color;
vec3 diffuseC = pass_Color;
vec3 specularC = vec3(1.0, 1.0, 1.0);
vec3 outlineC = vec3(1.0, 0.8, 0.8);
float limit = 0.1;

vec3 ambientL = vec3(0.2, 0.2, 0.2);
vec3 diffuseL = vec3(1, 1, 0.9);
vec3 specularL = vec3(0.5, 0.5, 0.5);
float shininess = 1;


void main() {
    float cos_of_angle_to_viewer = 1 - dot(normalize(pass_Normal), normalize(viewDir));
    vec3 ambient = ambientC * ambientL;

    float lambert = max(dot(pass_Normal, lightDir), 0);
    vec3 diffuse = diffuseC * diffuseL * lambert;

    float spec = 0.0;

    vec3 halfDir;
    if (lambert > 0) {
        halfDir = normalize(viewDir + lightDir);
        float angle = max(dot(pass_Normal, halfDir), 0.0);
        spec = pow(angle, shininess);
    }

    vec3 specular = specularC * spec;
    

    vec3 color = ambient + diffuse; // + specular;

    out_Color = vec4(color, 1.0);
}
