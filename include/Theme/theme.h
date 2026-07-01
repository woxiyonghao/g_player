//
// Created by MK-10 on 2026/7/1.
//

#ifndef G_PLAYER_THEME_H
#define G_PLAYER_THEME_H
#include <imgui.h>
#include <cstdint>
// 这是一个纯虚类（接口），规定了所有主题必须具备的能力
class Theme {
public:
    virtual ~Theme() = default;
    // 1. 告诉 Window，当前主题的背景底色是什么
    virtual uint32_t getClearColor() = 0;
    // 2. 告诉 Window，当前主题的内容要怎么画
    virtual void render(ImDrawList* drawList, int windowWidth, int windowHeight) = 0;
};

#endif //G_PLAYER_THEME_H
