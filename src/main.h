#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <iostream>
#include <filesystem>
#include "kiero.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <SimConnect.h>
#include "simvars/simvars.h"
#include "autopilotOnRoids/AutopilotOnRoids.h"
#include <sstream>


typedef HRESULT (__stdcall*Present)(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags);

typedef LRESULT (CALLBACK*WNDPROC)(HWND, UINT, WPARAM, LPARAM);

typedef uintptr_t PTR;
