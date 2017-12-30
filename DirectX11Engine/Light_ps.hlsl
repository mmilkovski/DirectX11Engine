////////////////////////////////////////////////////////////////////////////////
// Filename: light_ps
// The pixel shader gets its input from the vertex shader output.
// Msdn note: Pixel shaders can only write to parameters with the 
// SV_Depth and SV_Target system-value semantics.
////////////////////////////////////////////////////////////////////////////////

/////////////
// DEFINES //
/////////////
//#define NUM_LIGHTS 4
#define NUM_LIGHTS 3

/////////////
// GLOBALS //
/////////////
// texture resource that will be used for rendering the texture on the model
Texture2D shaderTextures[9];
// allows modifying how pixels are written to the polygon face, for example choosing which to draw. 
SamplerState SampleType;

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap  : register(s1);
SamplerComparisonState SampleTypeComp  : register(s2);

//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer LightBuffer:register(b0) //@TODO: register w same number as in class
{
    float4 ambientColor;
    float3 lightDirection;
    float specularPower;
    float4 specularColor;
	float4 diffuseCols[NUM_LIGHTS];
};

// value set here will be between 0 and 1.
cbuffer TranslationBuffer:register(b1) //was 2
{
    float textureTranslation; //@NOTE = hlsl automatically pads floats for you
};

// for alpha blending textures
cbuffer TransparentBuffer:register(b2) // was 3
{
    float blendAmount;
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
    float4 lightViewPositions[NUM_LIGHTS] : TEXCOORD6;
    float3 lightPositions[NUM_LIGHTS] : TEXCOORD9;
	float fogFactor : FOG;
};

float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
	float bias;
    float4 color;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
    float lightIntensity;
	float4 textureColor;


	// Set the bias value for fixing the floating point precision issues.
	//bias = 0.001f;
	bias = 0.0001f;

	// Set the default output color to the ambient light value for all pixels.
    color = /*float4(0.15f,0.15f,0.15f,0.f);*/ambientColor;

	////////////////////////////////////////////
	//////////////// LIGHT LOOP ////////////////
	////////////////////////////////////////////
	// Calculate the projected texture coordinates.
	for(int i = 0; i < NUM_LIGHTS; ++i)
	{
	projectTexCoord.x =  input.lightViewPositions[i].x / input.lightViewPositions[i].w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightViewPositions[i].y / input.lightViewPositions[i].w / 2.0f + 0.5f;

	if((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
		depthValue = shaderTextures[6 + i].Sample(SampleTypeClamp, projectTexCoord).r;
		// get the soft shadow edge
		float lightVisibility = shaderTextures[6 + i].SampleCmp(SampleTypeComp, projectTexCoord*0.999, .8f, -8 ).r;
		float lightVisibility2 = shaderTextures[6 + i].SampleCmp(SampleTypeComp, projectTexCoord*1.001, .8f, -2 ).r;
		float lightVisibility3 = shaderTextures[6 + i].SampleCmp(SampleTypeComp, projectTexCoord*1.002, .8f, 5 ).r;

		// Calculate the depth of the light.
		lightDepthValue = input.lightViewPositions[i].z / input.lightViewPositions[i].w;

		// Subtract the bias from the lightDepthValue.
		lightDepthValue = lightDepthValue - bias;

		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
			if(lightDepthValue < depthValue)
			{
				// Calculate the amount of light on this pixel.
				//lightIntensity = saturate(dot(input.normal, input.lightPositions[i]));
				lightIntensity = saturate(dot(input.normal, normalize(input.lightPositions[i])))/**lightVisibility*lightVisibility2*lightVisibility3*/;

				if(lightIntensity > 0.0f)
				{
					// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
					color += (diffuseCols[i] * lightIntensity * 0.25f);
				}
			}
			//else // shadow falloff here
			//{
			//	float4 shadowCol = (1,1,1,1);
			//	float shadowIntensity = saturate(length(input.lightPositions[i])*0.038);	
			//	color += shadowCol * shadowIntensity*shadowIntensity*shadowIntensity;
			//}
		}
	}

    // Saturate the final light color.
    color = saturate(color);

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTextures[0].Sample(SampleTypeWrap, input.tex);

	// Combine the light and texture color.
	color = color * textureColor;

    return color;
}



//////////////////////////////////////////////////////////////////////////////////
//// Pixel Shader
//////////////////////////////////////////////////////////////////////////////////
//float4 LightPixelShader(PixelInputType input) : SV_TARGET
//{
//	//////////// INIT VALUES ////////////////
//	// multi texturing data
//    float4 color1;
//    float4 color2;
//    float4 lightColor; //light map
//	float4 alphaValue;
//    float4 fogColor = float4(0.5f, 0.3f, 0.3f, 1.0f);
    
//	// computed blend of textures
//	float4 blendColor;

//	// lighting data
//    float4 textureColor;
//    float3 lightDir;
//    float lightIntensity;
//    float3 reflection;
//    float4 specular;
//	float4 specularIntensity;
//    float4 color;

//    float4 bumpMap;
//    float3 bumpNormal;

//	float gamma = 1.2f;
	
