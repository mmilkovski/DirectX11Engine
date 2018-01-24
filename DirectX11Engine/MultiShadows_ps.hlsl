/////////////
// DEFINES //
/////////////
#define NUM_LIGHTS 3

/////////////
// GLOBALS //
/////////////
// texture resource that will be used for rendering the texture on the model
Texture2D depthTextures[3]; // Only pass in the depth shaded textures here 

// allows modifying how pixels are written to the polygon face, for example choosing which to draw. 
SamplerState SampleTypeClamp : register(s0);

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
	float4 vertex_ScrnSpace : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
    float4 vertex_ProjLightSpace[NUM_LIGHTS] : TEXCOORD1;
    float3 lightPos_LS[NUM_LIGHTS] : TEXCOORD4;
};

float4 main(PixelInputType input) : SV_TARGET
{
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float4 lightColor = float4(0,0,0,0);

	// Set the bias value for fixing the floating point precision issues.
	float bias = 0.001f;

	//////////////// SHADOWING LOOP ////////////////
	for(int i = 0; i < NUM_LIGHTS; ++i)
	{
	// Calculate the projected texture coordinates.
	projectTexCoord.x =  input.vertex_ProjLightSpace[i].x / input.vertex_ProjLightSpace[i].w / 2.0f + 0.5f;
	projectTexCoord.y = -input.vertex_ProjLightSpace[i].y / input.vertex_ProjLightSpace[i].w / 2.0f + 0.5f;

	if((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
		depthValue = depthTextures[i].Sample(SampleTypeClamp, projectTexCoord).r;

		// Calculate the depth of the light.
		lightDepthValue = input.vertex_ProjLightSpace[i].z / input.vertex_ProjLightSpace[i].w;

		// Subtract the bias from the lightDepthValue.
		lightDepthValue = lightDepthValue - bias;

		// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
		// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
			if(lightDepthValue < depthValue)
			{
				// Calculate the amount of light on this pixel.
				float lightIntensity = saturate(dot(input.normal, normalize(input.lightPos_LS[i])));

				if(lightIntensity > 0.0f)
				{
					lightColor += (float4(1.0f, 1.0f, 1.0f, 1.0f) * lightIntensity) * .3f; // spotlight
				}
			}
		}
	}

	return saturate(lightColor);
}