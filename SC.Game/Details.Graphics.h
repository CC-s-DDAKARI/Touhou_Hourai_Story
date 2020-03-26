#pragma once

namespace SC::Game::Details
{
	class Graphics abstract
	{
	public:
		static RefPtr<IntegratedFactory> mFactory;
		static RefPtr<CDevice> mDevice;
		static RefPtr<CSwapChain> mSwapChain;

	public:
		static void Initialize();
		
	private:
		static void Dispose( object sender );
	};
}