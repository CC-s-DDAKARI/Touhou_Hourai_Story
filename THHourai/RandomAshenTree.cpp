using namespace Touhou;

RefPtr<GameObject> RandomAshenTree::ashenTrees[9];

RandomAshenTree::RandomAshenTree( String name ) : GameObject( name )
{
	IsLocked = true;

	LoadAssets();

	// 간단한 랜덤 함수를 사용합니다.
	auto rd = rand() % ( sizeof( ashenTrees ) / sizeof( ashenTrees[0] ) );
	var mytree = ashenTrees[rd]->Clone().As<GameObject>();
	mytree->Parent = this;

	IsLocked = false;
}

void RandomAshenTree::LoadAssets()
{
	if ( !ashenTrees[0] )
	{
		for ( int i = 0; i < 10; ++i )
		{
			ashenTrees[i] = AssetBundle::LoadModelAssetsAsync( String::Format( "Assets/Model/Trees/AshenTree{0}.mdl", i ).Chars )->GetResults().As<GameObject>();
			ashenTrees[i]->Transform->Scale = 0.01;
		}
	}
}