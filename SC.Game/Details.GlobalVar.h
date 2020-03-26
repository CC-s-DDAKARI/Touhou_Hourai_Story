#pragma once

namespace SC::Game::Details
{
	struct tag_GlobalVar
	{
		String systemMessageFontName;
		std::mutex globalMutex;

		RefPtr<GameLogic> gameLogic;
		Drawing::Point<double> scrollDelta;

		RefPtr<UI::TextFormat> defaultTextFormat;
		std::set<GlyphBuffer*> glyphBuffers;

		int frameIndex = 0;

		std::set<physx::PxScene*> pxSceneList;
		std::set<physx::PxRigidActor*> pxRigidActor;

		void InitializeComponents();
		void Release();
	};

	extern tag_GlobalVar GlobalVar;
	extern bool AppShutdown;
	extern GarbageCollector GC;
}