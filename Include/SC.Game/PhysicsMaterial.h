#pragma once

namespace SC::Game
{
	/// <summary> ���� ������ ǥ���մϴ�. </summary>
	class PhysicsMaterial : public Assets
	{
		sc_game_export_object( physx::PxMaterial* ) pxMaterial = nullptr;

	public:
		/// <summary> <see cref="Material"/> Ŭ������ �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		/// <param name="name"> ������ �ڻ��� �ĺ��� �̸��� �����մϴ�. </param>
		PhysicsMaterial( String name );
		~PhysicsMaterial() override;

		/// <summary> (Visual Studio) ��ü�� ���� ������ �� ������ ���� ����� �����ϰų� �����ɴϴ�. </summary>
		vs_property( double, DynamicFriction );

		/// <summary> ��ü�� ���� ������ �� ������ ���� ����� �����ɴϴ�. </summary>
		double DynamicFriction_get();

		/// <summary> ��ü�� ���� ������ �� ������ ���� ����� �����մϴ�. </summary>
		void DynamicFriction_set( double value );

		/// <summary> (Visual Studio) ��ü�� ���� ������ �� ������ ���� ����� �����ϰų� �����ɴϴ�. </summary>
		vs_property( double, StaticFriction );

		/// <summary> ��ü�� ���� ������ �� ������ ���� ����� �����ɴϴ�. </summary>
		double StaticFriction_get();

		/// <summary> ��ü�� ���� ������ �� ������ ���� ����� �����մϴ�. </summary>
		void StaticFriction_set( double value );
	};
}