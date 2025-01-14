#pragma once
#include "Shader.h"

// ������ ���������� solid�ϰ� �׸��� ������ ���̴�
// wireframe�� ���Ѵٸ� CreateRasterizerState �Լ��� override�϶�
class ColorShader :
    public Shader
{
public:
    ColorShader();

    virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;

    virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shader_blob) override;
    virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shader_blob) override;

};

