////////////////////////////////////////////////////////////////////////////////
// Filename: shadowshaderclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "shadowshaderclass.h"


ShadowShaderClass::ShadowShaderClass()
{
	//m_vertexShader = 0;
	//m_pixelShader = 0;
	m_layout = 0;
	m_sampleStateClamp = 0;
	m_matrixBuffer = 0;
	m_lightBuffer2 = 0;
}


ShadowShaderClass::ShadowShaderClass(const ShadowShaderClass& other)
{
}


//ShadowShaderClass::~ShadowShaderClass()
//{
//}


bool ShadowShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;


	// Initialize the vertex and pixel shaders.
	//result = InitializeShader(device, hwnd, L"../Engine/shadow.vs", L"../Engine/shadow.ps");
	result = InitializeShader(device, hwnd, L"shadow_vs.cso", L"shadow_ps.cso");

	if(!result)
	{
		return false;
	}

	return true;
}


//void ShadowShaderClass::Shutdown()
//{
//	// Shutdown the vertex and pixel shaders as well as the related objects.
//	ShutdownShader();
//
//	return;
//}


bool ShadowShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, 
							   XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix, 
							   ID3D11ShaderResourceView* depthMapTexture, XMFLOAT3 lightPosition)
{
	bool result;


	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix,  depthMapTexture, 
								 lightPosition);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, indexCount);

	return true;
}


bool ShadowShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	//ID3D10Blob* vertexShaderBuffer;
	//ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
    D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc2;

	CreateShaders(device, hwnd, vsFilename, psFilename);

	//// Initialize the pointers this function will use to null.
	//errorMessage = 0;
	//vertexShaderBuffer = 0;
	//pixelShaderBuffer = 0;

 //   // Compile the vertex shader code.
	////result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "ShadowVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
	////							   &vertexShaderBuffer, &errorMessage, NULL);
	//result = D3DCompileFromFile(vsFilename, NULL, NULL, "ShadowVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
	//	0, &vertexShaderBuffer, &errorMessage);
	//if(FAILED(result))
	//{
	//	// If the shader failed to compile it should have writen something to the error message.
	//	if(errorMessage)
	//	{
	//		OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
	//	}
	//	// If there was nothing in the error message then it simply could not find the shader file itself.
	//	else
	//	{
	//		MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
	//	}

	//	return false;
	//}

 //   // Compile the pixel shader code.
	////result = D3DX11CompileFromFile(psFilename, NULL, NULL, "ShadowPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
	////							   &pixelShaderBuffer, &errorMessage, NULL);
	//result = D3DCompileFromFile(psFilename, NULL, NULL, "ShadowPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
	//	0, &pixelShaderBuffer, &errorMessage);
	//if(FAILED(result))
	//{
	//	// If the shader failed to compile it should have writen something to the error message.
	//	if(errorMessage)
	//	{
	//		OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
	//	}
	//	// If there was nothing in the error message then it simply could not find the file itself.
	//	else
	//	{
	//		MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
	//	}

	//	return false;
	//}

 //   // Create the vertex shader from the buffer.
 //   result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	//if(FAILED(result))
	//{
	//	return false;
	//}

 //   // Create the pixel shader from the buffer.
 //   result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	//if(FAILED(result))
	//{
	//	return false;
	//}

	// Create the vertex input layout description.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, _vertexShaderBuffer->GetBufferPointer(), _vertexShaderBuffer->GetBufferSize(), 
		                               &m_layout);
	if(FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	//vertexShaderBuffer->Release();
	//vertexShaderBuffer = 0;

	//pixelShaderBuffer->Release();
	//pixelShaderBuffer = 0;

	// Create a clamp texture sampler state description.
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
    result = device->CreateSamplerState(&samplerDesc, &m_sampleStateClamp);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Setup the description of the light dynamic constant buffer that is in the vertex shader.
	lightBufferDesc2.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc2.ByteWidth = sizeof(LightBufferType2);
	lightBufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc2.MiscFlags = 0;
	lightBufferDesc2.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc2, NULL, &m_lightBuffer2);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}


bool ShadowShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, 
											XMMATRIX projectionMatrix, XMMATRIX lightViewMatrix, XMMATRIX lightProjectionMatrix, 
											ID3D11ShaderResourceView* depthMapTexture, XMFLOAT3 lightPosition)
{
	HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* dataPtr;
	LightBufferType2* dataPtr3;

	// Transpose the matrices to prepare them for the shader.
	//XMMATRIXTranspose(&worldMatrix, &worldMatrix);
	//XMMATRIXTranspose(&viewMatrix, &viewMatrix);
	//XMMATRIXTranspose(&projectionMatrix, &projectionMatrix);
	//XMMATRIXTranspose(&lightViewMatrix, &lightViewMatrix);
	//XMMATRIXTranspose(&lightProjectionMatrix, &lightProjectionMatrix);
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);
	lightViewMatrix = XMMatrixTranspose(lightViewMatrix);
	lightProjectionMatrix = XMMatrixTranspose(lightProjectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	dataPtr->lightView = lightViewMatrix;
	dataPtr->lightProjection = lightProjectionMatrix;

	// Unlock the constant buffer.
    deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &depthMapTexture);

	// Lock the second light constant buffer so it can be written to.
	result = deviceContext->Map(m_lightBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr3 = (LightBufferType2*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr3->lightPosition = lightPosition;
	dataPtr3->padding = 0.0f;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_lightBuffer2, 0);

	// Set the position of the light constant buffer in the vertex shader.
	bufferNumber = 1;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer2);

	return true;
}


void ShadowShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(m_vertexShader.Get(), NULL, 0);
    deviceContext->PSSetShader(m_pixelShader.Get(), NULL, 0);

	// Set the sampler states in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleStateClamp);

	// Render the triangle.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}