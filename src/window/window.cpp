//
// Created by MK-10 on 2026/6/4.
//
#include <iostream>
#include <ostream>

#include "window/window.h"
#include <string>
#include "GLFW//glfw3.h"
// 数学相关的函数
#include <cmath>
// 导入bgfx
#include "bgfx/bgfx.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "window/imgui_impl_bgfx.h"
#include "gamepadBtn/gamepadBtn.h"
#include "gamepadBtn/settingBtn.h"

// 告诉 GLFW 我们在 Mac 上，需要暴露 Cocoa 原生窗口接口
#define GLFW_EXPOSE_NATIVE_COCOA
#include "imgui_internal.h"
#include "GLFW/glfw3native.h"
#include "Theme/starryTheme.h"

/// 构造函数
Window::Window(int width, int height,const std::string &title): m_width(width), m_height(height), m_title(title) {
    std::cout
          << "Window::Window() info >> \n"
          << "\t width  : " << m_width << "\n"
          << "\t height : " << m_height << "\n"
          << "\t title  : " << m_title << std::endl;
}

/// 析构函数,负责清理资源
Window::~Window() {
    std::cout << "Window::~Window()" << std::endl;

    // 新增这三行清理 ImGui
    ImGui_Implbgfx_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // 必须在销毁底层窗口之前，先关闭 bgfx 引擎释放 GPU 资源！
    bgfx::shutdown();
    if (m_window) {
        // 清理 GLFW 分配的window内存
        glfwDestroyWindow(m_window);
    }
    // 彻底清理 GLFW 分配的底层资源
    glfwTerminate();

    // 释放Theme指针
    if (m_currentTheme != nullptr) {
        delete m_currentTheme;
        m_currentTheme = nullptr;
    }
}

/// 初始化函数,返回是否成功
bool Window::init() {
    std::cout << "Window::init()" << std::endl;
    // 1. 初始化 GLFW 库
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW!" << std::endl;
        return false;
    }
    // 由于使用了bgfx，这些opengl可以不用了，window创建一个纯粹的window视图便可
    // 2. 配置 macOS 必需的 OpenGL 核心模式
    // 使用 OpenGL 3.3 版本
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // 核心模式 (Core Profile)
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    // 苹果专属前向兼容
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    // 创建一个纯粹的window视图
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // 3. 创建底层的 C 语言窗口对象
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);
    if (!m_window) {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate(); // 释放 glfwInit()占用的内存
        return false;
    }

    // 4. 设置当前上下文 所有图形绘制命令，全都画在这个 m_window 对应的画板上(bgfx无需要上下文)
    // glfwMakeContextCurrent(m_window);

    // 5. 把 C++ 的 this 指针，强行存进 C 语言的 GLFW 窗口里
    glfwSetWindowUserPointer(m_window, static_cast<void*>(this));

    // 6. glfw 注册屏幕刷新回调
    glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);

    // 7. glfw 注册keyboard回调
    glfwSetKeyCallback(m_window, keyCallback);

    // 补充,注册: 窗口位置移动回调
    glfwSetWindowPosCallback(m_window, windowPositionCallback);
    // 补充,注册: 系统窗口强制刷新的回调
    glfwSetWindowRefreshCallback(m_window, windowRefreshCallback);

    // 空跑一下，防止bgfx在子线程上操作ui
    bgfx::renderFrame();
    // 8. bgfx init,并把回调事件给bgfx
    bgfx::Init bgfxInit;
    // 自动选择最优渲染器（在 Mac 上会自动启用 Metal）
    bgfxInit.type = bgfx::RendererType::Count;
    // 设置宽高
    bgfxInit.resolution.width = static_cast<uint32_t>(m_width);
    bgfxInit.resolution.height = static_cast<uint32_t>(m_height);
    // 开启垂直同步，防止画面撕裂
    bgfxInit.resolution.reset = BGFX_RESET_VSYNC;
    // bgfxInit 申请更大的显存控件
    bgfxInit.limits.maxTransientVbSize = 15 * 1024 * 1024;
    bgfxInit.limits.maxTransientIbSize = 15 * 1024 * 1024;
    // 【关键绑定】获取 Mac 的原生窗口句柄并塞给 bgfx
    bgfx::PlatformData pd{};
