#include "main.h"

#include "Console/Console.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device *pDevice = NULL;
ID3D11DeviceContext *pContext = NULL;
ID3D11RenderTargetView *mainRenderTargetView;
HANDLE hSimConnect = nullptr;
bool simConnectInitilailzed = false;

enum DATA_DEFINE_ID {
	THUGZ_DO_DEFINIING,
	THUGZ_DO_READONLY,
	THUGZ_DO_WRITING,
};

enum DATA_REQUEST_ID {
	THUGZ_DO_REQUESTING,
	THUGZ_DO_READONLY_REQUESTING
};

Console console;
AutopilotOnRoids::AutopilotOnRoids apOnTheRoidz(console);

void CALLBACK ThuggyDispatchProcRD(SIMCONNECT_RECV *pData, DWORD cbData, void *pContext) {
	switch (pData->dwID) {
		case SIMCONNECT_RECV_ID_SIMOBJECT_DATA: {
			auto pObjectData = reinterpret_cast<SIMCONNECT_RECV_SIMOBJECT_DATA *>(pData);
			if (pObjectData->dwRequestID == THUGZ_DO_READONLY_REQUESTING) {
				auto pSrO = reinterpret_cast<AutopilotOnRoids::SimvarsReadonly *>(&pObjectData->dwData);

				double atcRunwayRelativePositionX = pSrO->atcRunwayRelativePositionX;
				double atcRunwayRelativePositionY = pSrO->atcRunwayRelativePositionY;
				double atcRunwayRelativePositionZ = pSrO->atcRunwayRelativePositionZ;
				double atcRunwayHeadingDegreesTrue = pSrO->atcRunwayHeadingDegreesTrue;

				apOnTheRoidz.setReadonlyVars(AutopilotOnRoids::SimvarsReadonly{
					atcRunwayRelativePositionX,
					atcRunwayRelativePositionY,
					atcRunwayRelativePositionZ,
					atcRunwayHeadingDegreesTrue
				});
			}
			if (pObjectData->dwRequestID == THUGZ_DO_REQUESTING) {
				auto pS = reinterpret_cast<AutopilotOnRoids::SimVars *>(&pObjectData->dwData);

				double planeHeadingDegreesTrue = pS->planeHeadingDegreesTrue;
				double planeBankDegrees = pS->planeBankDegrees;
				double planePitchDegrees = pS->planePitchDegrees;
				double planeLatitude = pS->planeLatitude;
				double planeLongitude = pS->planeLongitude;
				double planeAltAboveGround = pS->planeAltAboveGround;

				apOnTheRoidz.setWriteableVars(AutopilotOnRoids::SimVars{
					planeHeadingDegreesTrue,
					planeBankDegrees,
					planePitchDegrees,
					planeLatitude,
					planeLongitude,
					planeAltAboveGround
				});
			}

			AutopilotOnRoids::SimVars result = apOnTheRoidz.getWritableVars();

			static bool s_gotResults = false;


			SIMCONNECT_DATA_INITPOSITION pos;
			pos.Altitude = result.planeAltAboveGround;
			pos.Airspeed = 0;
			pos.Longitude = result.planeLongitude;
			pos.Latitude = result.planeLatitude;
			pos.OnGround = 0;
			pos.Bank = 0.0;
			pos.Pitch = 0.0;
			pos.Heading = result.planeHeadingDegreesTrue;

			HRESULT hr = SimConnect_SetDataOnSimObject(hSimConnect, THUGZ_DO_WRITING,
			                                           SIMCONNECT_OBJECT_ID_USER,
			                                           0, 0, sizeof(pos), &pos);

			std::stringstream resultStream;

			resultStream << "Teleported Aircraft" << hr;
			console.addEntry(resultStream.str());
		}
	}
}

bool LoadFont(const std::string &fontFilePath, float fontSize) {
	ImGuiIO &io = ImGui::GetIO();

	std::filesystem::path fontPath(fontFilePath);
	if (!std::filesystem::exists(fontPath)) {
		std::cerr << "Font file not found: " << fontFilePath << std::endl;
		return false;
	}

	ImFont *font = io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), fontSize);
	if (font == nullptr) {
		std::cerr << "Failed to load font: " << fontFilePath << std::endl;
		return false;
	}

	io.FontDefault = font;

	return true;
}

