#pragma once

namespace SC::Game::Details
{
	/// <summary> 그래픽 장치에서 파생된 개체에 대한 공통 인터페이스를 제공합니다. </summary>
	struct IDeviceChild : virtual public Object
	{
		/// <summary> 이 개체를 생성한 장치 개체를 가져옵니다. </summary>
		virtual RefPtr<IDevice> GetDevice() = 0;
	};
}