#ifdef __APPLE__
    // 这就是跨语言/库混编的桥梁
    pd.nwh = glfwGetCocoaWindow(m_window);
#endif
    bgfxInit.platformData = pd;
    // 正式启动 bgfx 引擎
    if (!bgfx::init(bgfxInit)) {
        std::cout << "Failed to initialize GLFW! !bgfx::init(bgfxInit)....." << std::endl;
        return false;
    }
    // 设置默认的渲染视口 (View 0)
    bgfx::setViewRect(0,0,0,static_cast<uint16_t>(m_width),static_cast<uint16_t>(m_height));
    // 设置 View 0 的默认清除颜色（十六进制：0xRRGGBBFF，默认先给个深暗青色）
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x1a233aFF, 1.0f, 0);

    // 初始化ImGui
    Window::initImGui();

    // 装载繁星主题！
    m_currentTheme = new StarryTheme();
    return true;
}

/// 每一帧的更新逻辑（处理事件、交换缓冲）
void Window::update() {

    // 处理按键、鼠标、窗口拖动
    glfwPollEvents();

    // 交换双缓冲，把画好的东西显示到屏幕上
    // glfwSwapBuffers(m_window);

   render();
}

/// 检查窗口是否应该关闭
bool Window::shouldClose() const{
    // 真实逻辑：去问问 GLFW 当前窗口是不是被点了关闭按钮
    return glfwWindowShouldClose(m_window);
}


// --- 核心技巧：使用静态方法作为 GLFW 的回调 ---
/// 窗口大小改变回调, .cpp文件中不能写static，static只能写在.h
void Window::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto *appWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!appWindow || width <= 0 || height <= 0) {
        std::cout << "Window::framebufferResizeCallback() appwindow = null" << std::endl;
        return;
    }
    // 1. 重置 bgfx 的画布大小
    appWindow->m_width = width;
    appWindow->m_height = height;
    bgfx::reset(static_cast<uint32_t>(width), static_cast<uint32_t>(height), BGFX_RESET_VSYNC);
    bgfx::setViewRect(0, 0, 0, static_cast<uint16_t>(width), static_cast<uint16_t>(height));

    // 2. 在主循环被冻结时，在这里强行画一帧
    //appWindow->render();
}
/// 键盘事件回调, .cpp文件中不能写static，static只能写在.h
void Window::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    auto *appWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!appWindow) {
        std::cout << "Window::keyCallback() appwindow = null" << std::endl;
        return;
    }

    // 增加打印信息：实时监控键盘状态
    // 解释：action 的值通常有三种：
    // 1 (GLFW_PRESS): 按下
    // 0 (GLFW_RELEASE): 松开
    // 2 (GLFW_REPEAT): 长按连发
    std::cout << "[Key Event] 按键代码 (Key): " << key
          << " | 动作 (Action): " << action << std::endl;

    // 如果按下了 ESC 键，就通知 GLFW 准备关窗
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        std::cout << "⚠️ 检测到 ESC 被按下，正在向系统发送关窗信号..." << std::endl;
        glfwSetWindowShouldClose(window, true);
    }
}

