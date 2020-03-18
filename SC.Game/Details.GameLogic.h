#pragma once

namespace SC::Game::Details
{
	class GameLogic : virtual public Object
	{
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

		// 멀티 스레드를 위한 추가 디바이스 컨텍스트입니다.
		ComPtr<ID3D12CommandAllocator> pCommandAllocatorLights[2];
		RefPtr<CDeviceContext> mDeviceContextLights;
		ComPtr<ID3D12CommandAllocator> pCommandAllocatorRender[2];
		RefPtr<CDeviceContext> mDeviceContextRender;

	public:
		GameLogic();
		~GameLogic();

		void Update();
		void FixedUpdate();
		void Render();
		void ResizeBuffers( uint32 width, uint32 height );
	};
}