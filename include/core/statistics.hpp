#pragma once
#include <algorithm>

struct EngineStats
{
    // Assuming this is called each frame, will also auto count frames
    void SetData(float fps, float dt)
    {
        frame_count++;
        average_frame_count++;
        frames_per_second += fps;
        delta_time += dt;
        if (frame_count == next_average_frame)
        {
            float mult = 1.0f / static_cast<float>(average_frame_count);
            average_frame_count = 0;

            average_fps = frames_per_second * mult;
            average_dt = delta_time * mult;
            frames_per_second = 0.0f;
            delta_time = 0.0f;

            // It will update the framerate about four times per second (keep a min and max so it can't break)
            next_average_frame = frame_count + static_cast<int>(std::clamp(average_fps * 0.25f, 10.0f, 300.0f));
        }
    }

    float GetFPS()        const { return average_fps; }
    float GetDeltaTime()  const { return average_dt;  }
    int   GetFrameCount() const { return frame_count; }

private:
    int frame_count = 0;
    float frames_per_second = 0.0f;
    float delta_time = 0.0f;

    int next_average_frame = 60;

    int average_frame_count = 0;
    float average_fps = 0.0f;
    float average_dt = 0.0f;
};
