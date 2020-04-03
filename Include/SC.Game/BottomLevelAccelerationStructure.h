#pragma once

namespace SC::Game
{
	/// <summary> 레이 트레이싱에 사용되는 기하 구조의 데이터를 보관합니다. </summary>
	class BottomLevelAccelerationStructure : public Component
	{
		friend class GameObject;
		friend class Details::GameLogic;

		sc_game_export_object( ComPtr<ID3D12Resource> ) mAccel;
		sc_game_export_object( ComPtr<ID3D12Resource> ) mScratch;
		bool mBuilded = false;

		std::vector<MeshRenderer*> mMeshRenderers;
		std::vector<sc_game_export_bytes( D3D12_RAYTRACING_GEOMETRY_DESC, 56 )> mTriangles;

	private:
		void BuildRaytracingAccelerationStructure( RefPtr<Details::CDeviceContext>& deviceContext );

	public:
		/// <summary> <see cref="BottomLevelAccelerationStructure"/> 클래스의 새 인스턴스를 초기화합니다. </summary>
		BottomLevelAccelerationStructure();
		~BottomLevelAccelerationStructure();

		/// <summary> (<see cref="ICloneable"/> 인터페이스에서 구현 됨.) 현재 인스턴스의 복사본인 새 개체를 생성합니다. </summary>
		object Clone() override;

		/// <summary> (<see cref="Component"/> 클래스에서 상속 됨.) 컴포넌트에 대한 논리 갱신을 수행합니다. </summary>
		/// <param name="time"> 게임 시간을 전달합니다. </param>
		/// <param name="input"> 현재 프레임에서 입력 상태를 전달합니다. </param>
		void Update( Time& time, Input& input ) override;

	private:
		void InitializeBuffers();
	};
}