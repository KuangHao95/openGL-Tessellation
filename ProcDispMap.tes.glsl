// FILE: ProcDispMap.tes.glsl
// TESSELLATION EVALUATION SHADER (TES)
//============================================================================
// STUDENT NAME: Kuang Hao
// MATRIC NO.  : A0191488N
// NUSNET ID   : E0338132
// COMMENTS TO GRADER:
//
//============================================================================

#version 430 core

layout (triangles, fractional_odd_spacing) in;

//============================================================================
// Input from TCS.
//============================================================================
in vec3 tcs_Normal[];    // Normals at patch vertices (control points) in object space.
in vec2 tcs_TexCoord[];  // Texture coordinates at patch vertices (control points).

//============================================================================
// Output to Geometry Shader.
//============================================================================
out vec3 tes_Base_ecPosition;  // Eye-space position of new vertex on the triangle patch BEFORE displacement.
out vec3 tes_Base_ecNormal;    // Eye-space normal vector at new vertex on the triangle patch BEFORE displacement.
out vec2 tes_TexCoord;         // Texture coordinates of new vertex.
out vec3 tes_ecPosition;       // Eye-space position of new vertex AFTER displacement.

//============================================================================
// Other Uniform variables.
//============================================================================
uniform mat4 ViewMatrix;          // View transformation matrix.
uniform mat4 ModelViewMatrix;     // ModelView matrix.
uniform mat4 ModelViewProjMatrix; // ModelView matrix * Projection matrix.
uniform mat3 NormalMatrix;        // For transforming object-space direction vector to eye space.

//============================================================================
// MirrorTileDensity defines the number of hemispherical mirrors across each 
// dimension when the corresponding texture coordinate ranges from 0.0 to 1.0.
//============================================================================
uniform float MirrorTileDensity;  // (0.0, inf)

//============================================================================
// MirrorRadius is the radius of the hemispherical mirror in the "tile space".
// The radius is relative to the tile size, which is considered to be 1.0 x 1.0.
//============================================================================
uniform float MirrorRadius;  // (0.0, 0.5]

//============================================================================
// MirrorRadiusObjectSpace is the radius of the hemispherical mirror in object space.
//============================================================================
uniform float MirrorRadiusObjectSpace;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)                                                   
{                                                                                               
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;   
}                                                                                               
                                                                                                
vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)                                                   
{                                                                                               
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;   
}    

void main(void)
{
    /////////////////////////////////////////////////////////////////////////////
    // TASK:
    // * Use the barycentric coordinates of the new vertex to interpolate
    //   the 3D position, texture coordinates, and normal vector from those at 
    //   the patch vertices (control points).
    // * Determine whether new vertex is in mirror region or not.
    // * If new vertex is in the mirror region, displace it suitably.
    // * Output all the necessary data (in the correct coordinate space).
    /////////////////////////////////////////////////////////////////////////////

	// Interpolate data (BEFORE DISPLACEMENT)
	// Use the barycentric coordinates of the new vertex to interpolate the 3D position
	vec3 mcPos = interpolate3D(gl_in[0].gl_Position.xyz, gl_in[1].gl_Position.xyz, gl_in[2].gl_Position.xyz);
	tes_Base_ecPosition = vec3(ModelViewMatrix * vec4(mcPos, 1.0));
	// Interpolate normal vector
	vec3 mcNorm = interpolate3D(tcs_Normal[0], tcs_Normal[1], tcs_Normal[2]);
	tes_Base_ecNormal = NormalMatrix * mcNorm;
	tes_Base_ecNormal = normalize(tes_Base_ecNormal);
	// Interpolate texture coordinates
	tes_TexCoord = interpolate2D(tcs_TexCoord[0], tcs_TexCoord[1], tcs_TexCoord[2]);

	gl_Position = ModelViewProjMatrix * vec4(mcPos, 1.0);
	
	vec2 c = MirrorTileDensity * tes_TexCoord.st;
    vec2 p = fract( c ) - vec2( 0.5 );
    float sqrDist = dot( p, p );

    // Determine whether new vertex is in mirror region or not
    if ( sqrDist <= MirrorRadius * MirrorRadius ) {
    	// If new vertex is in the mirror region, displace it suitably.
    	//mcNorm = normalize(mcPos);
		// Compute displacement height
		
		float displacement = sqrt(MirrorRadiusObjectSpace * MirrorRadiusObjectSpace 
			- mcPos.x * mcPos.x - mcPos.z * mcPos.z);
		mcPos += mcNorm * displacement;
		tes_ecPosition = vec3(ModelViewMatrix * vec4(mcPos, 1.0));
		gl_Position = ModelViewProjMatrix * vec4(mcPos, 1.0);
    }

}
