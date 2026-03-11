#include "aupch.h"

#include "ImGuiLayer.h"
#include "Aurora/Core/Application.h"

#include "Platform/DirectX/Renderer/DirectX12Context.h"
#include "Platform/DirectX/Renderer/DirectX12RenderCommand.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_dx12.h>

#include <GLFW/glfw3.h>

namespace Aurora {
	ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {
	}

	void ImGuiLayer::OnAttach() {
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		//float fontSize = 18.0f; // *2.0f;
		//io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", fontSize);
		//io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", fontSize);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		SetDarkThemeColors();

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		//m_Context = dynamic_cast<WindowsWindow&>(Application::Get().GetWindow()).GetGraphicsContextAs<DirectX12Context>();
		m_Context = DirectX12RenderCommand::GetContext();
		ID3D12Device* device = m_Context->GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = m_SrvHeapSize;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		ThrowOnFail(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_ImGuiSrvHeap)));

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOther(window, true);

		ImGui_ImplDX12_InitInfo init_info = {};
		init_info.Device = device;
		init_info.CommandQueue = m_Context->GetCommandQueue();
		init_info.NumFramesInFlight = static_cast<int>(m_Context->GetFrameResourcesCount());
		init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		init_info.SrvDescriptorHeap = m_ImGuiSrvHeap.Get();
		init_info.UserData = this;

		init_info.SrvDescriptorAllocFn = [](ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu) {
			auto* layer = (ImGuiLayer*)info->UserData;

			// Ellenőrzés, hogy betelt-e
			if (layer->m_SrvAllocatedCount >= layer->m_SrvHeapSize) {
				// Itt illik logolni egy hibát vagy assertálni
				// PL: MS_CORE_ASSERT(false, "ImGui SRV Heap megtelt! Növeld meg a méretét!");
				return;
			}

			UINT index = layer->m_SrvAllocatedCount;
			layer->m_SrvAllocatedCount++;

			auto descriptor_size = info->Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			D3D12_CPU_DESCRIPTOR_HANDLE cpu_start = info->SrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			D3D12_GPU_DESCRIPTOR_HANDLE gpu_start = info->SrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

			out_cpu->ptr = cpu_start.ptr + (index * descriptor_size);
			out_gpu->ptr = gpu_start.ptr + (index * descriptor_size);
			};

		init_info.SrvDescriptorFreeFn = [](ImGui_ImplDX12_InitInfo*, D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE) {
			// Ebben az egyszerű példában nem szabadítunk fel semmit (Linear Allocator)
			// Ha kifinomultabb rendszer kell, itt csökkenthetnéd az indexet vagy használhatnál "Free List"-et.
			};

		ImGui_ImplDX12_Init(&init_info);

		//ImGui_ImplDX12_Init(device, 
		//	m_Context->GetFrameResourcesCount(), 
		//	DXGI_FORMAT_R8G8B8A8_UNORM, // TODO: get it from context too
		//	m_ImGuiSrvHeap.Get(),
		//	m_ImGuiSrvHeap->GetCPUDescriptorHandleForHeapStart(),
		//	m_ImGuiSrvHeap->GetGPUDescriptorHandleForHeapStart()
		//);
	}
	
	void ImGuiLayer::OnDetach() {
		// ImGui_ImplVulkan_Shutdown();
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	
	void ImGuiLayer::OnEvent(Event& event) {
		if (m_BlockEvents) {
			ImGuiIO& io = ImGui::GetIO();
			event.Handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			event.Handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::Begin() {
		//ImGui_ImplVulkan_NewFrame();
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End() {
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(static_cast<float>(app.GetWindow().GetWidth()), static_cast<float>(app.GetWindow().GetHeight()));

		//ThrowOnFail(m_Context->m_CommandList->Reset(m_Context->m_CommandAllocator.Get(), nullptr));

		//ID3D12DescriptorHeap* descriptorHeaps[] = { m_Context->m_CbvSrvUavHeap.Get() };
		//m_Context->m_CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
		//m_Context->m_CommandList->SetGraphicsRootSignature(m_Context->m_RootSignature.Get());

		// Rendering
		ImGui::Render();
		//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_Context->m_CommandList.Get());
		//ThrowOnFail(m_Context->m_CommandList->Close());

		ID3D12GraphicsCommandList* cmdList = m_Context->GetCommandList();
		ID3D12DescriptorHeap* descriptorHeaps[] = { m_ImGuiSrvHeap.Get() };
		cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList);

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::SetDarkThemeColors() {
		auto& colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

		// Headers
		colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		
		// Buttons
		colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Frame BG
		colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
		colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
		colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
		colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
		colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

		// Title
		colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	}

	uint32_t ImGuiLayer::GetActiveWidgetID() const {
		return GImGui->ActiveId;
	}
}
