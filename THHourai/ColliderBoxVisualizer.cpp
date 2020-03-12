using namespace Touhou;

ColliderBoxVisualizer::ColliderBoxVisualizer( String name ) : GameObject( name )
{
	AddComponent<MeshFilter>()->Mesh = Mesh::CreateCube( "colliderDebug" );
	AddComponent<MeshRenderer>();
}