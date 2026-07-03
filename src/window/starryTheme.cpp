#include "theme/StarryTheme.h"
#include <cstdlib>
#include <cmath>

//---------------- 辅助函数 ----------------
// 生成极简正态分布随机数，用于星河聚集
float gaussianRandom() {
    return ((float)(rand() % 1000) + (float)(rand() % 1000) + (float)(rand() % 1000)) / 3000.0f;
}

// 针对“图二”风格定制的恒星色温 (去除了突兀的橘红色，改为深空冰蓝和纯白)
ImU32 generateStarColor(float baseAlpha) {
    int roll = rand() % 100;
    int a = (int)baseAlpha;
    if (roll < 20) {
        return IM_COL32(100, 180, 255, a); // 冰蓝色
    } else if (roll < 40) {
        return IM_COL32(200, 230, 255, a); // 亮蓝白色
    } else {
        return IM_COL32(255, 255, 255, a); // 纯白色
    }
}

//---------------- 构造与初始化 ----------------
StarryTheme::StarryTheme() : m_time(0.0f) {
    m_worldWidth = 4000.0f;
    m_worldHeight = 4000.0f;

    // 维持大数量星星，但减小半径，突出“锐利感”
    for (int i = 0; i < 16000; i++) {
        Star s;
        if (rand() % 100 < 70) {
            // 银河聚集带
            s.x = (float)(rand() % (int)m_worldWidth);
            float diagonalY = s.x * 0.8f;
            float offset = (gaussianRandom() - 0.5f) * 1500.0f;
            s.y = diagonalY + offset;
        } else {
            // 全局散布
            s.x = (float)(rand() % (int)m_worldWidth);
            s.y = (float)(rand() % (int)m_worldHeight);
        }

        // 彻底取消巨大的星星，全改为锐利的微星
        s.radius = (float)(rand() % 100) / 100.0f * 1.0f + 0.2f;

        s.baseAlpha = (float)(rand() % 155 + 100);
        s.phase = (float)(rand() % 314) / 100.0f;
        s.speed = (float)(rand() % 50) / 100.0f + 0.05f;

        m_stars.push_back(s);
    }
}

uint32_t StarryTheme::getClearColor() {
    // 底色设为最深的纯黑，背景的质感交由 render 中的渐变层来完成
    return 0x000000FF;
}

//---------------- 核心渲染循环 ----------------
void StarryTheme::render(ImDrawList* drawList, int windowWidth, int windowHeight) {
    ImGuiIO& io = ImGui::GetIO();

    //---------------- 状态侦测与交互 ----------------
    bool isInteracting = false;

    if (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f) {
        isInteracting = true;
    }

    if (m_lastWindowWidth != 0 && (windowWidth != m_lastWindowWidth || windowHeight != m_lastWindowHeight)) {
        isInteracting = true;
    }

    m_lastWindowWidth = (float)windowWidth;
    m_lastWindowHeight = (float)windowHeight;

    if (isInteracting) {
        m_idleTimer = 0.0f;
        if (ImGui::IsMouseDragging(0)) {
            m_cameraX -= io.MouseDelta.x;
            m_cameraY -= io.MouseDelta.y;
        }
    } else {
        m_idleTimer += io.DeltaTime;
    }

    //---------------- 电影模式 ----------------
    if (m_idleTimer >= 1.0f) {
        float easeMultiplier = (m_idleTimer - 1.0f) / 2.0f;
        if (easeMultiplier > 1.0f) {
            easeMultiplier = 1.0f;
        }

        m_panTime += io.DeltaTime;

        float driftX = 15.0f + std::sin(m_panTime * 0.4f) * 20.0f;
        float driftY = 10.0f + std::cos(m_panTime * 0.25f) * 15.0f;

        m_cameraX += driftX * easeMultiplier * io.DeltaTime;
        m_cameraY += driftY * easeMultiplier * io.DeltaTime;
    }

    m_time += 0.02f;


    //---------------- 深空背景渲染 (无限视差画布) ----------------
    ImU32 col_top_left = IM_COL32(2, 1, 15, 255);
    ImU32 col_top_right = IM_COL32(10, 15, 40, 255);
    ImU32 col_bottom_right = IM_COL32(20, 50, 120, 255);
    ImU32 col_bottom_left = IM_COL32(5, 20, 60, 255);

    // 将 4000x4000 的世界扩展为 8000x8000 的无缝镜像单元
    float superWorldWidth = m_worldWidth * 2.0f;
    float superWorldHeight = m_worldHeight * 2.0f;

    // 视差滚动核心：乘以 0.15f，让背景移动速度只有前景星星的 15%，营造极强的空间深邃感！
    float bgX = std::fmod(-m_cameraX * 0.15f, superWorldWidth);
    if (bgX > 0.0f) bgX -= superWorldWidth;

    float bgY = std::fmod(-m_cameraY * 0.15f, superWorldHeight);
    if (bgY > 0.0f) bgY -= superWorldHeight;

    // 绘制 2x2 的超大网格，确保无论相机怎么移动，屏幕永远被背景覆盖
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            float startX = bgX + i * superWorldWidth;
            float startY = bgY + j * superWorldHeight;
            float midX = startX + m_worldWidth;
            float midY = startY + m_worldHeight;
            float endX = startX + superWorldWidth;
            float endY = startY + superWorldHeight;

            // 1. 左上角 (正常渐变)
            drawList->AddRectFilledMultiColor({startX, startY}, {midX, midY}, col_top_left, col_top_right, col_bottom_right, col_bottom_left);

            // 2. 右上角 (X轴镜像翻转：边缘颜色完美咬合)
            drawList->AddRectFilledMultiColor({midX, startY}, {endX, midY}, col_top_right, col_top_left, col_bottom_left, col_bottom_right);

            // 3. 左下角 (Y轴镜像翻转)
            drawList->AddRectFilledMultiColor({startX, midY}, {midX, endY}, col_bottom_left, col_bottom_right, col_top_right, col_top_left);

            // 4. 右下角 (X轴与Y轴同时镜像翻转)
            drawList->AddRectFilledMultiColor({midX, midY}, {endX, endY}, col_bottom_right, col_bottom_left, col_top_left, col_top_right);
        }
    }


    //---------------- 渲染星辰 ----------------
    for (auto& star : m_stars) {
        float twinkle = std::sin(m_time * 3.0f + star.phase);
        int currentAlpha = (int)(star.baseAlpha + twinkle * 80.0f);
        if (currentAlpha > 255) currentAlpha = 255;
        if (currentAlpha < 0) currentAlpha = 0;

        star.x -= star.speed * 0.2f;
        star.y -= star.speed * 0.1f;

        if (star.x < 0) star.x += m_worldWidth;
        if (star.y < 0) star.y += m_worldHeight;

        float screenX = std::fmod(star.x - m_cameraX, m_worldWidth);
        if (screenX < 0) screenX += m_worldWidth;
        float screenY = std::fmod(star.y - m_cameraY, m_worldHeight);
        if (screenY < 0) screenY += m_worldHeight;

        if (screenX < -10 || screenX > windowWidth + 10 ||
            screenY < -10 || screenY > windowHeight + 10) {
            continue;
        }

        ImU32 color = generateStarColor(currentAlpha);
        drawList->AddCircleFilled({screenX, screenY}, star.radius, color, 4);
    }
}