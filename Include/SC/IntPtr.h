#pragma once

namespace SC
{
	/// <summary> ����Ƽ�� ������ ���Ŀ� ���� ���� �Լ��� �����մϴ�. </summary>
	struct IntPtr : public ValueType, virtual public IEquatable<void*>, virtual public IComparable<void*>
	{
		/// <summary> ����Ƽ�� ������ ���� �����մϴ�. </summary>
		void* Value;

		/// <summary> <see cref="IntPtr"/> ����ü�� �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		IntPtr();

		/// <summary> <see cref="IntPtr"/> ����ü�� �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		IntPtr( void* ptr );

		/// <summary> <see cref="IntPtr"/> ����ü�� �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		IntPtr( const IntPtr& copy );

		/// <summary> (<see cref="Object"/> Ŭ�������� ��� ��.) ���� ��ü�� �ؽ�Ʈ ǥ���� �����Ͽ� ��ȯ�մϴ�. </summary>
		String ToString() override;

		/// <summary> (<see cref="Object"/> Ŭ�������� ��� ��.) ������ ��ü�� ���� ��ü�� ������ Ȯ���մϴ�. </summary>
		/// <param name="obj"> ���� ��ü�� �����մϴ�. </param>
		bool Equals( object obj ) override;

		/// <summary> (<see cref="IEquatable"/> �������̽����� ���� ��.) ���� ��ü�� ������ ������ �ٸ� ��ü�� �������� ���մϴ�. </summary>
		/// <param name="right"> ������ ������ ��ü�� �����մϴ�. </param>
		bool Equals( void* right ) override;

		/// <summary> (<see cref="IEquatable"/> �������̽����� ���� ��.) ���� ��ü�� ������ ������ �ٸ� ��ü�� ���ϰ� ���� �������� ���� ��ü�� ��ġ�� �ٸ� ��ü���� ������, ������ �Ǵ� ���������� ��Ÿ���� ������ ��ȯ�մϴ�. </summary>
		/// <param name="right"> ������ ������ ��ü�� �����մϴ�. </param>
		int CompareTo( void* right ) override;

		operator void*();
		IntPtr& operator=( const IntPtr& copy );
		bool operator==( const IntPtr& right ) const;
		bool operator!=( const IntPtr& right ) const;
		bool operator< ( const IntPtr& right ) const;
		bool operator> ( const IntPtr& right ) const;
		bool operator<=( const IntPtr& right ) const;
		bool operator>=( const IntPtr& right ) const;
	};
}