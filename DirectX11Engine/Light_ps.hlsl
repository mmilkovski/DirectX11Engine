////////////////////////////////////////////////////////////////////////////////
// Filename: light_ps
// The pixel shader gets its input from the vertex shader output.
// Msdn note: Pixel shaders can only write to parameters with the 
// SV_Depth and SV_Target system-value semantics.
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
// texture resource that will be used for rendering the texture on the model
//Texture2D shaderTexture;
Texture2D shaderTextures[5];
// allows modifying how pixels are written to the polygon face, for example choosing which to draw. 
SamplerState SampleType;

// hold diffuse color and direction of light
cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float specularPower;
    float4 specularColor;
};

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 viewDirection : TEXCOORD1;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
	//////////// INIT VALUES ////////////////
	// multi texturing data
    float4 color1;
    float4 color2;
    float4 lightColor; //light map
	float4 alphaValue;
    
	// computed blend of textures
	float4 blendColor;

	// lighting data
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float3 reflection;
    float4 specular;
    float4 color;

    float4 bumpMap;
    float3 bumpNormal;

	float gamma = 2.f;

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    //textureColor = shaderTexture.Sample(SampleType, input.tex);
	// Get the pixel color from the first texture.
    color1 = shaderTextures[0].Sample(SampleType, input.tex);

    // Get the pixel color from the second texture.
    color2 = shaderTextures[1].Sample(SampleType, input.tex);

	// Apply lightmap
	lightColor = shaderTextures[2].Sample(SampleType, input.tex);

	// Apply alpha
	alphaValue = shaderTextures[3].Sample(SampleType, input.tex);

	// Get normal value from the bump map
	bumpMap = shaderTextures[4].Sample(SampleType, input.tex);


	/////////////////// NORMAL MAPPING //////////////////
	// Expand the range of the normal value from (0, +1) to (-1, +1).
    bumpMap = (bumpMap * 2.0f) - 1.0f;

	// Change the COORDINATE BASIS of the normal into the space represented by basis vectors tangent, binormal, and normal!
    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);

	// Normalize the resulting bump normal.
    bumpNormal = normalize(bumpNormal);


	/////////////////// BLENDING /////////////////////////
    // Blend the two pixels together and multiply by the gamma value.
    blendColor = (alphaValue * color1) + ((1.0f - alphaValue) * color2) * lightColor /** bumpMap*/ ;//* gamma;
    
    // Saturate the final color.
    blendColor = saturate(blendColor);

    // Set the default output color to the ambient light value for all pixels.
    color = ambientColor;

    // Initialize the specular color.
    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Invert the light direction for calculations.
    lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    lightIntensity = saturate(dot(input.normal, lightDir));
   // lightIntensity = saturate(dot(bumpNormal, lightDir));

	if(lightIntensity > 0.0f)
    {
        // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
        color += (diffuseColor * lightIntensity);

        // Saturate the ambient and diffuse color.
        color = saturate(color);

		// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
        reflection = normalize(2 * lightIntensity * input.normal - lightDir); 

        // Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
        specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
    }

    // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
    color = color * blendColor; //textureColor;

    // Saturate the final light color.
   color = saturate(color + specular);

    return color;
}