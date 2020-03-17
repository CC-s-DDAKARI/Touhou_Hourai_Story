using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;

SkinnedMeshRendererQueue::SkinnedMeshRendererQueue()
{

}

SkinnedMeshRendererQueue::~SkinnedMeshRendererQueue()
{

}

void SkinnedMeshRendererQueue::Clear()
{
	mSkinnedPair.clear();

	mpAnimator = nullptr;
	mSkinnedMeshRenderers.clear();
}

void SkinnedMeshRendererQueue::PushAnimator( Animator* pAnimator )
{
	if ( mpAnimator && mSkinnedMeshRenderers.size() )
	{
		tag_SkinnedPair pair;

		pair.pAnimator = move( mpAnimator );
		pair.SkinnedMeshRenderers = move( mSkinnedMeshRenderers );

		mSkinnedPair.push_back( pair );
	}

	mpAnimator = pAnimator;
}

void SkinnedMeshRendererQueue::AddRenderer( SkinnedMeshRenderer* pRenderer )
{
	if ( mpAnimator )
	{
		mSkinnedMeshRenderers.push_back( pRenderer );
	}
}

list<SkinnedMeshRendererQueue::tag_SkinnedPair>& SkinnedMeshRendererQueue::GetCollections()
{
	return mSkinnedPair;
}