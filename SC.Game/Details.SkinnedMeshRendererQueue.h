#pragma once

namespace SC::Game::Details
{
	class SkinnedMeshRendererQueue : virtual public Object
	{
	public:
		struct tag_SkinnedPair
		{
			Animator* pAnimator = nullptr;
			std::list<SkinnedMeshRenderer*> SkinnedMeshRenderers;
		};

		std::list<tag_SkinnedPair> mSkinnedPair;

		Animator* mpAnimator = nullptr;
		std::list<SkinnedMeshRenderer*> mSkinnedMeshRenderers;

	public:
		SkinnedMeshRendererQueue();
		~SkinnedMeshRendererQueue() override;

		void Clear();
		void PushAnimator( Animator* pAnimator );
		void AddRenderer( SkinnedMeshRenderer* pRenderer );

		std::list<tag_SkinnedPair>& GetCollections();
	};
}