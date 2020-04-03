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

void GC::Add( ComPtr<ID3D12Fence> pFence, uint64 fenceValue, ComPtr<IUnknown> pUnknown )
{
	lock_guard<mutex> lock( mLocker );

	// ������ ������ �ۼ��մϴ�.
	tag_Fence fence;
	fence.pFence = move( pFence );
	fence.mFenceValue = fenceValue;

	tag_Garbage g;
	g.pUnknown = move( pUnknown );
	g.mVariant = move( fence );

	Push( move( g ) );
}

void GC::Add( int frameIndex, ComPtr<IUnknown> pUnknown, int delay )
{
	lock_guard<mutex> lock( mLocker );

	// ������ ������ �ۼ��մϴ�.
	tag_FrameIndex frame;
	frame.mFrameIndex = frameIndex;
	frame.mDelay = delay;

	tag_Garbage g;
	g.pUnknown = move( pUnknown );
	g.mVariant = move( frame );

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
			if ( g.mVariant.index() == 0 )
			{
				auto& fence = get<0>( g.mVariant );
				// ������ ��� ��ü�� ����� �Ϸ�Ǿ����� �˻��մϴ�.
				if ( fence.pFence->GetCompletedValue() > fence.mFenceValue )
				{
					g.pUnknown = nullptr;

					fence.pFence = nullptr;
					fence.mFenceValue = 0;
				}
				else
				{
					maxCount = i;
				}
			}
			else if ( g.mVariant.index() == 1 )
			{
				auto& frame = get<1>( g.mVariant );

				// ������ ��� ��ü�� ����� �Ϸ�Ǿ����� �˻��մϴ�.
				if ( frame.mFrameIndex == frameIndex )
				{
					// ������ ��� ���� ��ü�� �����մϴ�.
					if ( ( frame.mDelay ) -= 1 <= 0 )
					{
						g.pUnknown = nullptr;

						continue;
					}
				}
				else
				{
					maxCount = i;
				}
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