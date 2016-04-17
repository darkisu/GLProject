#version 330 core
in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;


out vec4 color;

uniform sampler2D texture_diffuse1;
uniform vec3 LightPos;
uniform vec3 LightColor;
uniform vec3 ViewPos;
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;

void main()
{    
    vec4 ObjectColor = texture(texture_diffuse1, TexCoords);

    //ambient
    vec3 ambient = 0 * LightColor;      //tuned for sponza.obj
    //vec3 ambient = Ka * LightColor;  

    //diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * 1.2 * LightColor;      //tuned for sponza.obj
    //vec3 diffuse = diff * Kd * LightColor;


    // Specular
    vec3 viewDir = normalize(ViewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = Ks * spec * LightColor;  

    vec3 result = (ambient + diffuse + specular) * vec3(ObjectColor);
    color =  vec4(result, 1.0f);
}