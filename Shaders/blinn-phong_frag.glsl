#version 330 core
struct material_t
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
in vec3 viewspace_normal;
in vec3 viewspace_pos;
out vec4 color;
uniform material_t material;
uniform vec3 viewspace_lightPos;

void main() {
    vec3 lightColor = vec3(1, 1, 1);
    vec3 lightDir = normalize(viewspace_lightPos - viewspace_pos);
    vec3 half_vec = (lightDir + normalize(-viewspace_pos)) / 2;
    vec3 ambient = lightColor * 0.2f;
    vec3 diffuse = lightColor * max(dot(viewspace_normal, lightDir), 0) * material.diffuse;

    vec3 specular = lightColor * pow(max(0, dot(half_vec, viewspace_normal)), 32);

//    color = vec4(ambient + diffuse + specular , 1);
    color=vec4(material.diffuse,1);
}



//stest