#version 150

in vec3 pass_Normal;
in vec3 pass_Color;
in vec3 lightDir;
in vec3 viewDir;

out vec4 out_Color;

vec3 ambientC = pass_Color;
vec3 diffuseC = pass_Color;
vec3 specularC = vec3(1.0, 1.0, 1.0);

vec3 ambientL = vec3(0.2, 0.2, 0.2);
vec3 diffuseL = vec3(1, 1, 0.9);
vec3 specularL = vec3(0.5, 0.5, 0.5);
float shininess = 16;

void graduate(inout float value) {
    if(value > 0.5) {
        value = 1.0;
    } else if(value > 0.2) {
        value = 0.6;
    } else if(value > 0) {
        value = 0.3;
    } else {
        value = 0;
    }
}

void main() {
    float cos_of_angle_to_viewer = dot(normalize(pass_Normal), normalize(viewDir));
    vec3 color;

    if(cos_of_angle_to_viewer < 0.3) {
        color = vec3(0.8, 0.8, 0.8);
    } else {
        vec3 ambient = ambientC * ambientL;

        float lambert = max(dot(pass_Normal, lightDir), 0);
        graduate(lambert);
        vec3 diffuse = diffuseC * diffuseL * lambert;

        float spec = 0.0;

        vec3 halfDir;
        if (lambert > 0) {
            halfDir = normalize(viewDir + lightDir);
            float angle = max(dot(pass_Normal, halfDir), 0.0);
            spec = pow(angle, shininess);
            spec = round(spec);
        }

        vec3 specular = specularC * spec;

        color = ambient + diffuse + specular;
    }

    out_Color = vec4(color, 1.0);
}
