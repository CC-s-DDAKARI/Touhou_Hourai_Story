using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace physx;
using namespace std;

TerrainCollider::TerrainCollider() : Collider()
{

}

TerrainCollider::~TerrainCollider()
{

}

object TerrainCollider::Clone()
{
	var clone = new TerrainCollider();
	Clone( clone.Get() );
	return clone;
}

void TerrainCollider::Update( Time& time, Input& input )
{
	if ( auto terrain = GetComponent<Terrain>(); mBakedTerrain != terrain )
	{
		Bake( terrain );
	}

	if ( !mPrevActualScale.Equals( Transform->ActualScale ) )
	{
		mPrevActualScale = Transform->ActualScale;

		auto heightFieldGeo = make_unique<PxHeightFieldGeometry>();
		heightFieldGeo->rowScale = ( PxReal )( mPrevActualScale.X * 2.0 ) / ( PxReal )mBakedTerrain->mHeightMap->width;
		heightFieldGeo->columnScale = ( PxReal )( mPrevActualScale.Z * 2.0 ) / ( float )mBakedTerrain->mHeightMap->height;
		heightFieldGeo->heightField = mCookedHeightField;
		heightFieldGeo->heightScale = ( PxReal )( mPrevActualScale.Y / 255 );

		mGeometry = move( heightFieldGeo );
		mHasUpdate = true;
	}

	Collider::Update( time, input );
}

void TerrainCollider::Bake( Terrain* terrain )
{
	auto heightMap = terrain->mHeightMap.Get();

	if ( heightMap )
	{
		PxHeightFieldDesc heightFieldDesc;
		heightFieldDesc.setToDefault();
		heightFieldDesc.nbRows = heightMap->width;
		heightFieldDesc.nbColumns = heightMap->height;
		heightFieldDesc.samples.data = heightMap->mHeightBuffer.data();
		heightFieldDesc.samples.stride = sizeof( HeightMap::Sample );

		PxDefaultMemoryOutputStream cookedData;
		Physics::mCooking->cookHeightField( heightFieldDesc, cookedData );
		PxDefaultMemoryInputData cookedInput( cookedData.getData(), cookedData.getSize() );
		mCookedHeightField = Physics::mPhysics->createHeightField( cookedInput );
		mBakedTerrain = terrain;
	}
}