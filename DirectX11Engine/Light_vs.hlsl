////////////////////////////////////////////////////////////////////////////////
// Filename: light_vs
// does the actual rendering of models.
// Called for every single vertex in the vertex buffer.
// Processes the vertices from a model and transforms them into view space.
// The vertex shader program will be called by the GPU for each vertex it needs to process.
////////////////////////////////////////////////////////////////////////////////

/////////////
// DEFINES //
/////////////
#define NUM_LIGHTS 3

/////////////
// GLOBALS //
/////////////
struct LightTemplate_VS
{
   	matrix lightViewMatrix;
    matrix lightProjMatrix;
	float4 lightPosition_WS;
};

//////////////////////
// CONSTANT BUFFERS //
//////////////////////
cbuffer MatrixBuffer:register(b0)
{
	matrix cb_worldMatrix;
	matrix cb_viewMatrix;
	matrix cb_projectionMatrix;

	LightTemplate_VS cb_lights[NUM_LIGHTS];
};

// pass in position of camera for reflection
cbuffer CameraBuffer:register(b1)
{
    float3 cb_camPosition_WS; // okay not to be float4?
    float cb_padding;
};

cbuffer FogBuffer:register(b2)
{
    float cb_fogStart_ViewSpace;
    float cb_fogEnd_ViewSpace;
};


//////////////
// TYPEDEFS //
//////////////

/** 
* Must create two different structures here since the semantics are different 
* for vertex and pixel shaders even though the structures are the same otherwise. 
* POSITION, COLOR, and SV_POSITION are semantics that convey to the GPU the use of the variable
*/
struct VertexInputType
{
	float4 vertex_ModelSpace : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct PixelInputType
{
	float4 vertex_ModelSpace : TEXCOORD12;
	float4 vertex_ScrnSpace : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
	float3 viewDirection : TEXCOORD1;
    float4 lightViewPositions[NUM_LIGHTS] : TEXCOORD6;
    float3 lightPositions[NUM_LIGHTS] : TEXCOORD9;
	float fogFactor : FOG; 
};

//// The output of the vertex shader will be sent to the pixel shader.
//////////////////////////////////////////////////////////////////////////////////
//// Vertex Shader
//////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
	//The final output for the vertex shader
	PixelInputType output;

	// Pass through tex coordinates untouched
	output.tex  = input.tex;

	// Pre-calculate vertex position in world space
	float4 vertex_WS = mul(input.vertex_ModelSpace, cb_worldMatrix);

	output.viewDirection = normalize(cb_camPosition_WS.xyz - vertex_WS.xyz);

	// Change the position vector to be 4 units for proper matrix calculations.
	input.vertex_ModelSpace.w = 1.0f;
	output.vertex_ModelSpace = input.vertex_ModelSpace; // store the raw vert position in model space for pixel shader

	// Bring vertex from model space into world, view (camera), then screen space
	float4 vertex_ViewSpace = mul(vertex_WS, cb_viewMatrix);
	output.vertex_ScrnSpace = mul(vertex_ViewSpace, cb_projectionMatrix);

	// Bring normal, tangent, and binormal into world space
    output.normal = normalize(mul(input.normal, (float3x3)cb_worldMatrix));
    output.tangent = normalize(mul(input.tangent, (float3x3)cb_worldMatrix));
    output.binormal = normalize(mul(input.binormal, (float3x3)cb_worldMatrix));

    // Calculate linear fog.
    output.fogFactor = saturate((cb_fogEnd_ViewSpace - vertex_ViewSpace.z) / (cb_fogEnd_ViewSpace - cb_fogStart_ViewSpace));
	
	//... SHADOWING ... //
	for(int i = 0; i< NUM_LIGHTS; ++i)
	{
		// Bring the vertex into light view space, then project it onto the texture
		output.lightViewPositions[i] = mul(vertex_WS, cb_lights[i].lightViewMatrix);
		output.lightViewPositions[i] = mul(output.lightViewPositions[i], cb_lights[i].lightProjMatrix);

		// Calculate light position relative to the vertex in WORLD SPACE
		output.lightPositions[i] = cb_lights[i].lightPosition_WS - vertex_WS.xyz;
	}

	return output;
}

// BACKUP /////
// The output of the vertex shader will be sent to the pixel shader.
////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
//PixelInputType LightVertexShader(VertexInputType input)
//{
//	//The final output for the vertex shader
//	PixelInputType output;

