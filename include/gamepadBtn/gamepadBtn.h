//
// Created by MK-10 on 2026/6/4.
//

#pragma once
#include <imgui.h>
#include <string>
#include <cmath>
// gamepad的基类
class GamepadBtn {
public:
    ImVec2 m_pos;    // 按钮的中心点坐标 (x, y)
    float m_radius;  // 按钮的半径（大小）
    ImU32 m_color;   // 按钮的主题颜色 (格式为 ImGui 的颜色格式)
    // 构造函数：诞生的时候必须交代位置、大小和颜色
    GamepadBtn(ImVec2 pos, float radius, ImU32 color) :m_pos(pos), m_radius(radius), m_color(color) {};
    // 虚析构函数 派生类覆盖指针，防止内存泄漏
    virtual ~GamepadBtn() = default;
    // 纯虚函数：这就是命令所有子类“必须自己实现怎么画自己”
    virtual void draw(ImDrawList* drawList) = 0;
};

class PSCircleBtn:public GamepadBtn {
public:
    PSCircleBtn(ImVec2 pos, float radius, ImU32 color)
        : GamepadBtn(pos, radius, color) {}
    // 实现具体的画法
    void draw(ImDrawList* drawList) override {
        // ImGui 画空心圆 API：中心点, 半径, 颜色, 细分数(32代表很圆), 线条粗细
        drawList->AddCircle(m_pos, m_radius, m_color, 32, 4.0f);
    }
    
};
