#version 330 core

in vec3 chCol;  // Ulazna boja sa vertex shadera
out vec4 outCol;  // Izlazna boja

void main()
{
    outCol = vec4(chCol, 1.0);  // Postavljanje boje iz vertex shadera
}