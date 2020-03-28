using namespace SC;
using namespace SC::Game::Details;

using namespace std;

mutex GC::mLocker;
vector<GC::tag_Garbage> GC::mGarbages;
size_t GC::mCount;
bool GC::mDisposed;

void GC::Initialize()
{
	App::Disposing += Dispose;
}

void GC::Add( ID3D12Fence* pFence, uint64 fenceValue, IUnknown* pUnknown )
{
	lock_guard<mutex> lock( mLocker );

	// 가비지 정보를 작성합니다.
	tag_Garbage g;
	g.mType = 0;
	g.pUnknown = pUnknown;
	g.mFence.pFence = pFence;
	g.mFence.mFenceValue = fenceValue;

	Push( move( g ) );
}

void GC::Add( int frameIndex, IUnknown* pUnknown, int delay )
{
	lock_guard<mutex> lock( mLocker );

	// 가비지 정보를 작성합니다.
	tag_Garbage g;
	g.mType = 1;
	g.pUnknown = pUnknown;
	g.mFrameIndex.mFrameIndex = frameIndex;
	g.mFrameIndex.mDelay = delay;

	Push( move( g ) );
}

void GC::Collect( int frameIndex )
{
	lock_guard<mutex> lock( mLocker );

	// 연결 종료된 가비지를 제외한 최고 높이의 큐 크기를 저장합니다.
	size_t maxCount = 0;

	for ( size_t i = 0; i < mCount; ++i )
	{
		// 가비지 정보를 가져옵니다.
		auto& g = mGarbages[i];

		if ( g.pUnknown )
		{
			if ( g.mType == 0 )
			{
				// 가비지 대상 개체의 사용이 완료되었는지 검사합니다.
				if ( g.mFence.pFence->GetCompletedValue() >= g.mFence.mFenceValue )
				{
					g.pUnknown = nullptr;

					g.mFence.pFence = nullptr;
					g.mFence.mFenceValue = 0;
				}
				else
				{
					maxCount = i;
				}
			}
			else if ( g.mType == 1 )
			{
				// 가비지 대상 개체의 사용이 완료되었는지 검사합니다.
				if ( g.mFrameIndex.mFrameIndex == frameIndex )
				{
					// 딜레이 경과 이후 개체를 제거합니다.
					if ( ( g.mFrameIndex.mDelay ) -= 1 <= 0 )
					{
						g.pUnknown = nullptr;

						continue;
					}
				}

				maxCount = i;
			}
		}
	}

	// 큐 크기를 업데이트합니다.
	mCount = maxCount;
}

void GC::Dispose( object sender )
{
	mDisposed = true;
}

void GC::Push( tag_Garbage&& garbage )
{
	if ( !mDisposed )
	{
		// 용량이 부족할 경우 늘립니다.
		if ( mGarbages.size() <= mCount )
		{
			mGarbages.resize( mGarbages.size() + 512 );
		}

		// 가비지 개체를 추가합니다.
		mGarbages[mCount++] = move( garbage );
	}
}