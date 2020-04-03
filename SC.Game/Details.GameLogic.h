#pragma once

namespace SC::Game::Details
{
	class GameLogic abstract
	{
	public:
		static Diagnostics::StepTimer mTimer;
		static Diagnostics::StepTimer mPhysicsTimer;
		static Diagnostics::StepTimer mRenderTimer;

		static RefPtr<VisibleViewStorage> mViewStorage;
		static RefPtr<CDeviceContext> mDeviceContext;
		static RefPtr<CDeviceContext> mDeviceContextCommit;

		static RefPtr<GeometryBuffer> mGeometryBuffer;
		static RefPtr<HDRBuffer> mHDRBuffer;
		static RefPtr<HDRComputedBuffer> mHDRComputedBuffer;

		static RefPtr<Mesh> mSkyboxMesh;
		static RefPtr<Scene> mCurrentScene;

		// 멀티 스레드를 위한 추가 개체입니다.
		static std::atomic<int> mCompletedValue;
		static int mCompletedGoal;
		static Threading::Event mCompletedEvent;
		static int mLightThreads;

		static bool mDisposed;

	public:
		static void Initialize();
		static void Update();
		static void FixedUpdate();
		static void Render( int frameIndex );

	private:
		static void Dispose( object sender );
		static void ResizeApp( object sender, Drawing::Point<int> size );

		static void CommitBuffer( int frameIndex );
		static void TerrainBaking( int frameIndex );
		static void MeshSkinning( int frameIndex );
		static void GeometryLighting( int frameIndex );
		static void GeometryWriting( int frameIndex );

		static void RenderSceneGraphForEachThreads( object, int threadIndex, std::list<Light*>& lights, int frameIndex );
		static void FetchAndSetThread();
	};
}