//	// Pass through tex coordinates untouched
//	output.tex  = input.tex;

//	// Pre-calculate vertex position in world space
//	float4 vertexPositionWS = mul(input.vertex_ModelSpace, cb_worldMatrix);

//	output.viewDirection = normalize(cb_camPosition_WS.xyz - vertexPositionWS.xyz);

//	// Change the position vector to be 4 units for proper matrix calculations.
//	input.vertex_ModelSpace.w = 1.0f;
//	output.vertex_ModelSpace = input.vertex_ModelSpace;

//	// Calculate the position of the vertex against the world, view, and projection matrices.
//	// place the vertex in the correct location for rendering in 3D space according to our view and then onto the 2D screen.
//	output.vertex_ScrnSpace = mul(input.vertex_ModelSpace, cb_worldMatrix);
//	//output.rawPosition = input.position;
//	output.vertex_ScrnSpace = mul(output.vertex_ScrnSpace, cb_viewMatrix);
//	output.vertex_ScrnSpace = mul(output.vertex_ScrnSpace, cb_projectionMatrix);

//	// Bring normal, tangent, and binormal into world space
//    output.normal = normalize(mul(input.normal, (float3x3)cb_worldMatrix));
//    output.tangent = normalize(mul(input.tangent, (float3x3)cb_worldMatrix));
//    output.binormal = normalize(mul(input.binormal, (float3x3)cb_worldMatrix));

//    // Calculate the camera position. // is this the same as the following method?
//    float4 cb_camPosition = mul(input.vertex_ModelSpace, cb_worldMatrix);
//    cb_camPosition = mul(cb_camPosition, cb_viewMatrix);
//	//float4 vertexPosViewSpace = mul(vertexPositionWS, cb_viewMatrix); @TODO: decide which way is better

//    // Calculate linear fog.
//    output.fogFactor = saturate((cb_fogEnd_ViewSpace - /*vertexPosViewSpace.z*/cb_camPosition.z) / (cb_fogEnd_ViewSpace - cb_fogStart_ViewSpace));

//	//////////// POINT LIGHTS ////////////////////
//	// Determine the light positions based on the position of the lights and the position of the vertex in the world.
//    //output.lightPos1.xyz = normalize(lightPosition_WS[0].xyz - vertexPositionWS.xyz);
//    //output.lightPos2.xyz = normalize(lightPosition_WS[1].xyz - vertexPositionWS.xyz);
//    //output.lightPos3.xyz = normalize(lightPosition_WS[2].xyz - vertexPositionWS.xyz);
	
//	//... SHADOWING ... ////... SHADOWING ... ////... SHADOWING ... ////... SHADOWING ... ////... SHADOWING ... //
//	for(int i = 0; i< NUM_LIGHTS; ++i)
//	{
//		//// Calculate the position of the vertice as viewed by the light sources.
//		//output.lightViewPositions[i] = mul(input.position, cb_worldMatrix);
//		//output.lightViewPositions[i] = mul(output.lightViewPositions[i], lightcb_viewMatrix[i]);
//		//output.lightViewPositions[i] = mul(output.lightViewPositions[i], lightProjectionMatrix[i]);
//		////output.lightPosition_WSs[i] = normalize(c_lightShadowPos[i].xyz - vertexPositionWS.xyz);

//		//// Calculate the world offset of the light from the vertex
//		//output.lightPosition_WSs[i] = c_lightShadowPos[i].xyz - vertexPositionWS.xyz;

//		// NEW ALL IN ONE LIGHT BUFFER METHOD
//		// Calculate the position of the vertice as viewed by the light sources.
//		output.lightViewPositions[i] = mul(input.vertex_ModelSpace, cb_worldMatrix);
//		output.lightViewPositions[i] = mul(output.lightViewPositions[i], cb_lights[i].lightViewMatrix);
//		output.lightViewPositions[i] = mul(output.lightViewPositions[i], cb_lights[i].lightProjMatrix);
//		//output.lightPosition_WSs[i] = normalize(c_lightShadowPos[i].xyz - vertexPositionWS.xyz);

//		// Calculate the world offset of the light from the vertex
//		output.lightPositions[i] = cb_lights[i].lightPosition_WS - vertexPositionWS.xyz;

//		//output.lightPositions[i] = mul(cb_lights[i].lightPosition_WS, cb_worldMatrix);
//		//output.lightPositions[i] = output.lightPositions[i] - vertexPositionWS.xyz;
//	}

//	return output;
//}