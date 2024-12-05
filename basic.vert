#version 330 core

layout(location = 0) in vec2 inPos;
layout(location = 1) in vec3 inCol;
out vec3 chCol;

uniform bool isBlue; 
uniform vec2 uTranslation;

void main()
{
    vec2 translatedPos = inPos + uTranslation;
    gl_Position = vec4(translatedPos, 0.0, 1.0);
    //gl_Position = vec4(inPos, 0.0, 1.0); 
    chCol = isBlue ? vec3(0.0,0.0, 1.0) : vec3(1.0, 1.0, 0.0); 
}