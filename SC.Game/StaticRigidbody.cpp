using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace physx;
using namespace std;

StaticRigidbody::StaticRigidbody() : Component()
{

}

StaticRigidbody::~StaticRigidbody()
{
	if ( !AppShutdown && pxRigidbody )
	{
		pxRigidbody->release();
		pxRigidbody = nullptr;
	}
}

object StaticRigidbody::Clone()
{
	var clone = new StaticRigidbody();
	return clone;
}

void StaticRigidbody::Update( Time& time, Input& input )
{
	UpdateRigidbodyPhysics();
}

void StaticRigidbody::UpdateRigidbodyPhysics()
{
	if ( pxRigidbody )
	{
		set<Collider*> updateRigidbodies;
		list<list<Collider*>::iterator> remove_list;

		// ������Ʈ���� �Ļ��� ��� ���� ������Ʈ�� �ö��̴� ������Ʈ�� �����ɴϴ�.
		auto collect = Linked->GetRawComponentsInChildren<Collider>();
		updateRigidbodies.insert( collect.begin(), collect.end() );

		for ( auto i = appliedColliders.begin(); i != appliedColliders.end(); ++i )
		{
			// ��� ������ٵ� ������Ʈ ������ٵ� ��Ͽ� ���� ��� ���ŵ˴ϴ�.
			if ( auto it = updateRigidbodies.find( *i ); it == updateRigidbodies.end() )
			{
				remove_list.push_back( i );
			}
			// ���� ��� ������Ʈ ��Ͽ��� ���ŵ˴ϴ�.
			else
			{
				updateRigidbodies.erase( it );
			}
		}

		// ���� ����� ����Ʈ���� ���� ����� �������� �����մϴ�.
		for ( auto i = remove_list.rbegin(); i != remove_list.rend(); ++i )
		{
			auto pShape = ( *( *i ) )->pxShape;
			if ( pShape )
			{
				pxRigidbody->detachShape( *pShape );
			}
			appliedColliders.erase( *i );
		}

		// ���� ������Ʈ ����� ���� �߰��� ������ٵ��Դϴ�.
		for ( auto i : updateRigidbodies )
		{
			if ( i->pxShape )
			{
				pxRigidbody->attachShape( *i->pxShape );
				appliedColliders.push_back( i );
			}
		}
	}
}