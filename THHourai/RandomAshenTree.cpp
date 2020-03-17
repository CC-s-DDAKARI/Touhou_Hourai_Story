using namespace Touhou;

RandomAshenTree::RandomAshenTree( String name ) : GameObject( name )
{
	IsLocked = true;

	// 간단한 랜덤 함수를 사용합니다.
	auto rd = rand() % 10;

	auto mytree = AssetBundle::LoadModelAssetsAsync( String::Format( "Assets/Model/Trees/AshenTree{0}.mdl", rd ).Chars )->GetResults().As<GameObject>();
	mytree->Transform->Scale = 0.01;
	mytree->Parent = this;

	auto collider = AddComponent<CapsuleCollider>();
	{
		collider->Radius = 0.5;
		collider->Height = 1.0;
		collider->Center = Vector3( 0, 1, 0 );
		collider->Rotation = Quaternion::AngleAxis( 3.14 * 0.5, Vector3::Forward );
	}

	IsLocked = false;
}