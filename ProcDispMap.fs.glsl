// FILE: ProcDispMap.fs.glsl
// FRAGMENT SHADER

#version 430 core

//============================================================================
// Input from Geometry Shader.
//============================================================================
in vec3 Base_ecPosition;   // Eye-space position of vertex BEFORE displacement.
in vec3 Base_ecNormal;     // Eye-space normal vector at vertex BEFORE displacement.
in vec2 TexCoord;          // Texture coordinates of vertex.
in vec3 ecPosition;        // Eye-space positions of vertices AFTER displacement.
in vec3 VertexWeights;     // For drawing wireframe.


//============================================================================
// Output to framebuffer.
//============================================================================
layout (location = 0) out vec4 FragColor;


uniform mat4 ViewMatrix;          // View transformation matrix.

//============================================================================
// Light info.
//============================================================================
uniform vec4 LightPosition; // Given in eye space. Can be directional.
uniform vec3 LightAmbient;
uniform vec3 LightDiffuse;
uniform vec3 LightSpecular;

//============================================================================
// Material info.
//============================================================================
//uniform vec3 MatlAmbient;
//uniform vec3 MatlDiffuse;
uniform vec3 MatlSpecular;
uniform float MatlShininess;

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
// Environment cubemap used for skybox and reflection mapping.
//============================================================================
layout (binding = 0) uniform samplerCube EnvMap;

//============================================================================
// The wood texture map whose color is used as the ambient and diffuse 
// material in the lighting computation.
//============================================================================
layout (binding = 1) uniform sampler2D WoodTexMap;

//============================================================================
// Other Uniform variables.
//============================================================================
uniform bool ShowWireframe;
uniform float WireframePixelWidth = 1.0;
uniform vec3 WireframeColor = vec3(0.1, 0.1, 0.1);



/////////////////////////////////////////////////////////////////////////////
// Computes and returns the values of N_dot_L and R_dot_V_pow_n 
// in the Phong Illumination Equation.
/////////////////////////////////////////////////////////////////////////////
void Phong(in vec3 ecPos, in vec3 ecN, out float N_dot_L, out float R_dot_V_pow_n)
{
    vec3 V = normalize( -ecPos );
    vec3 N = normalize( ecN );

    vec3 L;
    if (LightPosition.w == 0.0 )
        L = normalize(LightPosition.xyz);
    else
        L = normalize(LightPosition.xyz - ecPos);

    vec3 R = reflect( -L, N );
    N_dot_L = max( 0.0, dot( N, L ) );
    float R_dot_V = max( 0.0, dot( R, V ) );
    R_dot_V_pow_n = ( R_dot_V == 0.0 )? 0.0 : pow( R_dot_V, MatlShininess );
}



/////////////////////////////////////////////////////////////////////////////
// Computes and returns the tangent and binormal vectors.
/////////////////////////////////////////////////////////////////////////////
void compute_tangent_vectors( vec3 N, vec3 p, vec2 uv, out vec3 T, out vec3 B )
{
    // Please refer to "Followup: Normal Mapping Without Precomputed Tangents" at
    // http://www.thetenthplanet.de/archives/1180

    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );
 
    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    T = normalize( dp2perp * duv1.x + dp1perp * duv2.x );  // Tangent vector
    B = normalize( dp2perp * duv1.y + dp1perp * duv2.y );  // Binormal vector
}



/////////////////////////////////////////////////////////////////////////////
// Computes fragment color on wooden cube with hemisphereical mirrors.
/////////////////////////////////////////////////////////////////////////////
void drawWoodenCube()
{
    vec3 Base_ecNNormal = normalize(Base_ecNormal);

    // Find out whether fragment is in mirror or wood region.
    vec2 c = MirrorTileDensity * TexCoord.st;
    vec2 p = fract( c ) - vec2( 0.5 );
    float sqrDist = dot( p, p );

    if ( sqrDist > MirrorRadius * MirrorRadius ) 
    {
        // In wood region.
        float N_dot_L, R_dot_V_pow_n;
        Phong( Base_ecPosition, Base_ecNNormal, N_dot_L, R_dot_V_pow_n );

        vec3 diffuseAmbientMatl = texture(WoodTexMap, TexCoord.st).rgb;

        FragColor.rgb = LightAmbient * diffuseAmbientMatl +
                        LightDiffuse * diffuseAmbientMatl * N_dot_L +
                        LightSpecular * MatlSpecular * R_dot_V_pow_n;
        FragColor.a = 1.0;
    }
    else 
    {
        // In mirror region.

        // Compute perturbed normal vector.
        vec3 T, B;
        compute_tangent_vectors( Base_ecNNormal, Base_ecPosition, TexCoord.st, T, B );
        float h = sqrt( abs( MirrorRadius * MirrorRadius - sqrDist ) );
        vec3 tanPerturbedNormal = normalize( vec3( p.x, p.y, h ) );
        vec3 ecPerturbedNormal = normalize( tanPerturbedNormal.x * T  +  tanPerturbedNormal.y * B  +  
                                            tanPerturbedNormal.z * Base_ecNNormal );

        vec3 viewVec = normalize( -ecPosition );

        // Compute eye-space view reflection vector.
        vec3 ecReflectVec = reflect( -viewVec, ecPerturbedNormal );

        // View reflection vector transformed to world space.
        vec3 wcReflectVec = transpose(mat3(ViewMatrix)) * ecReflectVec;  

        FragColor = texture(EnvMap, wcReflectVec);
    }
}



/////////////////////////////////////////////////////////////////////////////
// Returns the index of the vector component that is the smallest.
/////////////////////////////////////////////////////////////////////////////
uint findMinIndex(vec3 v)
{
    if ( v[0] <= v[1] && v[0] <= v[2] )
        return 0;
    else if ( v[1] <= v[2] )
        return 1;
    else
        return 2;
}



void main()
{
    drawWoodenCube();

    if (ShowWireframe)
    {
        uint minIdx = findMinIndex(VertexWeights);
        float minWeight = VertexWeights[minIdx];
        float dMinWeight_dx = dFdx(minWeight);
        float dMinWeight_dy = dFdy(minWeight);
        float dMinWeight = length( vec2(dMinWeight_dx, dMinWeight_dy) );

        float lineWidth = WireframePixelWidth * dMinWeight;

        if (minWeight <= lineWidth)
            FragColor.rgb = mix( WireframeColor, FragColor.rgb, smoothstep( 0, lineWidth, minWeight) );
    }
}



/*
// FOR DEBUGGING USE

layout (location = 0) out vec4 FragColor;

void main()
{
    FragColor = vec4(1);
}
*/
