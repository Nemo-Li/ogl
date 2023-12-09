//
// Created by Nemo li on 2023/12/9.
//

#ifndef OGL_TIME_H
#define OGL_TIME_H

#include <string>
#include <chrono>

class Time {
public:
    Time();

    ~Time();

    //初始化
    static void Init();

    static void Update();

    //获取游戏运行时间
    static float TimeSinceStartup();

    static float delta_time() { return delta_time_; }

private:
    static std::chrono::system_clock::time_point startup_time_;
    static float last_frame_time_;
    //上一帧花费的时间
    static float delta_time_;
};


#endif //OGL_TIME_H
