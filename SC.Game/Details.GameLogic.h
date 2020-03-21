#pragma once

namespace SC::Game::Details
{
	class GameLogic : virtual public Object
	{
		friend class Application;

		struct tag_GraphPair
		{
			int ThreadIndex;
			std::list<GameObject*> Graph;
		};

		RefPtr<Diagnostics::StepTimer> timer;
		RefPtr<Diagnostics::StepTimer> physicsTimer;

		ComPtr<ID3D12CommandAllocator> pCommandAllocator[2];
		uint64 lastPending[2]{ };
		RefPtr<CDeviceContext> deviceContext;
		RefPtr<VisibleViewStorage> visibleViewStorage;
		RefPtr<Threading::Event> waitingHandle;

		RefPtr<GeometryBuffer> geometryBuffer;
		RefPtr<HDRBuffer> hdrBuffer;
		RefPtr<HDRComputedBuffer> hdrComputedBuffer;

		/* Default assets */
		RefPtr<Mesh> skyboxMesh;

		RefPtr<Scene> currentScene;

		// 멀티 스레드를 위한 추가 개체입니다.
		std::atomic<int> mCompletedValue;
		int mCompletedGoal;
		Threading::Event mCompletedEvent;

		int mLightThreads;

	public:
		GameLogic();
		~GameLogic();

		void Update();
		void FixedUpdate();
		void Render( int frameIndex, int fixedFrameIndex );
		void ResizeBuffers( uint32 width, uint32 height );

		void MeshSkinning( int frameIndex, int fixedFrameIndex );
		void GeometryWriting( int frameIndex, int fixedFrameIndex );

		void RenderSceneGraphForEachThreads( object, int threadIndex, std::list<Light*>& lights, int frameIndex, int fixedFrameIndex );
		void FetchAndSetThread();
	};
}