void InitImGui() {
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);

	if (!LoadFont("C:\\Windows\\Fonts\\Calibril.ttf", 16.0f)) {
		std::cerr << "Failed to init font, shits broken" << std::endl;
		return;
	}

	ImGuiStyle &style = ImGui::GetStyle();
	ImVec4 *colors = style.Colors;
	colors[ImGuiCol_Text] = ImVec4(0.55f, 0.75f, 0.77f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.90f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.33f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 0.00f, 0.99f, 0.30f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.84f, 0.13f, 0.59f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.51f, 0.17f, 0.61f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.48f, 0.16f, 0.41f, 0.65f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.09f, 0.86f, 0.92f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.21f, 0.85f, 0.80f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.12f, 0.52f, 0.54f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.98f, 0.26f, 0.93f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.97f, 0.26f, 0.98f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.84f, 0.06f, 0.98f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.62f, 0.09f, 0.62f, 0.43f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.64f, 0.08f, 0.60f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.95f, 0.26f, 0.98f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.72f, 0.10f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.10f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.02f, 0.11f, 0.86f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.95f, 0.26f, 0.98f, 0.74f);
	colors[ImGuiCol_TabActive] = ImVec4(0.87f, 0.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	style.WindowPadding = ImVec2(9.0f, 10.0f);
	style.FramePadding = ImVec2(9.0f, 5.0f);
	style.ItemSpacing = ImVec2(9.0f, 7.0f);
	style.ItemInnerSpacing = ImVec2(8.0f, 7.0f);
	style.TouchExtraPadding = ImVec2(0.0f, 0.0f);
	style.IndentSpacing = 20.0f;
	style.ScrollbarSize = 14.0f;
	style.GrabMinSize = 14.0f;

	style.WindowBorderSize = 0.0f;
	style.ChildBorderSize = 0.0f;
	style.PopupBorderSize = 0.0f;
	style.FrameBorderSize = 0.0f;
	style.TabBorderSize = 0.0f;

	style.WindowRounding = 12.0f;
	style.ChildRounding = 12.0f;
	style.FrameRounding = 12.0f;
	style.PopupRounding = 12.0f;
	style.ScrollbarRounding = 12.0f;
	style.GrabRounding = 12.0f;
	style.TabRounding = 12.0f;

	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
}

HMODULE hMod = nullptr;

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool isInjected = false;
bool g_showMenu = true;
bool g_teleportEnabled = false;

bool init = false;

void UnloadDll() {
	isInjected = false;

	if (window != nullptr && oWndProc != nullptr) {
		SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR) oWndProc);
		oWndProc = nullptr;
	}

	if (mainRenderTargetView != nullptr) {
		mainRenderTargetView->Release();
		mainRenderTargetView = nullptr;
	}

	if (pContext != nullptr) {
		pContext->Release();
		pContext = nullptr;
	}
	if (pDevice != nullptr) {
		pDevice->Release();
		pDevice = nullptr;
	}

	if (oPresent != nullptr) {
		kiero::unbind(8);

		oPresent = nullptr;
	}

	if (hMod != nullptr) {
		FreeLibraryAndExitThread(hMod, 0);
	}
}

void handleHotkey() {
	if (GetAsyncKeyState(VK_END) & 1) {
		UnloadDll();
	}
}

HRESULT __stdcall hkPresent(IDXGISwapChain *pSwapChain, UINT SyncInterval, UINT Flags) {
	if (!init) {
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void **)&pDevice))) {
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D *pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *) &pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC) SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR) WndProc);
			InitImGui();
			init = true;
			isInjected = true;
		} else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowSizeConstraints(ImVec2(1000, 800), ImVec2(1000, 800));

	if (g_showMenu) {
		ImGui::Begin("Thuggy", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		if (ImGui::Button("Teleport")) {
			SimConnect_CallDispatch(hSimConnect, ThuggyDispatchProcRD, NULL);
		}

		std::vector<std::string> cns = console.GetConsole();
		for (const std::string &str: cns) {
			ImGui::TextUnformatted(str.c_str());
		}
		ImGui::End();
	}

	ImGui::Render();

	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	handleHotkey();

	return oPresent(pSwapChain, SyncInterval, Flags);
}

SimVarsC vars;

