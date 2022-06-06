#include "Renderer/Renderer.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Renderer

      Summary:  Constructor

      Modifies: [m_driverType, m_featureLevel, m_d3dDevice, m_d3dDevice1,
                  m_immediateContext, m_immediateContext1, m_swapChain,
                  m_swapChain1, m_renderTargetView, m_depthStencil,
                  m_depthStencilView, m_cbChangeOnResize, m_cbShadowMatrix,
                  m_pszMainSceneName, m_camera, m_projection, m_scenes
                  m_invalidTexture, m_shadowMapTexture, m_shadowVertexShader,
                  m_shadowPixelShader].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    Renderer::Renderer()
        : m_driverType(D3D_DRIVER_TYPE_NULL)
        , m_featureLevel(D3D_FEATURE_LEVEL_11_0)
        , m_d3dDevice()
        , m_d3dDevice1()
        , m_immediateContext()
        , m_immediateContext1()
        , m_swapChain()
        , m_swapChain1()
        , m_renderTargetView()
        , m_depthStencil()
        , m_depthStencilView()
        , m_cbChangeOnResize()
        , m_pszMainSceneName(nullptr)
        , m_padding{ '\0' }
        , m_camera(XMVectorSet(0.0f, 3.0f, -6.0f, 0.0f))
        , m_projection()
        , m_scenes()
        , m_invalidTexture(std::make_shared<Texture>(L"Content/Common/InvalidTexture.png"))
        , m_shadowMapTexture()
        , m_shadowVertexShader()
        , m_shadowPixelShader()
    {
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Initialize

      Summary:  Creates Direct3D device and swap chain

      Args:     HWND hWnd
                  Handle to the window

      Modifies: [m_d3dDevice, m_featureLevel, m_immediateContext,
                  m_d3dDevice1, m_immediateContext1, m_swapChain1,
                  m_swapChain, m_renderTargetView, m_vertexShader,
                  m_vertexLayout, m_pixelShader, m_vertexBuffer
                  m_cbShadowMatrix].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Initialize definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::Initialize(_In_ HWND hWnd) {
        HRESULT hr = S_OK;

        RECT rc;
        GetClientRect(hWnd, &rc);
        UINT uWidth = static_cast<UINT>(rc.right - rc.left);
        UINT uHeight = static_cast<UINT>(rc.bottom - rc.top);

        UINT uCreateDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) || defined(_DEBUG)
        uCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_DRIVER_TYPE driverTypes[] =
        {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE,
        };
        UINT numDriverTypes = ARRAYSIZE(driverTypes);

        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };
        UINT numFeatureLevels = ARRAYSIZE(featureLevels);

        for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
        {
            m_driverType = driverTypes[driverTypeIndex];
            hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, uCreateDeviceFlags, featureLevels, numFeatureLevels,
                D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &m_featureLevel, m_immediateContext.GetAddressOf());

            if (hr == E_INVALIDARG)
            {
                // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
                hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, uCreateDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                    D3D11_SDK_VERSION, m_d3dDevice.GetAddressOf(), &m_featureLevel, m_immediateContext.GetAddressOf());
            }

            if (SUCCEEDED(hr))
            {
                break;
            }
        }
        if (FAILED(hr))
        {
            return hr;
        }

        // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
        ComPtr<IDXGIFactory1> dxgiFactory;
        {
            ComPtr<IDXGIDevice> dxgiDevice;
            hr = m_d3dDevice.As(&dxgiDevice);
            if (SUCCEEDED(hr))
            {
                ComPtr<IDXGIAdapter> adapter;
                hr = dxgiDevice->GetAdapter(&adapter);
                if (SUCCEEDED(hr))
                {
                    hr = adapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
                }
            }
        }
        if (FAILED(hr))
        {
            return hr;
        }

        // Create swap chain
        ComPtr<IDXGIFactory2> dxgiFactory2;
        hr = dxgiFactory.As(&dxgiFactory2);
        if (SUCCEEDED(hr))
        {
            // DirectX 11.1 or later
            hr = m_d3dDevice.As(&m_d3dDevice1);
            if (SUCCEEDED(hr))
            {
                m_immediateContext.As(&m_immediateContext1);
            }

            DXGI_SWAP_CHAIN_DESC1 sd =
            {
                .Width = uWidth,
                .Height = uHeight,
                .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                .SampleDesc = {.Count = 1u, .Quality = 0u },
                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = 1u
            };

            hr = dxgiFactory2->CreateSwapChainForHwnd(m_d3dDevice.Get(), hWnd, &sd, nullptr, nullptr, m_swapChain1.GetAddressOf());
            if (SUCCEEDED(hr))
            {
                hr = m_swapChain1.As(&m_swapChain);
            }
        }
        else
        {
            // DirectX 11.0 systems
            DXGI_SWAP_CHAIN_DESC sd =
            {
                .BufferDesc = {.Width = uWidth, .Height = uHeight, .RefreshRate = {.Numerator = 60, .Denominator = 1 }, .Format = DXGI_FORMAT_R8G8B8A8_UNORM },
                .SampleDesc = {.Count = 1, .Quality = 0 },
                .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                .BufferCount = 1u,
                .OutputWindow = hWnd,
                .Windowed = TRUE
            };

            hr = dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &sd, m_swapChain.GetAddressOf());
        }

        // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
        dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

        if (FAILED(hr))
        {
            return hr;
        }

        // Create a render target view
        ComPtr<ID3D11Texture2D> pBackBuffer;
        hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        if (FAILED(hr))
        {
            return hr;
        }

        hr = m_d3dDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        // Create depth stencil texture
        D3D11_TEXTURE2D_DESC descDepth =
        {
            .Width = uWidth,
            .Height = uHeight,
            .MipLevels = 1u,
            .ArraySize = 1u,
            .Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
            .SampleDesc = {.Count = 1u, .Quality = 0u },
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_DEPTH_STENCIL,
            .CPUAccessFlags = 0u,
            .MiscFlags = 0u
        };
        hr = m_d3dDevice->CreateTexture2D(&descDepth, nullptr, m_depthStencil.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        // Create the depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV =
        {
            .Format = descDepth.Format,
            .ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D,
            .Texture2D = {.MipSlice = 0 }
        };
        hr = m_d3dDevice->CreateDepthStencilView(m_depthStencil.Get(), &descDSV, m_depthStencilView.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

        // Setup the viewport
        D3D11_VIEWPORT vp =
        {
            .TopLeftX = 0.0f,
            .TopLeftY = 0.0f,
            .Width = static_cast<FLOAT>(uWidth),
            .Height = static_cast<FLOAT>(uHeight),
            .MinDepth = 0.0f,
            .MaxDepth = 1.0f,
        };
        m_immediateContext->RSSetViewports(1, &vp);

        // Set primitive topology
        m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // Create the constant buffers
        D3D11_BUFFER_DESC bd =
        {
            .ByteWidth = sizeof(CBChangeOnResize),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = 0
        };
        hr = m_d3dDevice->CreateBuffer(&bd, nullptr, m_cbChangeOnResize.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        // Initialize the projection matrix
        m_projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, static_cast<FLOAT>(uWidth) / static_cast<FLOAT>(uHeight), 0.01f, 1000.0f);

        CBChangeOnResize cbChangesOnResize =
        {
            .Projection = XMMatrixTranspose(m_projection)
        };
        m_immediateContext->UpdateSubresource(m_cbChangeOnResize.Get(), 0, nullptr, &cbChangesOnResize, 0, 0);

        bd.ByteWidth = sizeof(CBLights);
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0u;

        hr = m_d3dDevice->CreateBuffer(&bd, nullptr, m_cbLights.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        D3D11_BUFFER_DESC cbShadowMatrix =
        {
            .ByteWidth = sizeof(CBShadowMatrix),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = 0
        };

        hr = m_d3dDevice->CreateBuffer(&cbShadowMatrix, nullptr, m_cbShadowMatrix.GetAddressOf());

        if (FAILED(hr))     
            return hr;
        

        m_shadowMapTexture = std::make_shared<RenderTexture>(uWidth, uHeight);

        m_shadowMapTexture->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());

        /*for (UINT i = 0u; i < NUM_LIGHTS; i++)
        {
            m_scenes[m_pszMainSceneName]->GetPointLight(i)->Initialize(uWidth, uHeight);
        }*/
        m_camera.Initialize(m_d3dDevice.Get());

        if (!m_scenes.contains(m_pszMainSceneName))
        {
            return E_FAIL;
        }

        hr = m_scenes[m_pszMainSceneName]->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());
        if (FAILED(hr))
        {
            return hr;
        }

        hr = m_invalidTexture->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());
        if (FAILED(hr))
        {
            return hr;
        }

        return S_OK;

    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddScene

      Summary:  Add scene to renderer

      Args:     PCWSTR pszSceneName
                  The name of the scene
                const std::shared_ptr<Scene>&
                  The shared pointer to Scene

      Modifies: [m_scenes].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::AddScene(_In_ PCWSTR pszSceneName, _In_ const std::shared_ptr<Scene>& scene)
    {
        if (m_scenes.contains(pszSceneName))
        {
            return E_FAIL;
        }

        m_scenes[pszSceneName] = scene;

        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::GetSceneOrNull

      Summary:  Return scene with the given name or null

      Args:     PCWSTR pszSceneName
                  The name of the scene

      Returns:  std::shared_ptr<Scene>
                  The shared pointer to Scene
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    std::shared_ptr<Scene> Renderer::GetSceneOrNull(_In_ PCWSTR pszSceneName)
    {
        if (m_scenes.contains(pszSceneName))
        {
            return m_scenes[pszSceneName];
        }

        return nullptr;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetMainScene

      Summary:  Set the main scene

      Args:     PCWSTR pszSceneName
                  The name of the scene

      Modifies: [m_pszMainSceneName].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    HRESULT Renderer::SetMainScene(_In_ PCWSTR pszSceneName)
    {
        if (!m_scenes.contains(pszSceneName))
        {
            return E_FAIL;
        }

        m_pszMainSceneName = pszSceneName;

        return S_OK;
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::HandleInput

      Summary:  Handle user mouse input

      Args:     DirectionsInput& directions
                MouseRelativeMovement& mouseRelativeMovement
                FLOAT deltaTime
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderer::HandleInput(_In_ const DirectionsInput& directions, _In_ const MouseRelativeMovement& mouseRelativeMovement, _In_ FLOAT deltaTime)
    {
        m_camera.HandleInput(directions, mouseRelativeMovement, deltaTime);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Update

      Summary:  Update the renderables each frame

      Args:     FLOAT deltaTime
                  Time difference of a frame
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderer::Update(_In_ FLOAT deltaTime)
    {
        m_scenes[m_pszMainSceneName]->Update(deltaTime);

        m_camera.Update(deltaTime);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Renderer::SetShadowMapShaders

  Summary:  Set shaders for the shadow mapping

  Args:     std::shared_ptr<ShadowVertexShader>
              vertex shader
            std::shared_ptr<PixelShader>
              pixel shader

  Modifies: [m_shadowVertexShader, m_shadowPixelShader].
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    void Renderer::SetShadowMapShaders(_In_ std::shared_ptr<ShadowVertexShader> vertexShader, _In_ std::shared_ptr<PixelShader> pixelShader)
    {
        m_shadowVertexShader = move(vertexShader);
        m_shadowPixelShader = move(pixelShader);
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Render

      Summary:  Render the frame
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Render definition (remove the comment)
    --------------------------------------------------------------------*/
    void Renderer::Render() {

        //RenderSceneToTexture();
        float ClearColor[4] = { 0.0f, 0.125f, 0.6f, 1.0f };
        m_immediateContext->ClearRenderTargetView(m_renderTargetView.Get(), ClearColor);
        m_immediateContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
        UINT uStride = sizeof(SimpleVertex);
        UINT uOffset = 0;

        CBChangeOnCameraMovement cb_changesOnCameraMovement;
        cb_changesOnCameraMovement.View = XMMatrixTranspose(m_camera.GetView());
        XMStoreFloat4(&cb_changesOnCameraMovement.CameraPosition, m_camera.GetEye());
        m_immediateContext->UpdateSubresource(m_camera.GetConstantBuffer().Get(), 0, nullptr, &cb_changesOnCameraMovement, 0u, 0u);
        m_immediateContext->VSSetConstantBuffers(0u, 1u, m_camera.GetConstantBuffer().GetAddressOf());
        
        
        
        for (auto s : m_scenes) {

            if (s.second->GetSkyBox())
            {
                UINT aStrides[2] =
                {
                    sizeof(SimpleVertex),
                    sizeof(NormalData)
                };
                UINT aOffsets[2] = { 0u, 0u };

                ComPtr<ID3D11Buffer> aBuffers[2] =
                {
                    s.second->GetSkyBox()->GetVertexBuffer().Get(),
                    s.second->GetSkyBox()->GetNormalBuffer().Get()
                };

                // Set the vertex buffer
                m_immediateContext->IASetVertexBuffers(0u, 2u, aBuffers->GetAddressOf(), aStrides, aOffsets);
                m_immediateContext->IASetIndexBuffer(s.second->GetSkyBox()->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0u);
                m_immediateContext->IASetInputLayout(s.second->GetSkyBox()->GetVertexLayout().Get());

                CBChangesEveryFrame cbChangesEveryFrame =
                {
                    .World = XMMatrixTranspose(s.second->GetSkyBox()->GetWorldMatrix()),
                    .OutputColor = s.second->GetSkyBox()->GetOutputColor(),
                    .HasNormalMap = s.second->GetSkyBox()->HasNormalMap()
                };
                m_immediateContext->UpdateSubresource(s.second->GetSkyBox()->GetConstantBuffer().Get(), 0u, nullptr, &cbChangesEveryFrame, 0u, 0u);

                m_immediateContext->VSSetShader(s.second->GetSkyBox()->GetVertexShader().Get(), nullptr, 0u);
                m_immediateContext->VSSetConstantBuffers(0u, 1u, m_camera.GetConstantBuffer().GetAddressOf());
                m_immediateContext->VSSetConstantBuffers(1u, 1u, m_cbChangeOnResize.GetAddressOf());
                m_immediateContext->VSSetConstantBuffers(2u, 1u, s.second->GetSkyBox()->GetConstantBuffer().GetAddressOf());
                m_immediateContext->VSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());

                m_immediateContext->PSSetConstantBuffers(0u, 1u, m_camera.GetConstantBuffer().GetAddressOf());
                m_immediateContext->PSSetConstantBuffers(1u, 1u, m_cbChangeOnResize.GetAddressOf());
                m_immediateContext->PSSetConstantBuffers(2u, 1u, s.second->GetSkyBox()->GetConstantBuffer().GetAddressOf());
                m_immediateContext->PSSetShader(s.second->GetSkyBox()->GetPixelShader().Get(), nullptr, 0u);

                if (s.second->GetSkyBox()->HasTexture())
                {
                    for (UINT i = 0u; i < s.second->GetSkyBox()->GetNumMeshes(); ++i)
                    {
                        UINT materialIndex = s.second->GetSkyBox()->GetMesh(i).uMaterialIndex;

                        if (s.second->GetSkyBox()->GetMaterial(materialIndex)->pDiffuse)
                        {
                            eTextureSamplerType textureSamplerType = s.second->GetSkyBox()->GetMaterial(materialIndex)->pDiffuse->GetSamplerType();

                            m_immediateContext->PSSetShaderResources(0u, 1u, s.second->GetSkyBox()->GetMaterial(materialIndex)->pDiffuse->GetTextureResourceView().GetAddressOf());
                            m_immediateContext->PSSetSamplers(0u, 1u, Texture::s_samplers[static_cast<size_t>(textureSamplerType)].GetAddressOf());
                        }

                        if (s.second->GetSkyBox()->GetMaterial(materialIndex)->pNormal)
                        {
                            eTextureSamplerType textureSamplerType = s.second->GetSkyBox()->GetMaterial(materialIndex)->pNormal->GetSamplerType();

                            m_immediateContext->PSSetShaderResources(1u, 1u, s.second->GetSkyBox()->GetMaterial(materialIndex)->pNormal->GetTextureResourceView().GetAddressOf());
                            m_immediateContext->PSSetSamplers(0u, 1u, Texture::s_samplers[static_cast<size_t>(textureSamplerType)].GetAddressOf());
                        }

                        m_immediateContext->DrawIndexed(
                            s.second->GetSkyBox()->GetMesh(i).uNumIndices,
                            s.second->GetSkyBox()->GetMesh(i).uBaseIndex,
                            s.second->GetSkyBox()->GetMesh(i).uBaseVertex
                        );
                    }
                }
            }




            // 여기 다시 확인, for문 밖에서 동작?
            CBLights cb_lights;
            for (int k = 0; k < NUM_LIGHTS; k++) {
                cb_lights.LightPositions[k] = s.second->GetPointLight(k)->GetPosition();
                cb_lights.LightColors[k] = s.second->GetPointLight(k)->GetColor();
                //cb_lights.LightViews[k] = XMMatrixTranspose(s.second->GetPointLight(k)->GetViewMatrix());
                //cb_lights.LightProjections[k] = XMMatrixTranspose(s.second->GetPointLight(k)->GetProjectionMatrix());
                
            }
            m_immediateContext->UpdateSubresource(m_cbLights.Get(), 0u, nullptr, &cb_lights, 0u, 0u);
            m_immediateContext->VSSetConstantBuffers(3, 1, m_cbLights.GetAddressOf()); // 필요한가?
            m_immediateContext->PSSetConstantBuffers(3, 1, m_cbLights.GetAddressOf());

            for (auto renderable = s.second->GetRenderables().begin(); renderable != s.second->GetRenderables().end(); ++renderable)
            {
                // Set the vertex buffer
                UINT aStrides[2] =
                {
                    static_cast<UINT>(sizeof(SimpleVertex)),
                    static_cast<UINT>(sizeof(NormalData))
                };
                UINT aOffsets[2] = { 0u, 0u };
                ComPtr<ID3D11Buffer> aBuffers[2]
                {
                   renderable->second->GetVertexBuffer(),
                   renderable->second->GetNormalBuffer()
                };

                m_immediateContext->IASetVertexBuffers(0u, 2u, aBuffers->GetAddressOf(), aStrides, aOffsets);

                // Set the index buffer
                m_immediateContext->IASetIndexBuffer(renderable->second->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0u);

                // Set the input layout
                m_immediateContext->IASetInputLayout(renderable->second->GetVertexLayout().Get());

                // Create renderable constant buffer and update
                CBChangesEveryFrame cbChangesEveryFrame =
                {
                    .World = XMMatrixTranspose(renderable->second->GetWorldMatrix()),
                    .OutputColor = renderable->second->GetOutputColor(),
                    .HasNormalMap = renderable->second->HasNormalMap()
                };
                m_immediateContext->UpdateSubresource(renderable->second->GetConstantBuffer().Get(), 0u, nullptr, &cbChangesEveryFrame, 0u, 0u);

                // Render
                m_immediateContext->VSSetShader(renderable->second->GetVertexShader().Get(), nullptr, 0u);
                m_immediateContext->VSSetConstantBuffers(0u, 1u, m_camera.GetConstantBuffer().GetAddressOf());
                m_immediateContext->VSSetConstantBuffers(1u, 1u, m_cbChangeOnResize.GetAddressOf());
                m_immediateContext->VSSetConstantBuffers(2u, 1u, renderable->second->GetConstantBuffer().GetAddressOf());
                m_immediateContext->VSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());

                m_immediateContext->PSSetConstantBuffers(0u, 1u, m_camera.GetConstantBuffer().GetAddressOf());
                m_immediateContext->PSSetConstantBuffers(2u, 1u, renderable->second->GetConstantBuffer().GetAddressOf());
                m_immediateContext->PSSetConstantBuffers(3u, 1u, m_cbLights.GetAddressOf());
                m_immediateContext->PSSetShader(renderable->second->GetPixelShader().Get(), nullptr, 0u);

                m_immediateContext->PSSetShaderResources(2u, 1u, m_shadowMapTexture->GetShaderResourceView().GetAddressOf());
                m_immediateContext->PSSetSamplers(2u, 1u, m_shadowMapTexture->GetSamplerState().GetAddressOf());

                if (s.second->GetSkyBox()->HasTexture())
                {
                    for (UINT i = 0u; i < s.second->GetSkyBox()->GetNumMeshes(); ++i)
                    {
                        UINT materialIndex = s.second->GetSkyBox()->GetMesh(i).uMaterialIndex;

                        if (s.second->GetSkyBox()->GetMaterial(materialIndex)->pDiffuse)
                        {
                            eTextureSamplerType textureSamplerType = s.second->GetSkyBox()->GetMaterial(materialIndex)->pDiffuse->GetSamplerType();

                            m_immediateContext->PSSetShaderResources(0u, 1u, s.second->GetSkyBox()->GetMaterial(materialIndex)->pDiffuse->GetTextureResourceView().GetAddressOf());
                            m_immediateContext->PSSetSamplers(0u, 1u, Texture::s_samplers[static_cast<size_t>(textureSamplerType)].GetAddressOf());
                        }

                        if (s.second->GetSkyBox()->GetMaterial(materialIndex)->pNormal)
                        {
                            eTextureSamplerType textureSamplerType = s.second->GetSkyBox()->GetMaterial(materialIndex)->pNormal->GetSamplerType();

                            m_immediateContext->PSSetShaderResources(1u, 1u, s.second->GetSkyBox()->GetMaterial(materialIndex)->pNormal->GetTextureResourceView().GetAddressOf());
                            m_immediateContext->PSSetSamplers(0u, 1u, Texture::s_samplers[static_cast<size_t>(textureSamplerType)].GetAddressOf());
                        }

                        m_immediateContext->DrawIndexed(
                            s.second->GetSkyBox()->GetMesh(i).uNumIndices,
                            s.second->GetSkyBox()->GetMesh(i).uBaseIndex,
                            s.second->GetSkyBox()->GetMesh(i).uBaseVertex
                        );
                    }
                }
                else
                {
                    m_immediateContext->DrawIndexed(renderable->second->GetNumIndices(), 0u, 0);
                }
            }

            // voxel
            UINT stride[3] = { sizeof(SimpleVertex), sizeof(NormalData), sizeof(InstanceData) };
            UINT offset[3] = { 0u, 0u, 0u };
           
            for (auto i : s.second->GetVoxels()) {

                ComPtr<ID3D11Buffer> buffer[3] = { i->GetVertexBuffer().Get(), i->GetNormalBuffer().Get(), i->GetInstanceBuffer().Get() };
                m_immediateContext->IASetVertexBuffers(0, 3, buffer->GetAddressOf(), stride, offset);
                m_immediateContext->IASetIndexBuffer(i->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
                m_immediateContext->IASetInputLayout(i->GetVertexLayout().Get());

                CBChangesEveryFrame cbChanges = {
                    .World = XMMatrixTranspose(i->GetWorldMatrix()),
                    .OutputColor = i->GetOutputColor(),
                    .HasNormalMap = i->HasNormalMap()

                };
                m_immediateContext->UpdateSubresource(i->GetConstantBuffer().Get(), 0, nullptr, &cbChanges, 0, 0);

                m_immediateContext->VSSetShader(i->GetVertexShader().Get(), nullptr, 0);
                m_immediateContext->VSSetConstantBuffers(0, 1, m_camera.GetConstantBuffer().GetAddressOf());
                m_immediateContext->VSSetConstantBuffers(1, 1, m_cbChangeOnResize.GetAddressOf());
                m_immediateContext->VSSetConstantBuffers(2, 1, i->GetConstantBuffer().GetAddressOf());
                m_immediateContext->VSSetConstantBuffers(3, 1, m_cbLights.GetAddressOf());

                m_immediateContext->PSSetShader(i->GetPixelShader().Get(), nullptr, 0);
                m_immediateContext->PSSetConstantBuffers(0, 1, m_camera.GetConstantBuffer().GetAddressOf());
                m_immediateContext->PSSetConstantBuffers(2, 1, i->GetConstantBuffer().GetAddressOf());
                m_immediateContext->PSSetConstantBuffers(3, 1, m_cbLights.GetAddressOf());

                m_immediateContext->PSSetShaderResources(2u, 1, m_shadowMapTexture->GetShaderResourceView().GetAddressOf());
                m_immediateContext->PSSetSamplers(2u, 1, m_shadowMapTexture->GetSamplerState().GetAddressOf());
                if (i->HasTexture())
                {
                    for (UINT k = 0u; k < i->GetNumMeshes(); k++)
                    {
                        const UINT materialIndex = i->GetMesh(k).uMaterialIndex;
                        eTextureSamplerType textureSamplerType = i->GetMaterial(materialIndex)->pDiffuse->GetSamplerType();
                        if (i->GetMaterial(materialIndex)->pDiffuse)
                        {
                            m_immediateContext->PSSetShaderResources(0u, 1u, i->GetMaterial(materialIndex)->pDiffuse->GetTextureResourceView().GetAddressOf());
                            m_immediateContext->PSSetSamplers(0u, 1u, Texture::s_samplers[static_cast<size_t>(textureSamplerType)].GetAddressOf());
                        }
                        if (i->GetMaterial(materialIndex)->pNormal)
                        {
                            m_immediateContext->PSSetShaderResources(1, 1, i->GetMaterial(materialIndex)->pNormal->GetTextureResourceView().GetAddressOf());
                            m_immediateContext->PSSetSamplers(1, 1, Texture::s_samplers[static_cast<size_t>(textureSamplerType)].GetAddressOf());
                        }
                        m_immediateContext->DrawIndexedInstanced(i->GetNumIndices(), i->GetNumInstances(), 0, 0, 0);

                    }
                }
                else {
                    m_immediateContext->DrawIndexedInstanced(i->GetNumIndices(), i->GetNumInstances(), 0, 0, 0);
                }
            }
            

            // model

            // Animation을 지워야 한다.
            UINT aStrides[2] =
            {
                static_cast<UINT>(sizeof(SimpleVertex)),
                static_cast<UINT>(sizeof(NormalData))

            };
            UINT aOffsets[2] = { 0u,  0u };

            for (auto k : s.second->GetModels())
            {
                ComPtr<ID3D11Buffer> aBuffers[2]
                {
                   k.second->GetVertexBuffer().Get(),
                   k.second->GetNormalBuffer().Get()
                };

                m_immediateContext->IASetVertexBuffers(0, 2, aBuffers->GetAddressOf(), aStrides, aOffsets);
                m_immediateContext->IASetIndexBuffer(k.second->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);
                m_immediateContext->IASetInputLayout(k.second->GetVertexLayout().Get());



                CBChangesEveryFrame cb_ChangesEveryFrame = {
                    .World = XMMatrixTranspose(k.second->GetWorldMatrix()),
                    .OutputColor = k.second->GetOutputColor(),
                    .HasNormalMap = k.second->HasNormalMap()

                };
                m_immediateContext->UpdateSubresource(k.second->GetConstantBuffer().Get(), 0, nullptr, &cb_ChangesEveryFrame, 0, 0);

                CBSkinning cb_Skinning = {
                    .BoneTransforms = {}
                };
                for (UINT i = 0; i < k.second->GetBoneTransforms().size(); ++i)
                {
                    cb_Skinning.BoneTransforms[i] = XMMatrixTranspose(k.second->GetBoneTransforms()[i]);
                }

                m_immediateContext->UpdateSubresource(k.second->GetSkinningConstantBuffer().Get(), 0, nullptr, &cb_Skinning, 0, 0);

                m_immediateContext->VSSetShader(k.second->GetVertexShader().Get(), nullptr, 0);
                m_immediateContext->VSSetConstantBuffers(2, 1, k.second->GetConstantBuffer().GetAddressOf());
                m_immediateContext->VSSetConstantBuffers(4, 1, k.second->GetSkinningConstantBuffer().GetAddressOf());
                m_immediateContext->PSSetShader(k.second->GetPixelShader().Get(), nullptr, 0);
                m_immediateContext->PSSetConstantBuffers(2, 1, k.second->GetConstantBuffer().GetAddressOf());


                m_immediateContext->PSSetShaderResources(2u, 1, m_shadowMapTexture->GetShaderResourceView().GetAddressOf());
                m_immediateContext->PSSetSamplers(2u, 1, m_shadowMapTexture->GetSamplerState().GetAddressOf());

                if (k.second->HasTexture())
                {
                    for (UINT i = 0u; i < k.second->GetNumMeshes(); i++)
                    {
                        const UINT materialIndex = k.second->GetMesh(i).uMaterialIndex;
                        eTextureSamplerType textureSamplerType = k.second->GetMaterial(materialIndex)->pDiffuse->GetSamplerType();
                        if (k.second->GetMaterial(materialIndex)->pDiffuse)
                        {
                            m_immediateContext->PSSetShaderResources(0u, 1u, k.second->GetMaterial(materialIndex)->pDiffuse->GetTextureResourceView().GetAddressOf());
                            m_immediateContext->PSSetSamplers(0u, 1u, Texture::s_samplers[static_cast<size_t>(textureSamplerType)].GetAddressOf());
                        }
                        if (k.second->GetMaterial(materialIndex)->pNormal)
                        {
                            m_immediateContext->PSSetShaderResources(1, 1, k.second->GetMaterial(materialIndex)->pNormal->GetTextureResourceView().GetAddressOf());
                            m_immediateContext->PSSetSamplers(1, 1, Texture::s_samplers[static_cast<size_t>(textureSamplerType)].GetAddressOf());
                        }
                        m_immediateContext->DrawIndexed(
                            k.second->GetMesh(i).uNumIndices,
                            k.second->GetMesh(i).uBaseIndex,
                            k.second->GetMesh(i).uBaseVertex);
                    }
                }
                else {
                    m_immediateContext->DrawIndexed(k.second->GetNumIndices(), 0, 0);
                }
            }
            m_swapChain->Present(0, 0);
        }
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
  Method:   Renderer::RenderSceneToTexture

  Summary:  Render scene to the texture
M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
/*--------------------------------------------------------------------
  TODO: Renderer::RenderSceneToTexture definition (remove the comment)
--------------------------------------------------------------------*/
    void Renderer::RenderSceneToTexture() {
        //Unbind current pixel shader resources
        ID3D11ShaderResourceView* const pSRV[2] = { NULL, NULL };
        m_immediateContext->PSSetShaderResources(0, 2, pSRV);
        m_immediateContext->PSSetShaderResources(2, 1, pSRV);

        m_immediateContext->OMSetRenderTargets(1, m_shadowMapTexture->GetRenderTargetView().GetAddressOf(), m_depthStencilView.Get());
        m_immediateContext->ClearRenderTargetView(m_shadowMapTexture->GetRenderTargetView().Get(), Colors::White);
        m_immediateContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

        for (auto i : m_scenes[m_pszMainSceneName]->GetRenderables()) {
            UINT uStride = sizeof(SimpleVertex);
            UINT uOffset = 0;
            m_immediateContext->IASetVertexBuffers(0u, 1u, i.second->GetVertexBuffer().GetAddressOf(), &uStride, &uOffset);
            m_immediateContext->IASetIndexBuffer(i.second->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0); 
            m_immediateContext->IASetInputLayout(m_shadowVertexShader->GetVertexLayout().Get());

            CBShadowMatrix cb = {
                .World = XMMatrixTranspose(i.second->GetWorldMatrix()),
                //.View = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(0)->GetViewMatrix()),
                //.Projection = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(0)->GetProjectionMatrix()),
                .IsVoxel = FALSE
            };

            m_immediateContext->UpdateSubresource(i.second->GetConstantBuffer().Get(), 0, nullptr, &cb, 0, 0);

            m_immediateContext->VSSetShader(i.second->GetVertexShader().Get(), nullptr, 0);
            m_immediateContext->PSSetShader(i.second->GetPixelShader().Get(), nullptr, 0);

            for (UINT j = 0; j < i.second->GetNumMeshes(); j++)
            {
                m_immediateContext->DrawIndexed(i.second->GetMesh(j).uNumIndices, i.second->GetMesh(j).uBaseIndex, i.second->GetMesh(j).uBaseVertex);
            }
        }

        for (auto i : m_scenes[m_pszMainSceneName]->GetVoxels()) {
            UINT uStride = sizeof(SimpleVertex);
            UINT uOffset = 0;
            m_immediateContext->IASetVertexBuffers(0u, 2u, i->GetVertexBuffer().GetAddressOf(), &uStride, &uOffset);
            m_immediateContext->IASetIndexBuffer(i->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0); 
            m_immediateContext->IASetInputLayout(m_shadowVertexShader->GetVertexLayout().Get());

            m_immediateContext->VSSetShader(m_shadowVertexShader->GetVertexShader().Get(), nullptr, 0u);
            m_immediateContext->VSSetConstantBuffers(0u, 1u, m_cbShadowMatrix.GetAddressOf());
            m_immediateContext->PSSetShader(m_shadowPixelShader->GetPixelShader().Get(), nullptr, 0u);

            CBShadowMatrix cb = {
                .World = XMMatrixTranspose(i->GetWorldMatrix()),
                //.View = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(0)->GetViewMatrix()),
                //.Projection = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(0)->GetProjectionMatrix()),
                .IsVoxel = FALSE
            };
            m_immediateContext->UpdateSubresource(m_cbShadowMatrix.Get(), 0, nullptr, &cb, 0, 0);


            for (UINT j = 0; j < i->GetNumMeshes(); j++)
            {
                m_immediateContext->DrawIndexed(i->GetMesh(j).uNumIndices, i->GetMesh(j).uBaseIndex, i->GetMesh(j).uBaseVertex);
            }
        }

        for (auto i : m_scenes[m_pszMainSceneName]->GetModels()) {
            UINT uStride = sizeof(SimpleVertex);
            UINT uOffset = 0;
            m_immediateContext->IASetVertexBuffers(0u, 1u, i.second->GetVertexBuffer().GetAddressOf(), &uStride, &uOffset);
            m_immediateContext->IASetIndexBuffer(i.second->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0); 
            m_immediateContext->IASetInputLayout(m_shadowVertexShader->GetVertexLayout().Get());

            m_immediateContext->VSSetShader(m_shadowVertexShader->GetVertexShader().Get(), nullptr, 0u);
            m_immediateContext->VSSetConstantBuffers(0u, 1u, m_cbShadowMatrix.GetAddressOf());
            m_immediateContext->PSSetShader(m_shadowPixelShader->GetPixelShader().Get(), nullptr, 0u);

            CBShadowMatrix cb = {
                .World = XMMatrixTranspose(i.second->GetWorldMatrix()),
                //.View = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(0)->GetViewMatrix()),
                //.Projection = XMMatrixTranspose(m_scenes[m_pszMainSceneName]->GetPointLight(0)->GetProjectionMatrix()),
                .IsVoxel = FALSE
            };

            m_immediateContext->UpdateSubresource(m_cbShadowMatrix.Get(), 0, nullptr, &cb, 0, 0);


            for (UINT j = 0; j < i.second->GetNumMeshes(); j++)
            {
                m_immediateContext->DrawIndexed(i.second->GetMesh(j).uNumIndices, i.second->GetMesh(j).uBaseIndex, i.second->GetMesh(j).uBaseVertex);
            }
        }


        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
    * 
      Method:   Renderer::GetDriverType

      Summary:  Returns the Direct3D driver type

      Returns:  D3D_DRIVER_TYPE
                  The Direct3D driver type used
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    D3D_DRIVER_TYPE Renderer::GetDriverType() const
    {
        return m_driverType;
    }
    
    
}