#include "Rev/ImGui/ImGuiLayer.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/RenderCmd.h"

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_vulkan.h>

//temp
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <VulkanRHI/VulkanCore.h>


static ImGuiKey GlfwKeyToImGuiKey(int key)
{
    switch (key)
    {
    case GLFW_KEY_TAB: return ImGuiKey_Tab;
    case GLFW_KEY_LEFT: return ImGuiKey_LeftArrow;
    case GLFW_KEY_RIGHT: return ImGuiKey_RightArrow;
    case GLFW_KEY_UP: return ImGuiKey_UpArrow;
    case GLFW_KEY_DOWN: return ImGuiKey_DownArrow;
    case GLFW_KEY_PAGE_UP: return ImGuiKey_PageUp;
    case GLFW_KEY_PAGE_DOWN: return ImGuiKey_PageDown;
    case GLFW_KEY_HOME: return ImGuiKey_Home;
    case GLFW_KEY_END: return ImGuiKey_End;
    case GLFW_KEY_INSERT: return ImGuiKey_Insert;
    case GLFW_KEY_DELETE: return ImGuiKey_Delete;
    case GLFW_KEY_BACKSPACE: return ImGuiKey_Backspace;
    case GLFW_KEY_SPACE: return ImGuiKey_Space;
    case GLFW_KEY_ENTER: return ImGuiKey_Enter;
    case GLFW_KEY_ESCAPE: return ImGuiKey_Escape;
    case GLFW_KEY_APOSTROPHE: return ImGuiKey_Apostrophe;
    case GLFW_KEY_COMMA: return ImGuiKey_Comma;
    case GLFW_KEY_MINUS: return ImGuiKey_Minus;
    case GLFW_KEY_PERIOD: return ImGuiKey_Period;
    case GLFW_KEY_SLASH: return ImGuiKey_Slash;
    case GLFW_KEY_SEMICOLON: return ImGuiKey_Semicolon;
    case GLFW_KEY_EQUAL: return ImGuiKey_Equal;
    case GLFW_KEY_LEFT_BRACKET: return ImGuiKey_LeftBracket;
    case GLFW_KEY_BACKSLASH: return ImGuiKey_Backslash;
    case GLFW_KEY_RIGHT_BRACKET: return ImGuiKey_RightBracket;
    case GLFW_KEY_GRAVE_ACCENT: return ImGuiKey_GraveAccent;
    case GLFW_KEY_CAPS_LOCK: return ImGuiKey_CapsLock;
    case GLFW_KEY_SCROLL_LOCK: return ImGuiKey_ScrollLock;
    case GLFW_KEY_NUM_LOCK: return ImGuiKey_NumLock;
    case GLFW_KEY_PRINT_SCREEN: return ImGuiKey_PrintScreen;
    case GLFW_KEY_PAUSE: return ImGuiKey_Pause;
    case GLFW_KEY_KP_0: return ImGuiKey_Keypad0;
    case GLFW_KEY_KP_1: return ImGuiKey_Keypad1;
    case GLFW_KEY_KP_2: return ImGuiKey_Keypad2;
    case GLFW_KEY_KP_3: return ImGuiKey_Keypad3;
    case GLFW_KEY_KP_4: return ImGuiKey_Keypad4;
    case GLFW_KEY_KP_5: return ImGuiKey_Keypad5;
    case GLFW_KEY_KP_6: return ImGuiKey_Keypad6;
    case GLFW_KEY_KP_7: return ImGuiKey_Keypad7;
    case GLFW_KEY_KP_8: return ImGuiKey_Keypad8;
    case GLFW_KEY_KP_9: return ImGuiKey_Keypad9;
    case GLFW_KEY_KP_DECIMAL: return ImGuiKey_KeypadDecimal;
    case GLFW_KEY_KP_DIVIDE: return ImGuiKey_KeypadDivide;
    case GLFW_KEY_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
    case GLFW_KEY_KP_SUBTRACT: return ImGuiKey_KeypadSubtract;
    case GLFW_KEY_KP_ADD: return ImGuiKey_KeypadAdd;
    case GLFW_KEY_KP_ENTER: return ImGuiKey_KeypadEnter;
    case GLFW_KEY_KP_EQUAL: return ImGuiKey_KeypadEqual;
    case GLFW_KEY_LEFT_SHIFT: return ImGuiKey_LeftShift;
    case GLFW_KEY_LEFT_CONTROL: return ImGuiKey_LeftCtrl;
    case GLFW_KEY_LEFT_ALT: return ImGuiKey_LeftAlt;
    case GLFW_KEY_LEFT_SUPER: return ImGuiKey_LeftSuper;
    case GLFW_KEY_RIGHT_SHIFT: return ImGuiKey_RightShift;
    case GLFW_KEY_RIGHT_CONTROL: return ImGuiKey_RightCtrl;
    case GLFW_KEY_RIGHT_ALT: return ImGuiKey_RightAlt;
    case GLFW_KEY_RIGHT_SUPER: return ImGuiKey_RightSuper;
    case GLFW_KEY_MENU: return ImGuiKey_Menu;
    case GLFW_KEY_0: return ImGuiKey_0;
    case GLFW_KEY_1: return ImGuiKey_1;
    case GLFW_KEY_2: return ImGuiKey_2;
    case GLFW_KEY_3: return ImGuiKey_3;
    case GLFW_KEY_4: return ImGuiKey_4;
    case GLFW_KEY_5: return ImGuiKey_5;
    case GLFW_KEY_6: return ImGuiKey_6;
    case GLFW_KEY_7: return ImGuiKey_7;
    case GLFW_KEY_8: return ImGuiKey_8;
    case GLFW_KEY_9: return ImGuiKey_9;
    case GLFW_KEY_A: return ImGuiKey_A;
    case GLFW_KEY_B: return ImGuiKey_B;
    case GLFW_KEY_C: return ImGuiKey_C;
    case GLFW_KEY_D: return ImGuiKey_D;
    case GLFW_KEY_E: return ImGuiKey_E;
    case GLFW_KEY_F: return ImGuiKey_F;
    case GLFW_KEY_G: return ImGuiKey_G;
    case GLFW_KEY_H: return ImGuiKey_H;
    case GLFW_KEY_I: return ImGuiKey_I;
    case GLFW_KEY_J: return ImGuiKey_J;
    case GLFW_KEY_K: return ImGuiKey_K;
    case GLFW_KEY_L: return ImGuiKey_L;
    case GLFW_KEY_M: return ImGuiKey_M;
    case GLFW_KEY_N: return ImGuiKey_N;
    case GLFW_KEY_O: return ImGuiKey_O;
    case GLFW_KEY_P: return ImGuiKey_P;
    case GLFW_KEY_Q: return ImGuiKey_Q;
    case GLFW_KEY_R: return ImGuiKey_R;
    case GLFW_KEY_S: return ImGuiKey_S;
    case GLFW_KEY_T: return ImGuiKey_T;
    case GLFW_KEY_U: return ImGuiKey_U;
    case GLFW_KEY_V: return ImGuiKey_V;
    case GLFW_KEY_W: return ImGuiKey_W;
    case GLFW_KEY_X: return ImGuiKey_X;
    case GLFW_KEY_Y: return ImGuiKey_Y;
    case GLFW_KEY_Z: return ImGuiKey_Z;
    case GLFW_KEY_F1: return ImGuiKey_F1;
    case GLFW_KEY_F2: return ImGuiKey_F2;
    case GLFW_KEY_F3: return ImGuiKey_F3;
    case GLFW_KEY_F4: return ImGuiKey_F4;
    case GLFW_KEY_F5: return ImGuiKey_F5;
    case GLFW_KEY_F6: return ImGuiKey_F6;
    case GLFW_KEY_F7: return ImGuiKey_F7;
    case GLFW_KEY_F8: return ImGuiKey_F8;
    case GLFW_KEY_F9: return ImGuiKey_F9;
    case GLFW_KEY_F10: return ImGuiKey_F10;
    case GLFW_KEY_F11: return ImGuiKey_F11;
    case GLFW_KEY_F12: return ImGuiKey_F12;
    default: return ImGuiKey_None;
    }
}

