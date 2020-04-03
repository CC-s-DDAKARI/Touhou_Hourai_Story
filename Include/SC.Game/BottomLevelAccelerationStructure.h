#pragma once

namespace SC::Game
{
	/// <summary> ���� Ʈ���̽̿� ���Ǵ� ���� ������ �����͸� �����մϴ�. </summary>
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
		/// <summary> <see cref="BottomLevelAccelerationStructure"/> Ŭ������ �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		BottomLevelAccelerationStructure();
		~BottomLevelAccelerationStructure();

		/// <summary> (<see cref="ICloneable"/> �������̽����� ���� ��.) ���� �ν��Ͻ��� ���纻�� �� ��ü�� �����մϴ�. </summary>
		object Clone() override;

		/// <summary> (<see cref="Component"/> Ŭ�������� ��� ��.) ������Ʈ�� ���� �� ������ �����մϴ�. </summary>
		/// <param name="time"> ���� �ð��� �����մϴ�. </param>
		/// <param name="input"> ���� �����ӿ��� �Է� ���¸� �����մϴ�. </param>
		void Update( Time& time, Input& input ) override;

	private:
		void InitializeBuffers();
	};
}