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

	// ������ ������ �ۼ��մϴ�.
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

	// ������ ������ �ۼ��մϴ�.
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

	// ���� ����� �������� ������ �ְ� ������ ť ũ�⸦ �����մϴ�.
	size_t maxCount = 0;

	for ( size_t i = 0; i < mCount; ++i )
	{
		// ������ ������ �����ɴϴ�.
		auto& g = mGarbages[i];

		if ( g.pUnknown )
		{
			if ( g.mType == 0 )
			{
				// ������ ��� ��ü�� ����� �Ϸ�Ǿ����� �˻��մϴ�.
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
				// ������ ��� ��ü�� ����� �Ϸ�Ǿ����� �˻��մϴ�.
				if ( g.mFrameIndex.mFrameIndex == frameIndex )
				{
					// ������ ��� ���� ��ü�� �����մϴ�.
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

	// ť ũ�⸦ ������Ʈ�մϴ�.
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
		// �뷮�� ������ ��� �ø��ϴ�.
		if ( mGarbages.size() <= mCount )
		{
			mGarbages.resize( mGarbages.size() + 512 );
		}

		// ������ ��ü�� �߰��մϴ�.
		mGarbages[mCount++] = move( garbage );
	}
}