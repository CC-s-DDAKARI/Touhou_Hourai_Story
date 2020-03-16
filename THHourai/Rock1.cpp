using namespace Touhou;

RefPtr<GameObject> Rock1::model;

Rock1::Rock1( String name ) : GameObject( name )
{
	IsLocked = true;

	LoadAssets();
	model->Clone().As<GameObject>()->Parent = this;

	var boxCol = AddComponent<BoxCollider>();
	boxCol->Center = Vector3( 0, 1, 0 );
	boxCol->Extent = Vector3( 2, 1, 2 );

#if false && defined( _DEBUG )
	var go = new ColliderBoxVisualizer( "debugVisualizer" );
	go->Parent = this;

	go->Transform->Scale = Vector3( 2, 1, 2 );
	go->Transform->Position = Vector3( 0, 1, 0 );
#endif

	IsLocked = false;
}

void Rock1::LoadAssets()
{
	if ( !model )
	{
		model = AssetBundle::LoadModelAssetsAsync( "Assets/Model/Rock/scene.gltf" )->GetResults().As<GameObject>();

		model->Transform->Scale = 0.3;
		model->Transform->Position = Vector3( 0, 1.1, 0 );
		model->Transform->Rotation = Quaternion::AngleAxis( 3.14 * 0.5, Vector3::Right );
	}
}