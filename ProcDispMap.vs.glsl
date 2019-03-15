// FILE: ProcDispMap.vs.glsl
// VERTEX SHADER

#version 430 core

//============================================================================
// Vertex attributes.
//============================================================================
layout (location = 0) in vec3 vPosition;  // Vertex position in object space.
layout (location = 1) in vec3 vNormal;    // Vertex normal in object space.
layout (location = 2) in vec2 vTexCoord;  // 2D texture coordinates at vertex.

//============================================================================
// Output to TCS.
//============================================================================
out vec3 vs_Normal;     // Vertex normal in object space.
out vec2 vs_TexCoord;   // 2D texture coordinates at vertex.


void main()
{
    vs_Normal = vNormal;
    vs_TexCoord = vTexCoord;
    gl_Position = vec4(vPosition, 1.0);
}
