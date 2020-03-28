#pragma once

namespace SC::Game::Details
{
	class GC abstract
	{
		struct tag_Fence
		{
			ID3D12Fence* pFence;
			uint64 mFenceValue;
		};

		struct tag_FrameIndex
		{
			int mFrameIndex;
			int mDelay;
		};

		struct tag_Garbage
		{
			int mType;
			ComPtr<IUnknown> pUnknown;

			union
			{
				tag_Fence mFence;
				tag_FrameIndex mFrameIndex;
			};

			tag_Garbage()
			{
				mType = 0;
				pUnknown = nullptr;
			}
		};

		static std::mutex mLocker;
		static std::vector<tag_Garbage> mGarbages;
		static std::size_t mCount;
		static bool mDisposed;

	public:
		static void Initialize();
		static void Add( ID3D12Fence* pFence, uint64 fenceValue, IUnknown* pUnknown );
		static void Add( int frameIndex, IUnknown* pPageable, int delay );
		static void Collect( int frameIndex = -1 );

	private:
		static void Dispose( object sender );
		static void Push( tag_Garbage&& garbage );
	};
}