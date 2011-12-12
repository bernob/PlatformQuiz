#include "pch.h"
#include "D3DRenderer.h"

using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::UI::Core;

namespace DX
{
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
            __cli_WinRTraiseException(hr);
        }
    }

    inline IUnknown* GetIUnknown(Platform::Object^ o)
    {
        return (IUnknown*) reinterpret_cast<__cli_IUnknown*>(o);
    }
}

D3DRenderer::D3DRenderer(Windows::UI::Core::CoreWindow^ window) : m_window(window)
{
    Initialize();
}

D3DRenderer::~D3DRenderer()
{
    DiscardDeviceResources();
}

void D3DRenderer::Initialize()
{
    CreateDeviceIndependentResources();
    CreateDeviceResources();
    CreateWindowSizeDependentResources();
}

void D3DRenderer::CreateDeviceIndependentResources()
{
}

void D3DRenderer::CreateDeviceResources()
{
    // This flag is required in order to enable compatibility with Direct2D
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    ComPtr<IDXGIDevice> dxgiDevice;

#if defined(_DEBUG)
    // If the project is in a debug build, enable debugging via SDK Layers with this flag
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // This array defines the ordering of feature levels that D3D should attempt to create
    D3D_FEATURE_LEVEL featureLevels[] = 
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3
    };

    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    DX::ThrowIfFailed(
        D3D11CreateDevice(
            nullptr,                    // specify null to use the default adapter
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,                    // leave as null if hardware is used
            creationFlags,              // optionally set debug and Direct2D compatibility flags
            featureLevels,
            ARRAYSIZE(featureLevels),
            D3D11_SDK_VERSION,          // always set this to D3D11_SDK_VERSION
            &device,
            &m_featureLevel,
            &context
            )
        );

    DX::ThrowIfFailed(
        device.As(&m_d3dDevice)
        );

    DX::ThrowIfFailed(
        context.As(&m_d3dContext)
        );

    // Obtain the underlying DXGI device of the Direct3D device
    DX::ThrowIfFailed(
        m_d3dDevice.As(&dxgiDevice)
        );
}

void D3DRenderer::CreateWindowSizeDependentResources()
{
    // If the swap chain already exists, resize it
    if (m_swapChain != nullptr)
    {
        DX::ThrowIfFailed(
            m_swapChain->ResizeBuffers(2, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0)
            );
    }
    // Otherwise, create it
    else
    {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};
        swapChainDesc.Width = 0;                                     // use automatic sizing
        swapChainDesc.Height = 0;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;           // this is the most common swapchain format
        swapChainDesc.Stereo = false; 
        swapChainDesc.SampleDesc.Count = 1;                          // don't use multi-sampling
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;                               // use two buffers to enable flip effect
        swapChainDesc.Scaling = DXGI_SCALING_NONE;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // we recommend using this swap effect for all applications
        swapChainDesc.Flags = 0;

        // Once the desired swap chain description is configured, it must be created on the same adapter as our D3D Device

        // First, retrieve the underlying DXGI Device from the D3D Device
        ComPtr<IDXGIDevice1>  dxgiDevice;
        DX::ThrowIfFailed(
            m_d3dDevice.As(&dxgiDevice)
            );

        // Ensure that DXGI does not queue more than one frame at a time. This both reduces 
        // latency and ensures that the application will only render after each VSync, minimizing 
        // power consumption.
        DX::ThrowIfFailed(
            dxgiDevice->SetMaximumFrameLatency(1)
            );

        ComPtr<IDXGIAdapter> dxgiAdapter;
        DX::ThrowIfFailed(
            dxgiDevice->GetAdapter(&dxgiAdapter)
            );

        ComPtr<IDXGIFactory2> dxgiFactory;
        DX::ThrowIfFailed(
            dxgiAdapter->GetParent(
                __uuidof(IDXGIFactory2), 
                &dxgiFactory
                )
            );

        DX::ThrowIfFailed(
            dxgiFactory->CreateSwapChainForImmersiveWindow(
                m_d3dDevice.Get(),
                DX::GetIUnknown(m_window),
                &swapChainDesc,
                nullptr,    // allow on all displays
                &m_swapChain
                )
            );
    }

    ComPtr<ID3D11Texture2D> backBuffer;
    DX::ThrowIfFailed(
        m_swapChain->GetBuffer(
            0,
            __uuidof(ID3D11Texture2D),
            &backBuffer
            )
        );

    DX::ThrowIfFailed(
        m_d3dDevice->CreateRenderTargetView(
            backBuffer.Get(),
            nullptr,
            &m_renderTargetView
            )
        );

    D3D11_TEXTURE2D_DESC backBufferDesc = {0};
    backBuffer->GetDesc(&backBufferDesc);

    CD3D11_TEXTURE2D_DESC depthStencilDesc(
        DXGI_FORMAT_D24_UNORM_S8_UINT, 
        backBufferDesc.Width,
        backBufferDesc.Height,
        1,
        1,
        D3D11_BIND_DEPTH_STENCIL);

    ComPtr<ID3D11Texture2D> depthStencil;
    DX::ThrowIfFailed(
        m_d3dDevice->CreateTexture2D(
            &depthStencilDesc,
            nullptr,
            &depthStencil
            )
        );

    DX::ThrowIfFailed(
        m_d3dDevice->CreateDepthStencilView(
            depthStencil.Get(),
            &CD3D11_DEPTH_STENCIL_VIEW_DESC(D3D11_DSV_DIMENSION_TEXTURE2D),
            &m_depthStencilView
            )
        );

    CD3D11_VIEWPORT viewport(
        0.0f,
        0.0f,
        static_cast<float>(backBufferDesc.Width),
        static_cast<float>(backBufferDesc.Height)
        );

    m_d3dContext->RSSetViewports(1, &viewport);
}

void D3DRenderer::UpdateForWindowSizeChange()
{
    m_renderTargetView = nullptr;
    m_depthStencilView = nullptr;
    CreateWindowSizeDependentResources();
}

void D3DRenderer::SetDpi(float dpi)
{
    m_dpi = dpi;
}

void D3DRenderer::Update(float timeTotal, float timeDelta)
{
}

void D3DRenderer::Render()
{
    Clear();
    // draw here
    Present();
}

void D3DRenderer::Present()
{
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    HRESULT hr = m_swapChain->Present(1, 0);

    // If the device was removed either by a disconnect or a driver upgrade, we 
    // must completely reinitialize the renderer.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        Initialize();
    }
    else
    {
        DX::ThrowIfFailed(hr);
    }
}

void D3DRenderer::Clear()
{
    const float color[4] = { 0.f, 123.f/255.f, 167.f/255.0f, 1.0f };
    m_d3dContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
    m_d3dContext->ClearRenderTargetView(m_renderTargetView.Get(), color);
    m_d3dContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DRenderer::DiscardDeviceResources()
{
    m_d3dDevice = nullptr;
    m_d3dContext = nullptr;
    m_swapChain = nullptr;
    m_renderTargetView = nullptr;
    m_depthStencil = nullptr;
    m_depthStencilView = nullptr;
}

