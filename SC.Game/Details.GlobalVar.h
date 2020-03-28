#pragma once

namespace SC::Game::Details
{
	struct tag_GlobalVar
	{
		std::mutex globalMutex;

		Drawing::Point<double> scrollDelta;

		int frameIndex = 0;
	};

	extern tag_GlobalVar GlobalVar;
	extern bool AppShutdown;
}