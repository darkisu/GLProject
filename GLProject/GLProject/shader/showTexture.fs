#version 330 core
in vec3 FragPos;
in vec2 UV;

out vec4 color;

uniform sampler2D renderedTexture;

void main(){
    color = vec4(1.0f,0.0f,0.0f,0.0f);//texture( renderedTexture, UV);
}