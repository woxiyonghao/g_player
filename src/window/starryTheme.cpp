//
// Created by MK-10 on 2026/7/1.
//


#include "theme/starryTheme.h"
#include <cstdlib>
#include <cmath>
#define START_COUNT 16000
StarryTheme::StarryTheme(int width, int height):m_time(0.0f) {
    // 诞生 START_COUNT 颗星星
    for (int i = 0; i < START_COUNT; i ++) {
        Star s;
        s.x = (float)(rand() % width);
        s.y = (float)(rand() % height);
        s.radius = (float)(rand() % 100) / 100.0f * 1.5f + 0.5f;
        s.baseAlpha = (float)(rand() % 155 + 100);
        s.phase = (float)(rand() % 314) / 100.0f;
        s.speed = (float)(rand() % 50) / 100.0f + 0.1f;
        m_stars.push_back(s);
    }
}

uint32_t StarryTheme::getClearColor() {
    // 动态计算深邃的呼吸蓝背景色
    m_time += 0.02f;
    float colorFactor = (std::sin(m_time) * 0.5f) + 0.5f;
    uint8_t r = static_cast<uint8_t>(colorFactor * 255 * 0.1f); // 压暗红色，让夜空更深邃
    uint8_t g = 0x20;
    uint8_t b = 0x60;
    return (static_cast<uint32_t>(r) << 24) | (static_cast<uint32_t>(g) << 16) | (static_cast<uint32_t>(b) << 8) | 0xFF;
}

void StarryTheme::render(ImDrawList* drawList, int windowWidth, int windowHeight) {
    // 遍历并画出每一颗闪烁微动的星星
    for (auto& star : m_stars) {
        float twinkle = std::sin(m_time * 3.0f + star.phase);
        int currentAlpha = (int)(star.baseAlpha + twinkle * 80.0f);

        if (currentAlpha > 255) currentAlpha = 255;
        if (currentAlpha < 0) currentAlpha = 0;

        // 让星星整体向左上角微微飘动
        star.x -= star.speed * 0.2f;
        star.y -= star.speed * 0.1f;

        // 飞出屏幕的星星从对面重生
        if (star.x < 0) star.x = (float)windowWidth;
        if (star.y < 0) star.y = (float)windowHeight;

        ImU32 color = IM_COL32(255, 255, 255, currentAlpha);
        // 画出星星
        drawList->AddCircleFilled({star.x, star.y}, star.radius, color, 4);
    }
}