void Window::render() {
    // 动态改变背景色的魔法（让颜色随着时间呼吸变化）
    // static float time = 0.0f;
    // time += 0.02f;
    // // 利用 sin 函数把值限制在 0.0 ~ 1.0 之间
    // float colorFactor = (std::sin(time) * 0.5f) + 0.5f;
    // // 动态计算红色通道的值 (0 ~ 255)
    // uint8_t r = static_cast<uint8_t>(colorFactor * 255);
    // uint8_t g = 0x20; // 固定的绿色分量
    // uint8_t b = 0x60; // 固定的蓝色分量
    // // 组合成 bgfx 认识的 0xRRGGBBFF 格式
    // uint32_t clearColor = (static_cast<uint32_t>(r) << 24) |
    //                       (static_cast<uint32_t>(g) << 16) |
    //                       (static_cast<uint32_t>(b) << 8) |
    //                       0xFF;
    //
    // // 重新设置这一帧画布的底色
    // bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, clearColor, 1.0f, 0);
    // // 告诉 bgfx 这一帧我们要刷新 View 0
    // bgfx::touch(0);
    // // 注意：使用 bgfx 后，我们【不再需要】原来的 glfwSwapBuffers(m_window) 了！
    //
    // // 开始画ps按钮
    // // 1. 启动新的一帧
    // ImGui_Implbgfx_NewFrame();
    // ImGui_ImplGlfw_NewFrame();
    // ImGui::NewFrame();
    //
    // // 2. 画你的按键！
    // Window::renderBtn();
    //
    // // 3. ImGui 开始打包数据
    // ImGui::Render();
    // // 4. 将 UI 打包发送给 GPU
    // ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
    //
    // // 因为 bgfx::frame() 会接管一切，在后台默默和 Metal 驱动打交道并刷新屏幕。
    // bgfx::frame();


    // 0. 清理旧画布
    uint32_t clearColor = m_currentTheme ? m_currentTheme->getClearColor() : 0x000000FF; // 默认色
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, clearColor, 1.0f, 0); // 重新设置这一帧画布的底色并擦除上一帧的残影
    bgfx::touch(0); // 告诉 bgfx 这一帧我们要刷新 View 0

    // 1.准备新的画布
    ImGui_Implbgfx_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // 2.渲染的内容
    if (m_currentTheme) {
        ImDrawList *bgDrawList = ImGui::GetBackgroundDrawList();
        m_currentTheme->render(bgDrawList,m_width,m_height);
    }
    
    // 必须调用 renderBtn()，否则按钮的代码永远不会执行！
    renderBtn();

    // 3.提交frame
    ImGui::Render();
    ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
    bgfx::frame();
}


/// 窗口位置移动回调
void Window::windowPositionCallback(GLFWwindow *window, int posx, int posy) {
    auto *appWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!appWindow) {
        std::cout << "Window::windowPositionCallback() appwindow = null" << std::endl;
       return;
    }
    // 主循环被冻结了？没关系，我们在这里强行画！
    appWindow->render();
}

/// 系统强制重绘回调
void Window::windowRefreshCallback(GLFWwindow *window){
    auto *appWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!appWindow) {
        std::cout << "Window::windowRefreshCallback() appwindow = null" << std::endl;
        return;
    }
    // 主循环被冻结了？没关系，我们在这里强行画！
    appWindow->render();
}

// 专门初始化 ImGui
void Window::initImGui() {
    // 1. 创建 ImGui 上下文
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // 2. 开启深色主题（可以以后改您的“高达主题”）
    ImGui::StyleColorsDark();

    // 3. 初始化平台和渲染器后端
    ImGui_ImplGlfw_InitForOther(m_window, true);
    // 这里传入 bgfx 的 view id (我们用 255 这个专门留给 UI 的通道)
    ImGui_Implbgfx_Init(255);
}

// 专门绘制按键
void Window::renderBtn() {
    // 创建一个全屏的、透明的、不可见的底层面板，专门用来承载我们的按钮和交互
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(m_width, m_height));
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | 
                                    ImGuiWindowFlags_NoBackground | 
                                    ImGuiWindowFlags_NoSavedSettings | 
                                    ImGuiWindowFlags_NoFocusOnAppearing | 
                                    ImGuiWindowFlags_NoBringToFrontOnFocus;
    
    ImGui::Begin("##FullscreenHUD", nullptr, window_flags);
    
    // 获取这个透明面板的画笔
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // 1. 实例化设置按钮并将其放置在窗口左下角
    float btnRadius = 20.0f;
    float padding = 20.0f; 
    SettingBtn settingBtn({btnRadius + padding, m_height - btnRadius - padding}, btnRadius);
    settingBtn.draw(drawList);
    
    ImGui::End();
}