#pragma once
#include <algorithm>


struct EngineStats
{
	// Assuming this is called each frame, will also auto count frames
	void SetData( float fps, float dt )
	{
		m_frameCount++;
		m_averageFrameCount++;
		m_framesPerSecond += fps;
		m_deltaTime += dt;
		if (m_frameCount == m_nextAverageFrame)
		{
			const float mult    = 1.0f / static_cast<float>(m_averageFrameCount);
			m_averageFrameCount = 0;

			m_averageFps      = m_framesPerSecond * mult;
			m_averageDt       = m_deltaTime * mult;
			m_framesPerSecond = 0.0f;
			m_deltaTime       = 0.0f;

			// It will update the framerate about four times per second (keep a min and max so it can't break)
			m_nextAverageFrame = m_frameCount + static_cast<int>(std::clamp(m_averageFps * 0.25f, 10.0f, 300.0f));
		}
	}


	[[nodiscard]]
	float GetFPS() const { return m_averageFps; }


	[[nodiscard]]
	float GetDeltaTime() const { return m_averageDt; }


	[[nodiscard]]
	int GetFrameCount() const { return m_frameCount; }

private:

	int   m_frameCount      = 0;
	float m_framesPerSecond = 0.0f;
	float m_deltaTime       = 0.0f;

	int m_nextAverageFrame = 60;

	int   m_averageFrameCount = 0;
	float m_averageFps        = 0.0f;
	float m_averageDt         = 0.0f;
};