//	////////// POINT LIGHTS ////////////
//	float lightIntensity1, lightIntensity2, lightIntensity3, lightIntensity4;
//	float4 lightColor1, lightColor2, lightColor3, lightColor4;
	
//	// Calculate the different amounts of light on this pixel based on the positions of the lights.
//    lightIntensity1 = saturate(dot(input.normal, input.lightPos1));
//    lightIntensity2 = saturate(dot(input.normal, input.lightPos2));
//    lightIntensity3 = saturate(dot(input.normal, input.lightPos3));
//    lightIntensity4 = saturate(dot(input.normal, input.lightPos4));

//	// Determine the diffuse color amount of each of the four lights.
//    lightColor1 = diffuseColors[0] * lightIntensity1;
//    lightColor2 = diffuseColors[1] * lightIntensity2;
//    lightColor3 = diffuseColors[2] * lightIntensity3;
//    lightColor4 = diffuseColors[3] * lightIntensity4;
//	float totalLightColor = saturate(lightColor1 + lightColor2 + lightColor3 + lightColor4);

//	// Translate the position where we sample the pixel from.
//    input.tex.x += textureTranslation;

//    // Sample the pixel color from the textures using the sampler at this texture coordinate location.
//	color1 = shaderTextures[0].Sample(SampleType, input.tex);

//    // Get the pixel color from the second texture.
//    color2 = shaderTextures[1].Sample(SampleType, input.tex);

//	// Apply lightmap
//	lightColor = shaderTextures[2].Sample(SampleType, input.tex);

//	// Apply alpha
//	alphaValue = shaderTextures[3].Sample(SampleType, input.tex);

//	// Get normal value from the bump map
//	bumpMap = shaderTextures[4].Sample(SampleType, input.tex);

//	/////////////////// NORMAL MAPPING //////////////////
//	// Expand the range of the normal value from (0, +1) to (-1, +1).
//    bumpMap = (bumpMap * 2.0f) - 1.0f;

//	// Change the COORDINATE BASIS of the normal into the space represented by basis vectors tangent, binormal, and normal!
//    bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);

//	// Normalize the resulting bump normal.
//    bumpNormal = normalize(bumpNormal);

//	/////////////////// BLENDING /////////////////////////
//    // Blend the two pixels together and multiply by the gamma value.
//    blendColor =  saturate((alphaValue * color1) + ((1.0f - alphaValue) * color2) /** lightColor*/ * gamma);

//    // Set the default output color to the ambient light value for all pixels.
//    color = ambientColor;

//    // Initialize the specular color.
//    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

//	// Invert the light direction for calculations.
//    lightDir = -lightDirection;

//    // Calculate the amount of light on this pixel.
//    lightIntensity = saturate(dot(bumpNormal, lightDir));

//	if(lightIntensity > 0.0f)
//    {
//	    // Sample the pixel from the specular map texture.
//        specularIntensity = shaderTextures[5].Sample(SampleType, input.tex);

//        // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
//        color += (diffuseColor * lightIntensity); //@TODO: TEST
//		//color += totalLightColor;

//        // Saturate the ambient and diffuse color.
//        color = saturate(color);

//		// Calculate the reflection vector based on the light intensity, normal vector, and light direction.
//        reflection = normalize(2 * lightIntensity * bumpNormal - lightDir); 

//        // Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
//        specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);

//		// Use the specular map to determine the intensity of specular light at this pixel.
//       // specular = specular * specularIntensity;
//    }

//    // Multiply the texture pixel and the final diffuse color to get the final pixel color result.
//    color = color * blendColor; //textureColor;

//    // Saturate the final light color.
//    color = saturate(color + specular);

//	/////////////// TRANSPARENCY /////////////////////
//    // Set the alpha value of this pixel to the blending amount to create the alpha blending effect.
//    color.a = blendAmount;

//    //return color;

//	//////////////////////
//	/// SHADOW MAPPING ///
//	//////////////////////

//	color = ambientColor;
//    float bias = 0.001f; // for fixing floating point precision issues
//    float2 projectTexCoord;
//	projectTexCoord.x =  input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
//    projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;
    
//	    // Determine if the projected coordinates are in the 0 to 1 range.  If so then this pixel is in the view of the light.
//    if((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
//    {
//	 // Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
//     float depthValue = shaderTextures[6].Sample(SampleTypeClamp, projectTexCoord).r;

//        // Calculate the depth of the light.
//        float lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;

//        // Subtract the bias from the lightDepthValue.
//        lightDepthValue = lightDepthValue - bias;	

//		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
//        // If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
//        if(lightDepthValue < depthValue)
//        {
//		    // Calculate the amount of light on this pixel.
//            lightIntensity = saturate(dot(input.normal, input.lightShadowPos));

//            if(lightIntensity > 0.0f)
//            {
//                // Determine the final diffuse color based on the diffuse color and the amount of light intensity.
//                color += (diffuseColor * lightIntensity);

//                // Saturate the final light color.
//                color = saturate(color);
//            }
//        }
//	}

//	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
//    textureColor = shaderTextures[0].Sample(SampleTypeWrap, input.tex);

//    // Combine the light and texture color.
//    color = color * textureColor;

//    return color;
//}