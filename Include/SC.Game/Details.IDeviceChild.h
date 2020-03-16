#pragma once

namespace SC::Game::Details
{
	/// <summary> �׷��� ��ġ���� �Ļ��� ��ü�� ���� ���� �������̽��� �����մϴ�. </summary>
	struct IDeviceChild : virtual public Object
	{
		/// <summary> �� ��ü�� ������ ��ġ ��ü�� �����ɴϴ�. </summary>
		virtual RefPtr<IDevice> GetDevice() = 0;
	};
}