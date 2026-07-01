//
// Created by MK-10 on 2026/7/1.
//


#include "theme/starryTheme.h"
#include <cstdlib>
#include <cmath>
#define START_COUNT 96000

// 辅助函数：生成一个带权重的随机颜色（蓝白、纯白、橘黄）
ImU32 generateStarColor(float baseAlpha) {
    int roll = rand() % 100;
    int a = (int)baseAlpha;
    if (roll < 20) { // 20% 橘黄/红色老恒星
        return IM_COL32(255, 200, 150, a);
    } else if (roll < 40) { // 20% 亮蓝色年轻恒星
        return IM_COL32(180, 220, 255, a);
    } else { // 60% 纯白主序星
        return IM_COL32(255, 255, 255, a);
    }
}

// 辅助函数：生成趋向于中心的随机数（极简版正态分布近似）
float gaussianRandom() {
    return ((float)(rand() % 1000) + (float)(rand() % 1000) + (float)(rand() % 1000)) / 3000.0f;
}

StarryTheme::StarryTheme() : m_time(0.0f) {
    // 宇宙依然是 4000x4000
    m_worldWidth = 4000.0f;
    m_worldHeight = 4000.0f;

    for (int i = 0; i < START_COUNT; i++) {
        Star s;
        //  魔法1：制造银河对角线 (从左上到右下)
        if (rand() % 100 < 70) {
            // 70% 的星星聚集在银河带附近
            s.x = (float)(rand() % (int)m_worldWidth);
            // y 坐标以一条对角线为基准，加上一个正态分布的偏移量
            float diagonalY = s.x * 0.8f; // 银河的倾斜度
            float offset = (gaussianRandom() - 0.5f) * 1500.0f; // 聚集带的宽度
            s.y = diagonalY + offset;
        } else {
            // 30% 的星星依然散布全宇宙
            s.x = (float)(rand() % (int)m_worldWidth);
            s.y = (float)(rand() % (int)m_worldHeight);
        }

        //  星星大小两极分化：极少数超级大星，绝大多数小碎星
        if (rand() % 100 < 2) {
            s.radius = (float)(rand() % 100) / 100.0f * 2.5f + 2.0f; // 大星
        } else {
            s.radius = (float)(rand() % 100) / 100.0f * 1.0f + 0.3f; // 碎星
        }

        s.baseAlpha = (float)(rand() % 155 + 100);
        s.phase = (float)(rand() % 314) / 100.0f;
        s.speed = (float)(rand() % 50) / 100.0f + 0.05f;

        m_stars.push_back(s);
    }
}


uint32_t StarryTheme::getClearColor() {
    // 配合星云，把底层背景压到极暗的纯深空黑紫色
    return 0x050510FF;
}

