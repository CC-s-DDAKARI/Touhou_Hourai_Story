using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace physx;

PhysicsMaterial::PhysicsMaterial( String name ) : Assets( name )
{
	pxMaterial = Physics::mPhysics->createMaterial( 0.5f, 0.5f, 0.6f );
}

PhysicsMaterial::~PhysicsMaterial()
{
	if ( pxMaterial )
	{
		pxMaterial->release();
		pxMaterial = nullptr;
	}
}

double PhysicsMaterial::DynamicFriction_get()
{
	return ( double )pxMaterial->getDynamicFriction();
}

void PhysicsMaterial::DynamicFriction_set( double value )
{
	pxMaterial->setDynamicFriction( ( PxReal )value );
}

double PhysicsMaterial::StaticFriction_get()
{
	return ( double )pxMaterial->getStaticFriction();
}

void PhysicsMaterial::StaticFriction_set( double value )
{
	pxMaterial->setStaticFriction( ( PxReal )value );
}