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

		// 오브젝트에서 파생된 모든 게임 오브젝트의 컬라이더 컴포넌트를 가져옵니다.
		auto collect = Linked->GetRawComponentsInChildren<Collider>();
		updateRigidbodies.insert( collect.begin(), collect.end() );

		for ( auto i = appliedColliders.begin(); i != appliedColliders.end(); ++i )
		{
			// 장면 리지드바디가 업데이트 리지드바디 목록에 없을 경우 제거됩니다.
			if ( auto it = updateRigidbodies.find( *i ); it == updateRigidbodies.end() )
			{
				remove_list.push_back( i );
			}
			// 있을 경우 업데이트 목록에서 제거됩니다.
			else
			{
				updateRigidbodies.erase( it );
			}
		}

		// 제거 목록의 리스트에서 적용 목록의 아이템을 제거합니다.
		for ( auto i = remove_list.rbegin(); i != remove_list.rend(); ++i )
		{
			auto pShape = ( *( *i ) )->pxShape;
			if ( pShape )
			{
				pxRigidbody->detachShape( *pShape );
			}
			appliedColliders.erase( *i );
		}

		// 남은 업데이트 목록은 새로 추가된 리지드바디입니다.
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