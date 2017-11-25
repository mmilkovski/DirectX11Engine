////////////////////////////////////////////////////////////////////////////////
// Filename: textclass.h
////////////////////////////////////////////////////////////////////////////////
#pragma once

//////////////
// INCLUDES //
//////////////
#include "fontclass.h"
#include "ShaderManagerClass.h"
//#include "fontshaderclass.h"

////////////////////////////////////////////////////////////////////////////////
// Class name: TextClass
////////////////////////////////////////////////////////////////////////////////
class TextClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, int, int, int, bool, FontClass*, char*, int, int, float, float, float);
	void Shutdown();
	void Render(ID3D11DeviceContext*, /*FontShaderClass* pFontShader*/ShaderManagerClass* pShaderManager, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

	bool UpdateSentence(ID3D11DeviceContext*, FontClass*, char*, int, int, float, float, float);

private:
	bool InitializeSentence(ID3D11Device*, ID3D11DeviceContext*, FontClass*, char*, int, int, float, float, float);
	void RenderSentence(ID3D11DeviceContext*, /*class FontShaderClass* pFontShader, */ShaderManagerClass* shaderManager, XMMATRIX, XMMATRIX, XMMATRIX, ID3D11ShaderResourceView*);

private:
	//FontShaderClass* _FontShader;
	ShaderManagerClass* _ShaderManager;

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer, *m_vertexBuffer2, *m_indexBuffer2;
	int m_screenWidth, m_screenHeight, m_maxLength, m_vertexCount, m_indexCount;
	//bool m_shadow;
	XMFLOAT4 m_pixelColor;
};