//
// Created by MK-10 on 2026/7/1.
//

#ifndef G_PLAYER_STARRYTHEME_H
#define G_PLAYER_STARRYTHEME_H
#include "Theme.h"
#include <vector>

// 星星的基因结构
struct Star {
    float x, y,radius,baseAlpha,phase,speed;
};


class StarryTheme: public Theme {
private:
    std::vector<Star> m_stars;
    float m_time; // 用来控制呼吸和闪烁的全局时间
public:
    // 构造时需要知道窗口大小，以便生成满天繁星
    StarryTheme(int width, int height);

    // 实现接口方法
    uint32_t getClearColor() override;
    void render(ImDrawList* drawList, int windowWidth, int windowHeight) override;
};
#endif //G_PLAYER_STARRYTHEME_H
