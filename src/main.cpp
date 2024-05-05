#include "main.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND h_wnd, UINT msg, WPARAM w_param, LPARAM l_param);

Present o_present;
HWND window = NULL;
WNDPROC o_wnd_proc;
ID3D11Device* device_ptr = NULL;
ID3D11DeviceContext* context_ptr = NULL;
ID3D11RenderTargetView* main_render_target_view_ptr;

void init_imgui() {
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(device_ptr, context_ptr);
}

LRESULT __stdcall window_process(const HWND h_wnd, UINT u_msg, WPARAM w_param, LPARAM l_param) {
    if(true && ImGui_ImplWin32_WndProcHandler(h_wnd, u_msg, w_param, l_param))
    return true;

    return CallWindowProc(o_wnd_proc, h_wnd, u_msg, w_param, l_param);
}

bool init = false;
HRESULT __stdcall present_hook(IDXGISwapChain* swap_chain_ptr, UINT sync_interval, UINT flags) {
    if(!init){
        if(SUCCEEDED(swap_chain_ptr->GetDevice(__uuidof(ID3D11Device),(void**)& device_ptr))){
            device_ptr->GetImmediateContext(&context_ptr);
            DXGI_SWAP_CHAIN_DESC sd; 
            swap_chain_ptr -> GetDesc(&sd);
            window = sd.OutputWindow;
            ID3D11Texture2D* texture_buf_ptr;
            swap_chain_ptr->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& texture_buf_ptr);
            device_ptr->CreateRenderTargetView(texture_buf_ptr, NULL, &main_render_target_view_ptr);
            texture_buf_ptr->Release();
            o_wnd_proc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)window_process);
            init_imgui();
            init = true;
        }
        else return o_present(swap_chain_ptr, sync_interval, flags);
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Thuggy");
    ImGui::End();

    ImGui::Render();

    context_ptr->OMSetRenderTargets(1, &main_render_target_view_ptr, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    return o_present(swap_chain_ptr, sync_interval, flags);
}

DWORD WINAPI main_thread(LPVOID lp_reserved){
    bool init_hook = false;
    do{
        if(kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success){
            kiero::bind(8, (void**)& o_present, present_hook);
            init_hook = true;
        }
    }while (!init_hook);
    return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hMod);
        CreateThread(nullptr, 0, main_thread, hMod, 0, nullptr);
        break;
    
    case DLL_PROCESS_DETACH:
    kiero::shutdown();
        break;
    }
    return TRUE;
}