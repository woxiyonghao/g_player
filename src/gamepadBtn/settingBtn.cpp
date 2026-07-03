#include "gamepadBtn/settingBtn.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <cmath>
#include <iostream>

// 构造函数：固定黑色主题，只需传入位置和大小
SettingBtn::SettingBtn(ImVec2 pos, float radius) 
    : GamepadBtn(pos, radius, IM_COL32(30, 30, 30, 255)) { // 默认深灰黑色
}

void SettingBtn::draw(ImDrawList* drawList) {
    // 1. 设置一个看不见的按钮来接收点击和悬停事件
    ImGui::SetCursorScreenPos(ImVec2(m_pos.x - m_radius, m_pos.y - m_radius));
    // 用 UUID 防止多个按钮 ID 冲突
    ImGui::InvisibleButton("##SettingBtn", ImVec2(m_radius * 2, m_radius * 2));
    
    bool isHovered = ImGui::IsItemHovered();
    bool isActive = ImGui::IsItemActive();
    
    if (isActive) {
        std::cout << "设置按钮被按下了！" << std::endl;
    }

    // 2. 绘制软阴影 (Soft Drop Shadow)
    // 利用多个透明度递减的圆，模拟拟态风的扩散阴影
    if (!isActive) { // 按下时阴影减弱，体现按下去的物理感
        drawList->AddCircleFilled(ImVec2(m_pos.x, m_pos.y + 2.0f), m_radius + 4.0f, IM_COL32(0, 0, 0, 40));
        drawList->AddCircleFilled(ImVec2(m_pos.x, m_pos.y + 4.0f), m_radius + 8.0f, IM_COL32(0, 0, 0, 20));
    } else {
        drawList->AddCircleFilled(ImVec2(m_pos.x, m_pos.y + 1.0f), m_radius + 2.0f, IM_COL32(0, 0, 0, 50));
    }

    // 3. 绘制按钮主体 (黑色背景)
    ImU32 btnColor = m_color;
    if (isActive) {
        btnColor = IM_COL32(20, 20, 20, 255); // 按下时变得更黑
    } else if (isHovered) {
        btnColor = IM_COL32(45, 45, 45, 255); // 悬停时变亮
    }
    
    // 如果悬停，画一个极其细微的白色边框发光
    if (isHovered) {
        drawList->AddCircle(m_pos, m_radius + 1.0f, IM_COL32(255, 255, 255, 60), 32, 2.0f);
    }
    drawList->AddCircleFilled(m_pos, m_radius, btnColor, 32);

    // 4. 纯代码手绘齿轮 (Gear Icon)
    ImU32 iconColor = IM_COL32(240, 240, 240, 255); // 近似白色
    float gear_r = m_radius * 0.45f;
    
    // 画齿轮的外围齿 (4条粗线交叉，形成8个齿)
    float toothThickness = m_radius * 0.22f;
    for (int i = 0; i < 4; ++i) {
        float angle = i * (IM_PI / 4.0f);
        float dx = std::cos(angle) * gear_r;
        float dy = std::sin(angle) * gear_r;
        drawList->AddLine(
            ImVec2(m_pos.x - dx, m_pos.y - dy),
            ImVec2(m_pos.x + dx, m_pos.y + dy),
            iconColor, toothThickness);
    }
    
    // 画齿轮的主体圆心
    drawList->AddCircleFilled(m_pos, gear_r * 0.75f, iconColor, 32);
    
    // 镂空中心圆 (用背景色覆盖)
    drawList->AddCircleFilled(m_pos, gear_r * 0.35f, btnColor, 32);
}
