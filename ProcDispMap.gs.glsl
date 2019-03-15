// FILE: ProcDispMap.gs.glsl
// GEOMETRY SHADER

// This geometry shader is here only to facilitate the drawing of 
// wireframe in the fragment shader.

#version 430 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

//============================================================================
// Input from TES.
//============================================================================
in vec3 tes_Base_ecPosition[];  // Eye-space positions of vertices BEFORE displacement.
in vec3 tes_Base_ecNormal[];    // Eye-space normal vectors at vertices BEFORE displacement.
in vec2 tes_TexCoord[];         // Texture coordinates of vertices.
in vec3 tes_ecPosition[];       // Eye-space positions of vertices AFTER displacement.

//============================================================================
// Output to Fragment Shader.
//============================================================================
out vec3 Base_ecPosition;   // Eye-space position of vertex BEFORE displacement.
out vec3 Base_ecNormal;     // Eye-space normal vector at vertex BEFORE displacement.
out vec2 TexCoord;          // Texture coordinates of vertex.
out vec3 ecPosition;        // Eye-space positions of vertices AFTER displacement.
out vec3 VertexWeights;     // For drawing wireframe.


const vec3 VERTEX_WEIGHTS[3] = { vec3(1, 0, 0), vec3(0, 1, 0), vec3(0, 0, 1) };


void main()
{
    for (int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
        Base_ecPosition = tes_Base_ecPosition[i];
        Base_ecNormal = tes_Base_ecNormal[i];
        TexCoord = tes_TexCoord[i];
        ecPosition = tes_ecPosition[i];
        VertexWeights = VERTEX_WEIGHTS[i];
        EmitVertex();
    }
    EndPrimitive();
}
