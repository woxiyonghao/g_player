//
// Created by MK-10 on 2026/6/4.
//
#include <iostream>
#include <ostream>

#include "window/window.h"
#include <string>

#include "GLFW//glfw3.h"
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
    if (m_window) {
        // 清理 GLFW 分配的window内存
        glfwDestroyWindow(m_window);
    }
    // 彻底清理 GLFW 分配的底层资源
    glfwTerminate();
}

/// 初始化函数,返回是否成功
bool Window::init() {
    std::cout << "Window::init()" << std::endl;
    // 1. 初始化 GLFW 库
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW!" << std::endl;
        return false;
    }
    // 2. 配置 macOS 必需的 OpenGL 核心模式
    // 使用 OpenGL 3.3 版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // 核心模式 (Core Profile)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    // 苹果专属前向兼容
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 3. 创建底层的 C 语言窗口对象
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), NULL, NULL);
    if (!m_window) {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate(); // 释放 glfwInit()占用的内存
        return false;
    }

    // 4. 设置当前上下文 所有图形绘制命令，全都画在这个 m_window 对应的画板上
    glfwMakeContextCurrent(m_window);

    // 5. 把 C++ 的 this 指针，强行存进 C 语言的 GLFW 窗口里
    glfwSetWindowUserPointer(m_window, static_cast<void*>(this));

    // 6. glfw 注册屏幕刷新回调
    glfwSetFramebufferSizeCallback(m_window, framebufferResizeCallback);

    // 7. glfw 注册keyboard回调
    glfwSetKeyCallback(m_window, keyCallback);

    return true;
}

/// 每一帧的更新逻辑（处理事件、交换缓冲）
void Window::update() {
    
    // 处理按键、鼠标、窗口拖动
    glfwPollEvents();

    // 交换双缓冲，把画好的东西显示到屏幕上
    glfwSwapBuffers(m_window);
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
    if (!appWindow) {
        std::cout << "Window::framebufferResizeCallback() appwindow = null" << std::endl;
        return;
    }
    appWindow->m_width = width;
    appWindow->m_height = height;
    std::cout << "[Window Event] 窗口大小已改变 -> 宽: " << width
                  << " | 高: " << height << std::endl;
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