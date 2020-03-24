using namespace Touhou;

using namespace SC::Drawing;

using namespace std;

Scene0_Terrain::Scene0_Terrain( String name ) : GameObject( name )
{
	IsLocked = true;
	InitializeComponents();
	IsLocked = false;
}

void Scene0_Terrain::InitializeComponents()
{
	// 바닥
	declare_element( GameObject, terrain );
	auto ter = terrain->AddComponent<Terrain>();
	auto trc = terrain->AddComponent<TerrainCollider>();
	trc->Center = Vector3( -50, 0, -50 );
	ter->HeightMap = new HeightMap( "heightMap", "Assets/Texture/HeightMap.raw", 969, 969 );

	var mat = new Material( "terrainMat" );
	mat->DiffuseMap = new Texture2D( "diffuseMap", "Assets/Texture/templeBase01_diffuse.jpg" );
	mat->NormalMap = new Texture2D( "diffuseMap", "Assets/Texture/templeBase01_normal.jpg" );
	mat->TexScale = 100;
	ter->Material = mat;

	auto trp = terrain->Transform;
	trp->Scale = Vector3( 50, 3, 50 );
	trp->Position = Vector3( 40, 0, -40 );
	terrain->Parent = this;

#if false && defined( _DEBUG )
	var go = new ColliderBoxVisualizer( "debugVisualizer" );
	go->Parent = terrain;

	go->Transform->Scale = Vector3( 1, 1, 1 );
	go->Transform->Position = Vector3( 0, -1, 0 );
#endif

	// 나무 배치
	vector<Vector3> placements;

	for ( int j = 0; j < 8; ++j )
	{
		for ( int i = 0; i < 50; ++i )
		{
			auto place = Vector3( -9 + j, 0, 9 - i * 1 );
			placements.push_back( place );
		}
	}

	for ( int i = 0; i < ( int )placements.size(); ++i )
	{
		var tree = new RandomAshenTree( String::Format( "tree{0}", i ) );
		tree->Transform->Position = placements[i];
		tree->Parent = this;

		auto td = tree->AddComponent<ThreadDispatcher>();
		td->ThreadID = ( i / 200 );
	}

	// 테스트
	RefPtr<GameObject> test01 = new ColliderBoxVisualizer( "test01" );
	test01->AddComponent<Rigidbody>();
	test01->AddComponent<BoxCollider>();
	test01->Transform->Position = Vector3( 3, 1, 3 );
	test01->Parent = this;

	test01 = new ColliderBoxVisualizer( "test02" );
	test01->AddComponent<Rigidbody>();
	test01->AddComponent<BoxCollider>();
	test01->Transform->Position = Vector3( 5, 1, 3 );
	test01->Parent = this;

	test01 = test01->Clone().As<GameObject>();
	test01->Transform->Position = Vector3( 5, 1, 1 );
	test01->Parent = this;

	test01 = test01->Clone().As<GameObject>();
	test01->Transform->Position = Vector3( 5, 1, -1 );
	test01->Parent = this;

	test01 = test01->Clone().As<GameObject>();
	test01->Transform->Position = Vector3( 5, 1, -3 );
	test01->Parent = this;

	// 테스트2
	var trigger = new GameObject( "testTrigger" );
	auto boxCol = trigger->AddComponent<BoxCollider>();
	boxCol->Center = Vector3( 0, 1, 0 );
	boxCol->HalfExtents = Vector3( 1, 1, 1 );
	boxCol->IsTrigger = true;
	trigger->Transform->Position = Vector3( 5, 1, -5 );
	trigger->Parent = this;
}