DWORD WINAPI MainThread(LPVOID lpReserved) {
	bool init_hook = false;
	do {
		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success) {
			kiero::bind(8, (void **) &oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);

	if (SUCCEEDED(SimConnect_Open(&hSimConnect, "ThugwareFRGetWithTheProgramLilNigga", NULL, 0, 0, 0))) {
		simConnectInitilailzed = true;

		SimConnect_AddToDataDefinition(hSimConnect, THUGZ_DO_READONLY,
		                               vars.getSimVarName(SimVarsEnum::ATC_RUNWAY_RELATIVE_POSITION_X),
		                               vars.getSimVarUnit(SimVarUnits::METERS), SIMCONNECT_DATATYPE_FLOAT64);
		SimConnect_AddToDataDefinition(hSimConnect, THUGZ_DO_READONLY,
		                               vars.getSimVarName(SimVarsEnum::ATC_RUNWAY_RELATIVE_POSITION_Y),
		                               vars.getSimVarUnit(SimVarUnits::METERS), SIMCONNECT_DATATYPE_FLOAT64);
		SimConnect_AddToDataDefinition(hSimConnect, THUGZ_DO_READONLY,
		                               vars.getSimVarName(SimVarsEnum::ATC_RUNWAY_RELATIVE_POSITION_Z),
		                               vars.getSimVarUnit(SimVarUnits::METERS), SIMCONNECT_DATATYPE_FLOAT64);
		SimConnect_AddToDataDefinition(hSimConnect, THUGZ_DO_READONLY,
		                               vars.getSimVarName(SimVarsEnum::ATC_RUNWAY_HEADING_DEGREES_TRUE),
		                               vars.getSimVarUnit(SimVarUnits::DEGREES), SIMCONNECT_DATATYPE_FLOAT64);

		SimConnect_AddToDataDefinition(hSimConnect, THUGZ_DO_DEFINIING,
		                               vars.getSimVarName(SimVarsEnum::PLANE_HEADING_DEGREES_TRUE),
		                               vars.getSimVarUnit(SimVarUnits::DEGREES));

		SimConnect_AddToDataDefinition(hSimConnect, THUGZ_DO_DEFINIING,
		                               vars.getSimVarName(SimVarsEnum::PLANE_BANK_DEGREES),
		                               vars.getSimVarUnit(SimVarUnits::RADIANS));
		SimConnect_AddToDataDefinition(hSimConnect, THUGZ_DO_DEFINIING,
		                               vars.getSimVarName(SimVarsEnum::PLANE_PITCH_DEGREES),
		                               vars.getSimVarUnit(SimVarUnits::RADIANS));

		SimConnect_AddToDataDefinition(hSimConnect, THUGZ_DO_DEFINIING,
		                               vars.getSimVarName(SimVarsEnum::PLANE_LATITUDE),
		                               vars.getSimVarUnit(SimVarUnits::RADIANS));

		SimConnect_AddToDataDefinition(hSimConnect, THUGZ_DO_DEFINIING,
		                               vars.getSimVarName(SimVarsEnum::PLANE_LONGITUDE),
		                               vars.getSimVarUnit(SimVarUnits::RADIANS));

		SimConnect_AddToDataDefinition(hSimConnect, THUGZ_DO_DEFINIING,
		                               vars.getSimVarName(SimVarsEnum::PLANE_ALT_ABOVE_GROUND),
		                               vars.getSimVarUnit(SimVarUnits::FEET));

		HRESULT hr = SimConnect_AddToDataDefinition(hSimConnect, THUGZ_DO_WRITING, "Initial Position", NULL,
		                                            SIMCONNECT_DATATYPE_INITPOSITION);

		std::stringstream stream;

		stream << "Data Defs:" << hr;

		console.addEntry(stream.str());


		SimConnect_RequestDataOnSimObject(hSimConnect, THUGZ_DO_REQUESTING, THUGZ_DO_DEFINIING,
		                                  SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
	}

	while (true) {
		if (g_teleportEnabled) {
			if (simConnectInitilailzed) {
				g_teleportEnabled = false;
			}
		}

		if (GetAsyncKeyState(VK_DELETE) % 1) {
			g_showMenu = true;
		}
		Sleep(100);

		// add some way to panic out of simconnect if it breaks, best way is just to restart the game tho ngl
	}

	if (hSimConnect) {
		SimConnect_Close(hSimConnect);
	}
	return TRUE;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
	switch (dwReason) {
		case DLL_PROCESS_ATTACH:
			hMod = hModule;
			DisableThreadLibraryCalls(hModule);
			CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
			break;
		case DLL_PROCESS_DETACH:
			kiero::shutdown();

			break;
	}
	return TRUE;
}
