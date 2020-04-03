#pragma once

namespace SC::Game::Details
{
	class GC abstract
	{
		struct tag_Fence
		{
			ComPtr<ID3D12Fence> pFence;
			uint64 mFenceValue;
		};

		struct tag_FrameIndex
		{
			int mFrameIndex;
			int mDelay;
		};

		struct tag_Garbage
		{
			ComPtr<IUnknown> pUnknown;
			std::variant<tag_Fence, tag_FrameIndex> mVariant;
		};

		static std::mutex mLocker;
		static std::vector<tag_Garbage> mGarbages;
		static std::size_t mCount;
		static bool mDisposed;

	public:
		static void Initialize();
		static void Add( ComPtr<ID3D12Fence> pFence, uint64 fenceValue, ComPtr<IUnknown> pUnknown );
		static void Add( int frameIndex, ComPtr<IUnknown> pUnknown, int delay );
		static void Collect( int frameIndex = -1 );

	private:
		static void Dispose( object sender );
		static void Push( tag_Garbage&& garbage );
	};
}