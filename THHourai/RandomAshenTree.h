#pragma once

namespace Touhou
{
	class RandomAshenTree : public GameObject
	{
		static RefPtr<GameObject> ashenTrees[9];

	public:
		RandomAshenTree( String name );

	private:
		void LoadAssets();
	};
}