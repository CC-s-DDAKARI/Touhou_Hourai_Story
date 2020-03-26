#pragma once

namespace SC::Game::Details
{
	struct tag_GlobalVar
	{
		Application* pApp;
		HWND hWnd;
		String systemMessageFontName;
		std::mutex globalMutex;

		RefPtr<GameLogic> gameLogic;
		Drawing::Point<double> scrollDelta;

		RefPtr<UI::TextFormat> defaultTextFormat;
		std::set<GlyphBuffer*> glyphBuffers;

		int frameIndex = 0;

		physx::PxDefaultAllocator pxDefaultAllocator;
		physx::PxDefaultErrorCallback pxDefaultErrorCallback;

		physx::PxFoundation* pxFoundation = nullptr;
		physx::PxPvd* pxPvd = nullptr;
		physx::PxPvdTransport* pxPvdTransport = nullptr;
		physx::PxPhysics* pxDevice = nullptr;
		physx::PxCooking* pxCooking = nullptr;
		physx::PxDefaultCpuDispatcher* pxDefaultDispatcher = nullptr;
		physx::PxCudaContextManager* pxCudaManager = nullptr;

		std::set<physx::PxScene*> pxSceneList;
		std::set<physx::PxRigidActor*> pxRigidActor;

		void InitializeComponents();
		void Release();
	};

	extern tag_GlobalVar GlobalVar;
	extern bool AppShutdown;
	extern GarbageCollector GC;
}