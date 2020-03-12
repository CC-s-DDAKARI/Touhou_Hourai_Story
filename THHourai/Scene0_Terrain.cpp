using namespace Touhou;

using namespace SC::Drawing;

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

	declare_element( Material, terrain_material );
	terrain_material->DiffuseMap = new Texture2D( "terrain_material_diffuse", "Assets/Texture/templeBase01_diffuse.jpg" );
	terrain_material->NormalMap = new Texture2D( "terrain_material_diffuse", "Assets/Texture/templeBase01_normal.jpg" );

	terrain->AddComponent<MeshRenderer>()->Material = terrain_material;
	terrain->AddComponent<MeshFilter>()->Mesh = Mesh::CreatePlane( "Scene01_Terrain_Plane", 100.0f, 100.0f );
	
	var rigidbody = terrain->AddComponent<StaticRigidbody>();

	var boxCollider = terrain->AddComponent<BoxCollider>();
	boxCollider->Center = Vector3( 0, -1, 0 );

	auto trp = terrain->Transform;
	trp->Scale = Vector3( 100, 1, 100 );
	trp->Position = Vector3( 94, 0, -94 );
	terrain->Parent = this;

#if defined( _DEBUG )
	var go = new ColliderBoxVisualizer( "debugVisualizer" );
	go->Parent = terrain;

	go->Transform->Scale = Vector3( 1, 1, 1 );
	go->Transform->Position = Vector3( 0, -1, 0 );
#endif

	// 돌
	var rock01 = new Rock1( "rock01" );
	rock01->Transform->Position = Vector3( -5.0, 0, 1 );
	rock01->Parent = this;

	// 테스트
	RefPtr<GameObject> test01 = new ColliderBoxVisualizer( "test01" );
	test01->AddComponent<Rigidbody>();
	test01->AddComponent<BoxCollider>();
	test01->Transform->Position = Vector3( 3, 1, 3 );
	test01->Parent = this;

	test01 = test01->Clone().As<GameObject>();
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
}