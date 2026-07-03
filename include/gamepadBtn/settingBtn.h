#pragma once
#include "gamepadBtn.h"
#include <imgui.h>

class SettingBtn : public GamepadBtn {
public:
    SettingBtn(ImVec2 pos, float radius);
    
    // 覆盖 draw 方法
    void draw(ImDrawList* drawList) override;
};
