#pragma once

namespace SC::Game::Details
{
	/// <summary> 그래픽 자원을 표현합니다. </summary>
	struct IResource : virtual public Object, virtual public IDeviceChild, virtual public IDisposable
	{

	};
}