namespace Rev
{
static void ImGuiLayer_UpdateKeyModifiers()
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(ImGuiMod_Ctrl, ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl));
    io.AddKeyEvent(ImGuiMod_Shift, ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift));
    io.AddKeyEvent(ImGuiMod_Alt, ImGui::IsKeyDown(ImGuiKey_LeftAlt) || ImGui::IsKeyDown(ImGuiKey_RightAlt));
    io.AddKeyEvent(ImGuiMod_Super, ImGui::IsKeyDown(ImGuiKey_LeftSuper) || ImGui::IsKeyDown(ImGuiKey_RightSuper));
}
static void ImGuiLayer_SetClipboardText(void* user_data, const char* text)
{
    Application::GetApp().GetWindow()->SetClipboardText(text);
}
static const char* ImGuiLayer_GetClipboardText(void* user_data)
{
    return Application::GetApp().GetWindow()->GetClipboardText();
}

static VkDescriptorPool ImGuiLayer_Vulkan_Init()
{
    REV_CORE_ASSERT(GetRenderAPI() == ERenderAPI::Vulkan);

    FVulkanContext* pVkContext = static_cast<FVulkanContext*>(RenderCmd::GetContext());

    // 1: create descriptor pool for IMGUI
    //  the size of the pool is very oversize, but it's copied from imgui demo itself.
    VkDescriptorPoolSize PoolSizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

    VkDescriptorPoolCreateInfo PoolCreateInfo{};
    PoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    PoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    PoolCreateInfo.maxSets = 1000;
    PoolCreateInfo.poolSizeCount = (uint32_t)std::size(PoolSizes);
    PoolCreateInfo.pPoolSizes = PoolSizes;

    VkDescriptorPool ImguiPool;
    REV_VK_CHECK(vkCreateDescriptorPool(FVulkanCore::GetDevice(), &PoolCreateInfo, nullptr, &ImguiPool));

    // this initializes imgui for Vulkan
    ImGui_ImplVulkan_InitInfo ImguiInitInfo{};
    ImguiInitInfo.Instance = FVulkanCore::GetInstance();
    ImguiInitInfo.PhysicalDevice = FVulkanCore::GetPhysicalDevice();
    ImguiInitInfo.Device = FVulkanCore::GetDevice();
    ImguiInitInfo.Queue = FVulkanCore::GetQueue(VQK_Graphics);
    ImguiInitInfo.DescriptorPool = ImguiPool;
    ImguiInitInfo.MinImageCount = 3;
    ImguiInitInfo.ImageCount = 3;
    ImguiInitInfo.UseDynamicRendering = true;

    //dynamic rendering parameters for imgui to use
    ImguiInitInfo.PipelineRenderingCreateInfo = {};
    ImguiInitInfo.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    ImguiInitInfo.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
    ImguiInitInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats = &pVkContext->GetSwapchain().GetFormat();

    ImguiInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&ImguiInitInfo);
    ImGui_ImplVulkan_CreateFontsTexture();

    return ImguiPool;
}

