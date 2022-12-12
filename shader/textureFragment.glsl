#version 150 core

in vec2 UV;

out vec4 newColor;

uniform sampler2D renderedTexture;

void main(){
    vec4 color = texture(renderedTexture, UV);
    newColor = color;
}