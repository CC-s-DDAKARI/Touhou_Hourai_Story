using namespace Touhou;

RandomAshenTree::RandomAshenTree( String name ) : GameObject( name )
{
	IsLocked = true;

	// 간단한 랜덤 함수를 사용합니다.
	auto rd = rand() % 10;

	var mytree = AssetBundle::LoadModelAssetsAsync( String::Format( "Assets/Model/Trees/AshenTree{0}.mdl", rd ).Chars )->GetResults().As<GameObject>();
	mytree->Transform->Scale = 0.01;
	mytree->Parent = this;

	IsLocked = false;
}