#pragma once

namespace SC::Game::Details
{
	/// <summary> 자원을 서술하는 서술자 개체에 대한 공통 인터페이스를 제공합니다. </summary>
	struct IView : virtual public Object, virtual public IDeviceChild
	{

	};
}