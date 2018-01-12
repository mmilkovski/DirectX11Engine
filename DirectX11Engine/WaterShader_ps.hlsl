////////////////////////////////////////////////////////////////////////////////
// Filename: water.ps
////////////////////////////////////////////////////////////////////////////////

/////////////
// GLOBALS //
/////////////
SamplerState SampleType;
//3 textures:
// 1) reflection texture for the scene reflection. 
// 2) refraction texture for the refraction of the scene. 
// 3) normal map texture for simulating water ripples.

Texture2D shaderTextures[3];
//Texture2D reflectionTexture;
//Texture2D refractionTexture;
//Texture2D normalTexture;

cbuffer WaterBuffer
{
    float waterTranslation;
    float reflectRefractScale;
    float2 padding;
};

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
    float4 refractionPosition : TEXCOORD2;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 WaterPixelShader(PixelInputType input) : SV_TARGET
{
    float2 reflectTexCoord;
    float2 refractTexCoord;
    float4 normalMap;
    float3 normal;
    float4 reflectionColor;
    float4 refractionColor;
    float4 color;

    // Move the position the water normal is sampled from to simulate moving water.	
    input.tex.y += waterTranslation;

    // Calculate the projected reflection texture coordinates.
    reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0f + 0.5f;
    reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / 2.0f + 0.5f;
	
    // Calculate the projected refraction texture coordinates.
    refractTexCoord.x = input.refractionPosition.x / input.refractionPosition.w / 2.0f + 0.5f;
    refractTexCoord.y = -input.refractionPosition.y / input.refractionPosition.w / 2.0f + 0.5f;

    // Sample the normal from the normal map texture.
    //normalMap = normalTexture.Sample(SampleType, input.tex);
    normalMap = shaderTextures[2].Sample(SampleType, input.tex);

    // Expand the range of the normal from (0,1) to (-1,+1).
    normal = (normalMap.xyz * 2.0f) - 1.0f;

    // Re-position the texture coordinate sampling position by the normal map value to simulate the rippling wave effect.
    reflectTexCoord = reflectTexCoord + (normal.xy * reflectRefractScale);
    refractTexCoord = refractTexCoord + (normal.xy * reflectRefractScale);

    // Sample the texture pixels from the textures using the updated texture coordinates.
    //reflectionColor = reflectionTexture.Sample(SampleType, reflectTexCoord);
    //refractionColor = refractionTexture.Sample(SampleType, refractTexCoord);
	reflectionColor = shaderTextures[0].Sample(SampleType, reflectTexCoord);
    refractionColor = shaderTextures[1].Sample(SampleType, refractTexCoord);

    // Combine the reflection and refraction results for the final color.
    //color = lerp(reflectionColor, refractionColor, 0.6f);
    color = refractionColor;
	
    return color;
}