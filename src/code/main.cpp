#include <iostream>
#include "window/window.h"
int main() {
    // 1.创建一个自定义的window窗口
    Window appWindow(1024, 768, "g_player - My First C++ Window");
    if (!appWindow.init()) {
        std::cout << "Failed to initialize, app exit...." << std::endl;
        return -1;
    }
    std::cout << "成功创建app window，按ESC可以退出app" << std::endl;

    // 2. 只要没有收到退出信息，那么无限递归刷新app
    while (!appWindow.shouldClose()) {
        appWindow.update();
    }

    // 循环结束，程序准备退出。
    // myApp 走到大括号尽头时，C++ 会自动调用 ~Window() 析构函数清理内存！
    std::cout << "窗口已关闭，程序安全退出。" << std::endl;
    return 0;
}