static void ImGuiLayer_Vulkan_Shutdown(VkDescriptorPool ImguiPool)
{
    REV_CORE_ASSERT(GetRenderAPI() == ERenderAPI::Vulkan);

    ImGui_ImplVulkan_Shutdown();
    vkDestroyDescriptorPool(FVulkanCore::GetDevice(), ImguiPool, nullptr);
}

static void ImGuiLayer_Vulkan_Draw()
{
    REV_CORE_ASSERT(GetRenderAPI() == ERenderAPI::Vulkan);

    FVulkanContext* pVkContext = static_cast<FVulkanContext*>(RenderCmd::GetContext());
    VkCommandBuffer CmdBuffer = pVkContext->GetMainCmdBuffer();
    VkImageView ImageView = pVkContext->GetSwapchainImageView();
    VkExtent2D Extent = pVkContext->GetSwapchain().GetExtent();

    VkRenderingAttachmentInfo ColorAttachment = FVulkanInit::AttachmentInfo(ImageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    VkRenderingInfo RenderingInfo{};
    RenderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    RenderingInfo.pNext = nullptr;
    RenderingInfo.flags = 0;
    RenderingInfo.renderArea.extent = Extent;
    RenderingInfo.pColorAttachments = &ColorAttachment;
    RenderingInfo.colorAttachmentCount = 1;
    RenderingInfo.pDepthAttachment = nullptr;
    RenderingInfo.pStencilAttachment = nullptr;
    RenderingInfo.layerCount = 1;
    RenderingInfo.viewMask = 0;

    vkCmdBeginRendering(CmdBuffer, &RenderingInfo);

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), CmdBuffer);

    vkCmdEndRendering(CmdBuffer);
}
    
ImGuiLayer::ImGuiLayer()
	: Layer("ImGuiLayer")
{
}

ImGuiLayer::~ImGuiLayer()
{
}

void ImGuiLayer::OnAttach()
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    io.SetClipboardTextFn = ImGuiLayer_SetClipboardText;
    io.GetClipboardTextFn = ImGuiLayer_GetClipboardText;
    io.ClipboardUserData = nullptr;

    switch (GetRenderAPI())
    {
    case ERenderAPI::OpenGL:
        ImGui_ImplOpenGL3_Init("#version 450");
        break;
    case ERenderAPI::Vulkan:
        mPlatformData = ImGuiLayer_Vulkan_Init();
        break;
    default:
        REV_CORE_ASSERT(false, "[ImGuiLayer] Unknown render api")
        break;
    }

}

