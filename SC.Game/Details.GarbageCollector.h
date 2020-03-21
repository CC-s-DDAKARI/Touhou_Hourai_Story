#pragma once

namespace SC::Game::Details
{
	class GarbageCollector
	{
		std::mutex lock;
		std::queue<std::pair<object, int>> garbages;

		int frameIndex = 0;

	public:
		void Add( const object& garbage );
		void Collect();
		void CollectAll();
	};
}