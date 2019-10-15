// FILE: ProcDispMap.tcs.glsl
// TESSELLATION CONTROL SHADER (TCS)
//============================================================================
// STUDENT NAME: Kuang Hao
//============================================================================

#version 430 core

layout (vertices = 3) out;  // Each patch is a triangle patch.

//============================================================================
// Input from Vertex Shader.
//============================================================================
in vec3 vs_Normal[];    // Normals in object space. 
in vec2 vs_TexCoord[];  // Texture coordinates.

//============================================================================
// Output to TES.
//============================================================================
out vec3 tcs_Normal[];   // Normals in object space. 
out vec2 tcs_TexCoord[]; // Texture coordinates.

//============================================================================
// Other Uniform variables.
//============================================================================
uniform mat4 ModelViewProjMatrix; // ModelView matrix * Projection matrix.
uniform float ViewportWidth;      // Viewport width in pixels.
uniform float ViewportHeight;     // Viewport Height in pixels.

//============================================================================
// TessEdgePixelLength is the desired pixel length of each short edge produced by the 
// tessellation on the outer edges of the patch. 
// This is used to compute the tessellation level of each edge of the patch.
// Each patch edge is first projected onto viewport to find its pixel length. Then the 
// tessellation level for that patch edge is the edge length divided by TessEdgePixelLength.
//============================================================================
uniform float TessEdgePixelLength = 20.0;  


void main()
{
    // Pass along the vertex position unmodified.
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tcs_Normal[gl_InvocationID] = vs_Normal[gl_InvocationID];
    tcs_TexCoord[gl_InvocationID] = vs_TexCoord[gl_InvocationID];

    // Let only one of the 3 TCS invocations compute the tessellation levels.
    if (gl_InvocationID == 0)
    {
        /////////////////////////////////////////////////////////////////////////////
        // TASK:
        // * Project the patch vertices (control points) to viewport.
        // * Compute the length of the patch edges in pixels.
        // * Compute the tessellation level for each outer edge (see the description
        //   of TessEdgePixelLength).
        // * The inner tessellation level is the max of those of the 3 outer edges.
        /////////////////////////////////////////////////////////////////////////////

    	// Project the patch vertices (control points) to viewport
    	vec4 temp1 = {ViewportWidth,0, 0, 0};
    	vec4 temp2 = {0, ViewportHeight, 0, 0};
    	vec4 temp3 = {0, 0, 1, 0};
    	vec4 temp4 = {ViewportWidth, ViewportHeight, 1, 1};
    	mat4 mat = {temp1, temp2, temp3, temp4};
    	vec4 p0 = mat * ModelViewProjMatrix * gl_in[0].gl_Position;
    	vec4 p1 = mat * ModelViewProjMatrix * gl_in[1].gl_Position;
    	vec4 p2 = mat * ModelViewProjMatrix * gl_in[2].gl_Position;

    	// Compute the length of the patch edges in pixels
    	float pix = sqrt(ViewportHeight * ViewportHeight + ViewportWidth * ViewportWidth);
    	float pix_length0 = distance(p1, p2) * pix;
    	float pix_length1 = distance(p0, p2) * pix;
    	float pix_length2 = distance(p0, p1) * pix;

    	// Compute the tessellation level for each outer edge
    	gl_TessLevelOuter[0] = pix_length0 / TessEdgePixelLength;
    	gl_TessLevelOuter[1] = pix_length1 / TessEdgePixelLength;
    	gl_TessLevelOuter[2] = pix_length2 / TessEdgePixelLength;

    	// The inner tessellation level is the max of those of the 3 outer edges
    	gl_TessLevelInner[0] = max(max(gl_TessLevelOuter[0], gl_TessLevelOuter[1]), gl_TessLevelOuter[2]);
    }
}