void StarryTheme::render(ImDrawList* drawList, int windowWidth, int windowHeight) {
    ImGuiIO& io = ImGui::GetIO();

    //---------------- 状态侦测与交互 ----------------
    bool isInteracting = false;

    // 鼠标是否在移动？
    if (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f) {
        isInteracting = true;
    }

    // 窗口尺寸是否发生了变化？(即用户在拉伸窗口)
    if (m_lastWindowWidth != 0 && (windowWidth != m_lastWindowWidth || windowHeight != m_lastWindowHeight)) {
        isInteracting = true;
    }

    // 更新上一次的窗口尺寸
    m_lastWindowWidth = (float)windowWidth;
    m_lastWindowHeight = (float)windowHeight;

    // 根据用户行为更新状态机
    if (isInteracting) {
        // 一旦有动静，立即打断，计时器归零
        m_idleTimer = 0.0f;

        // 如果是按住拖拽，听从用户的控制
        if (ImGui::IsMouseDragging(0)) {
            m_cameraX -= io.MouseDelta.x;
            m_cameraY -= io.MouseDelta.y;
        }
    } else {
        // 如果安静了，就开始默默计时
        m_idleTimer += io.DeltaTime;
    }


    //---------------- 电影模式 ----------------
    // 安静超过 1.0 秒后触发柔和游场模式
    if (m_idleTimer >= 1.0f) {
        // 缓动进入 (Ease-In)：用 2 秒的时间，让速度倍率从 0.0 过渡到 1.0
        float easeMultiplier = (m_idleTimer - 1.0f) / 2.0f;
        if (easeMultiplier > 1.0f) {
            easeMultiplier = 1.0f;
        }

        // 累加相机的专属游场时间
        m_panTime += io.DeltaTime;

        // 无重力漂浮轨迹：利用 sin 和 cos 波形，叠加出柔和的曲线移动方向
        float driftX = 15.0f + std::sin(m_panTime * 0.4f) * 20.0f;
        float driftY = 10.0f + std::cos(m_panTime * 0.25f) * 15.0f;

        // 乘以 io.DeltaTime 保证帧率无关
        m_cameraX += driftX * easeMultiplier * io.DeltaTime;
        m_cameraY += driftY * easeMultiplier * io.DeltaTime;
    }


    //---------------- 背景星云渲染 ----------------
    m_time += 0.02f;

    // 绘制巨大的“星云”伪装色，晕染深紫和暗红的宇宙气体
    // 视差效果：乘以 0.5 让星云移动得比星星慢，取模让星云也能无限拖拽
    float nebula1X = std::fmod(1000.0f - m_cameraX * 0.5f, m_worldWidth);
    float nebula1Y = std::fmod(1000.0f - m_cameraY * 0.5f, m_worldHeight);
    if (nebula1X < 0) nebula1X += m_worldWidth;
    if (nebula1Y < 0) nebula1Y += m_worldHeight;
    drawList->AddCircleFilled({nebula1X, nebula1Y}, 800.0f, IM_COL32(120, 50, 150, 8), 64);

    float nebula2X = std::fmod(2500.0f - m_cameraX * 0.5f, m_worldWidth);
    float nebula2Y = std::fmod(2000.0f - m_cameraY * 0.5f, m_worldHeight);
    if (nebula2X < 0) nebula2X += m_worldWidth;
    if (nebula2Y < 0) nebula2Y += m_worldHeight;
    drawList->AddCircleFilled({nebula2X, nebula2Y}, 1000.0f, IM_COL32(200, 80, 50, 6), 64);


    //---------------- 画星星 ----------------
    for (auto& star : m_stars) {
        // 计算闪烁透明度
        float twinkle = std::sin(m_time * 3.0f + star.phase);
        int currentAlpha = (int)(star.baseAlpha + twinkle * 80.0f);
        if (currentAlpha > 255) currentAlpha = 255;
        if (currentAlpha < 0) currentAlpha = 0;

        // 星星自身的微动
        star.x -= star.speed * 0.2f;
        star.y -= star.speed * 0.1f;

        // 飞出世界边界的处理
        if (star.x < 0) star.x += m_worldWidth;
        if (star.y < 0) star.y += m_worldHeight;

        // 世界坐标转屏幕坐标
        float screenX = std::fmod(star.x - m_cameraX, m_worldWidth);
        if (screenX < 0) screenX += m_worldWidth;
        float screenY = std::fmod(star.y - m_cameraY, m_worldHeight);
        if (screenY < 0) screenY += m_worldHeight;

        // 视锥剔除 (Frustum Culling)
        if (screenX < -10 || screenX > windowWidth + 10 ||
            screenY < -10 || screenY > windowHeight + 10) {
            continue;
        }

        // 获取星星色温
        ImU32 color = generateStarColor(currentAlpha);

        // 为较大的星星添加光晕特效 (Bloom)
        if (star.radius > 1.5f) {
            drawList->AddCircleFilled({screenX, screenY}, star.radius * 2.5f, color & 0x00FFFFFF | (currentAlpha / 4) << 24, 8);
        }

        // 绘制星星实体
        drawList->AddCircleFilled({screenX, screenY}, star.radius, color, 4);
    }
}