//
// Created by MK-10 on 2026/6/4.
//

#ifndef G_PLAYER_WINDOW_H
#define G_PLAYER_WINDOW_H

#include <iostream>
#include <string>
#include <GLFW/glfw3.h>
#include "gamepadBtn/gamepadBtn.h"
#include "Theme/theme.h"
class Window {
public:

    // 当前正在运行的主题
    Theme *m_currentTheme = nullptr;
    
    // 测试PS 的 〇 按钮 (Circle)
    GamepadBtn *m_gamepadBtn;
    void initImGui();   // 专门初始化 ImGui
    void renderBtn();   // 专门绘制按键

    
    /// 构造函数
    Window(int width, int height,const std::string &title);

    /// 析构函数,负责清理资源
    ~Window();

    /// 初始化函数,返回是否成功
    bool init();

    /// 每一帧的更新逻辑（处理事件、交换缓冲）
    void update();

    /// 检查窗口是否应该关闭
    bool shouldClose() const;

    /// bgfx 渲染
    void render();

    /// 获取底层的 GLFWwindow 指针 (供外部集成 bgfx 或 ImGui 使用)
    GLFWwindow *getNativeWindow() const { return m_window ;};

    int getWidth() const { return m_width; }

    int getHeight() const { return m_height; }

    // --- 核心技巧：使用静态方法作为 GLFW 的回调 ---
    /// 窗口大小改变回调
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
    /// 键盘事件回调
    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    /// 窗口位置移动回调
    static void windowPositionCallback(GLFWwindow *window, int posx, int posy);

    /// 系统强制重绘回调
    static void windowRefreshCallback(GLFWwindow *window);
private:
    GLFWwindow *m_window;
    int m_width;
    int m_height;
    const std::string m_title;
};

#endif //G_PLAYER_WINDOW_H