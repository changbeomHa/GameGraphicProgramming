#include "Texture.h"

#include "Texture/DDSTextureLoader.h"
#include "Texture/WICTextureLoader.h"

namespace library
{
    ComPtr<ID3D11SamplerState> Texture::s_samplers[static_cast<size_t>(eTextureSamplerType::COUNT)];

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Texture::Texture

      Summary:  Constructor

      Args:     const std::filesystem::path& textureFilePath
                  Path to the texture to use
                eTextureSamplerType textureSamplerType
                  Texture sampler type of this texture

      Modifies: [m_filePath, m_textureRV, m_textureSamplerType].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Texture::Texture definition (remove the comment)
    --------------------------------------------------------------------*/
    Texture::Texture(_In_ const std::filesystem::path& filePath, _In_opt_ eTextureSamplerType textureSamplerType) :
        m_filePath(filePath),
        m_textureRV(),
        m_textureSamplerType(textureSamplerType)
    {
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Texture::Initialize

      Summary:  Initializes the texture

      Args:     ID3D11Device* pDevice
                  The Direct3D device to create the buffers
                ID3D11DeviceContext* pImmediateContext
                  The Direct3D context to set buffers

      Modifies: [m_textureRV].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Texture::Initialize definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Texture::Initialize(_In_ ID3D11Device* pDevice, _In_ ID3D11DeviceContext* pImmediateContext)
    {
        HRESULT hr = CreateWICTextureFromFile(
            pDevice,
            pImmediateContext,
            m_filePath.c_str(),
            nullptr,
            m_textureRV.GetAddressOf()
        );
        if (FAILED(hr))
        {
            hr = CreateDDSTextureFromFile(pDevice, m_filePath.c_str(), nullptr, m_textureRV.GetAddressOf());
            if (FAILED(hr))
            {
                OutputDebugString(L"Can't load texture from \"");
                OutputDebugString(m_filePath.c_str());
                OutputDebugString(L"\n");
                return hr;
            }
        }


        // WRAP : 모든 (u,v) 정수 접합부에서 텍스처를 타일링
        // CLAMP : [0.0, 1.0] 범위를 벗어난 텍스처 좌표는 각각 0.0 또는 1.0에서 텍스처 색상으로 설정됨
        
        // Create the sample state
        if (!s_samplers[static_cast<size_t>(eTextureSamplerType::TRILINEAR_WRAP)].Get())
        {
            D3D11_SAMPLER_DESC sampDesc =
            {
                /*--------------------------------------------------------------------
                  TODO: Initialize the trilinear wrap sampler (remove the comment)
                --------------------------------------------------------------------*/
                .Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
                .AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
                .AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
                .AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
                .ComparisonFunc = D3D11_COMPARISON_ALWAYS,
                .MinLOD = 0,
                .MaxLOD = D3D11_FLOAT32_MAX
            };
            hr = pDevice->CreateSamplerState(&sampDesc, s_samplers[static_cast<size_t>(eTextureSamplerType::TRILINEAR_WRAP)].GetAddressOf());
            if (FAILED(hr))
            {
                return hr;
            }
        }

        if (!s_samplers[static_cast<size_t>(eTextureSamplerType::TRILINEAR_CLAMP)].Get())
        {
            D3D11_SAMPLER_DESC sampDesc =
            {
                /*--------------------------------------------------------------------
                  TODO: Initialize the trilinear clamp sampler (remove the comment)
                --------------------------------------------------------------------*/
                .Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
                .AddressU = D3D11_TEXTURE_ADDRESS_CLAMP,
                .AddressV = D3D11_TEXTURE_ADDRESS_CLAMP,
                .AddressW = D3D11_TEXTURE_ADDRESS_CLAMP,
                .ComparisonFunc = D3D11_COMPARISON_ALWAYS,
                .MinLOD = 0,
                .MaxLOD = D3D11_FLOAT32_MAX
            };
            hr = pDevice->CreateSamplerState(&sampDesc, s_samplers[static_cast<size_t>(eTextureSamplerType::TRILINEAR_CLAMP)].GetAddressOf());
            if (FAILED(hr))
            {
                return hr;
            }
        }

        return hr;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Texture::GetTextureResourceView

      Summary:  Constructor

      Returns:  ComPtr<ID3D11ShaderResourceView>&
                  Shader resource view
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    ComPtr<ID3D11ShaderResourceView>& Texture::GetTextureResourceView()
    {
        return m_textureRV;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Texture::GetSamplerType

  Summary:  Returns the sampler type

  Returns:  eTextureSamplerType
              Sampler type
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    eTextureSamplerType Texture::GetSamplerType() const
    {
        return m_textureSamplerType;
    }
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Texture::GetSamplerState

      Summary:  Constructor

      Returns:  ComPtr<ID3D11SamplerState>&
                  Sampler state
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*ComPtr<ID3D11SamplerState>& Texture::GetSamplerState()
    {
        return m_samplerLinear;
    }*/
}