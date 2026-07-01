//
// Created by MK-10 on 2026/7/1.
//

#ifndef G_PLAYER_STARRYTHEME_H
#define G_PLAYER_STARRYTHEME_H
#include "theme.h"
#include <vector>

// 星星的基因结构
struct Star {
    float x, y,radius,baseAlpha,phase,speed;
};


class StarryTheme: public Theme {
private:
    std::vector<Star> m_stars;
    float m_time; // 用来控制呼吸和闪烁的全局时间

    //  世界与相机系统
    float m_cameraX = 0.0f;        // 相机的 X 坐标
    float m_cameraY = 0.0f;        // 相机的 Y 坐标
    float m_worldWidth = 4000.0f;  // 宇宙的绝对宽度
    float m_worldHeight = 4000.0f; // 宇宙的绝对高度

    // 新增：电影感闲置相机的状态
    float m_idleTimer = 0.0f;         // 记录已经多久没操作了（秒）
    float m_lastWindowWidth = 0.0f;   // 记录上一帧的窗口宽度，用来检测缩放
    float m_lastWindowHeight = 0.0f;  // 记录上一帧的窗口高度
    float m_panTime = 0.0f;           // 专门给相机漂浮动画用的时间
public:
    // 构造时需要知道窗口大小，以便生成满天繁星
    StarryTheme();

    // 实现接口方法
    uint32_t getClearColor() override;
    void render(ImDrawList* drawList, int windowWidth, int windowHeight) override;
};
#endif //G_PLAYER_STARRYTHEME_H