void ImGuiLayer::OnDetach()
{
    switch (GetRenderAPI())
    {
    case ERenderAPI::OpenGL:
        ImGui_ImplOpenGL3_Shutdown();
        break;
    case ERenderAPI::Vulkan:
        ImGuiLayer_Vulkan_Shutdown((VkDescriptorPool)mPlatformData);
        mPlatformData = nullptr;
        break;
    default:
        REV_CORE_ASSERT(false, "[ImGuiLayer] Unknown render api")
            break;
    }
    ImGui::DestroyContext();
}

void ImGuiLayer::OnUpdate(float dt)
{
	Application& app = Application::GetApp();

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(app.GetWindow()->GetWidth(), app.GetWindow()->GetHeight());
	io.DeltaTime = 1.0f/60.0f;

    switch (GetRenderAPI())
    {
    case ERenderAPI::OpenGL:
        ImGui_ImplOpenGL3_NewFrame();
        break;
    case ERenderAPI::Vulkan:
        ImGui_ImplVulkan_NewFrame();
        break;
    default:
        REV_CORE_ASSERT(false, "[ImGuiLayer] Unknown render api")
            break;
    }

	ImGui::NewFrame();

	static bool show = true;
	ImGui::ShowDemoWindow(&show);

	ImGui::Render();
}

void ImGuiLayer::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<KeyPressedEvent>(RE_BIND_EVENT_FN(ImGuiLayer::OnKeyPressed, this));
	dispatcher.Dispatch<KeyReleasedEvent>(RE_BIND_EVENT_FN(ImGuiLayer::OnKeyReleased, this));
	dispatcher.Dispatch<KeyTypedEvent>(RE_BIND_EVENT_FN(ImGuiLayer::OnKeyTyped, this));
	dispatcher.Dispatch<MouseButtonPressedEvent>(RE_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonPressed, this));
	dispatcher.Dispatch<MouseButtonReleasedEvent>(RE_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonReleased, this));
	dispatcher.Dispatch<MouseScrolledEvent>(RE_BIND_EVENT_FN(ImGuiLayer::OnMouseScrolled, this));
	dispatcher.Dispatch<MouseMovedEvent>(RE_BIND_EVENT_FN(ImGuiLayer::OnMouseMoved, this));
	dispatcher.Dispatch<WindowResizeEvent>(RE_BIND_EVENT_FN(ImGuiLayer::OnWindowResize, this));
}

void ImGuiLayer::OnDraw()
{
    switch (GetRenderAPI())
    {
    case ERenderAPI::OpenGL:
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        break;
    case ERenderAPI::Vulkan:
        ImGuiLayer_Vulkan_Draw();
        break;
    default:
        REV_CORE_ASSERT(false, "[ImGuiLayer] Unknown render api")
            break;
    }
}

bool ImGuiLayer::OnKeyPressed(KeyPressedEvent& e)
{
    ImGuiLayer_UpdateKeyModifiers();
	ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(GlfwKeyToImGuiKey(e.GetKeyCode()), true);
	return false;
}

bool ImGuiLayer::OnKeyReleased(KeyReleasedEvent& e)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(GlfwKeyToImGuiKey(e.GetKeyCode()), false);
	return false;
}

bool ImGuiLayer::OnKeyTyped(KeyTypedEvent& e)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(e.GetKeyCode());
    return false;
}

bool ImGuiLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
{
    ImGuiLayer_UpdateKeyModifiers();
	int button = e.GetMouseButton();
	ImGuiIO& io = ImGui::GetIO();
	if (button >= 0 && button < ImGuiMouseButton_COUNT)
		io.AddMouseButtonEvent(button, true);
	return false;
}

bool ImGuiLayer::OnMouseButtonReleased(MouseButtonReleasedEvent& e)
{
	int button = e.GetMouseButton();
	ImGuiIO& io = ImGui::GetIO();
	if (button >= 0 && button < ImGuiMouseButton_COUNT)
		io.AddMouseButtonEvent(button, false);
	return false;
}

bool ImGuiLayer::OnMouseScrolled(MouseScrolledEvent& e)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseWheelEvent(e.GetXOffset(), e.GetYOffset());
	return false;
}

bool ImGuiLayer::OnMouseMoved(MouseMovedEvent& e)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent(e.GetX(), e.GetY());
	return false;
}

bool ImGuiLayer::OnWindowResize(WindowResizeEvent& e)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(e.GetWidth(), e.GetHeight());
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	//glViewport(0, 0, e.GetWidth(), e.GetHeight());

	return false;
}

}