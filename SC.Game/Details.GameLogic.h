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

		RefPtr<VisibleViewStorage> mVisibleViewStorage;
		RefPtr<CDeviceContext> mDeviceContext;
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
		void Render( int frameIndex );
		void ResizeBuffers( uint32 width, uint32 height );

		void TerrainBaking( int frameIndex );
		void MeshSkinning( int frameIndex );
		void GeometryLighting( int frameIndex );
		void GeometryWriting( int frameIndex );

		void RenderSceneGraphForEachThreads( object, int threadIndex, std::list<Light*>& lights, int frameIndex );
		void FetchAndSetThread();
	};
}