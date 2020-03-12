#pragma once

namespace SC::Game
{
	/// <summary> 물리 재질을 표현합니다. </summary>
	class PhysicsMaterial : public Assets
	{
		sc_game_export_object( physx::PxMaterial* ) pxMaterial = nullptr;

	public:
		/// <summary> <see cref="Material"/> 클래스의 새 인스턴스를 초기화합니다. </summary>
		/// <param name="name"> 데이터 자산의 식별자 이름을 전달합니다. </param>
		PhysicsMaterial( String name );
		~PhysicsMaterial() override;

		/// <summary> (Visual Studio) 물체가 유동 상태일 때 물리적 마찰 계수를 설정하거나 가져옵니다. </summary>
		vs_property( double, DynamicFriction );

		/// <summary> 물체가 유동 상태일 때 물리적 마찰 계수를 가져옵니다. </summary>
		double DynamicFriction_get();

		/// <summary> 물체가 유동 상태일 때 물리적 마찰 계수를 설정합니다. </summary>
		void DynamicFriction_set( double value );

		/// <summary> (Visual Studio) 물체가 정지 상태일 때 물리적 마찰 계수를 설정하거나 가져옵니다. </summary>
		vs_property( double, StaticFriction );

		/// <summary> 물체가 정지 상태일 때 물리적 마찰 계수를 가져옵니다. </summary>
		double StaticFriction_get();

		/// <summary> 물체가 정지 상태일 때 물리적 마찰 계수를 설정합니다. </summary>
		void StaticFriction_set( double value );
	};
}