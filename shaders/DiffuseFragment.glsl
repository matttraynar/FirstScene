#version 410 core

in vec3 fragmentNormal;

out vec4 fragmentColour;

uniform vec4 colour;
uniform vec3 lightPos;
uniform vec4 lightDiffuse;

void main()
{
    //Set the colour to black
    fragmentColour = vec4(0.0);

    //Normalize the two variables
    vec3 N = normalize(fragmentNormal);
    vec3 L = normalize(lightPos);

    //Change the colour of the fragments by adding the:
    //base colour,
    //light diffuse,
    //and the dot of the current normal and the light's normal
    fragmentColour += colour*lightDiffuse*dot(L,N);
}
