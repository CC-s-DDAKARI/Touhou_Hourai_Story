#pragma once

namespace Touhou
{
	class Rock1 : public GameObject
	{
		static RefPtr<GameObject> model;

	public:
		Rock1( String name );

	private:
		void LoadAssets();
	};
}