#pragma once

namespace SC::Game
{
	/// <summary> 게임 엔진에서 모든 게임 항목에 대한 기본 클래스를 나타냅니다. </summary>
	class GameObject : public Assets, virtual public IEnumerable<RefPtr<GameObject>>, virtual public ICloneable
	{
		friend class Scene;
		friend class Transform;
		friend class Collider;

		Scene* pScene = nullptr;

		using ComponentPair = std::pair<std::size_t, RefPtr<Component>>;
		std::vector<ComponentPair> components;
		RefPtr<Transform> transform;

		std::vector<RefPtr<GameObject>> gameObjects;
		GameObject* parent = nullptr;

		sc_game_export_object( physx::PxRigidActor* ) pxRigidbody = nullptr;
		bool isStaticRigid = false;

		void AttachCollider( Collider* pCol );

	protected:
		/// <summary> 개체에서 컴포넌트가 추가되려고 합니다. </summary>
		/// <param name="typeId"> 추가할 컴포넌트의 타입 ID(typeof(T))가 전달됩니다. </param>
		/// <param name="pComponent"> 추가될 컴포넌트 개체가 전달됩니다. </param>
		/// <returns> true를 반환할 경우 컴포넌트가 추가되며, false를 반환할 경우 컴포넌트가 추가되지 않습니다. </returns>
		virtual bool OnAddComponent( size_t typeId, Component* pComponent );

		/// <summary> 개체에서 컴포넌트가 제거되려고 합니다. </summary>
		/// <param name="typeId"> 제거될 컴포넌트의 타입 ID(typeof(T))가 전달됩니다. </param>
		/// <param name="pComponent"> 제거될 컴포넌트 개체가 전달됩니다. </param>
		/// <returns> true를 반환할 경우 컴포넌트가 제거되며, false를 반환할 경우 컴포넌트가 제거되지 않습니다. </returns>
		virtual bool OnRemoveComponent( size_t typeId, Component* pComponent );

		/// <summary> 개체가 장면에 부착될 때 호출됩니다. </summary>
		/// <param name="pScene"> 대상 장면 개체가 전달됩니다. </param>
		virtual void OnSceneAttached( Scene* pScene );

		/// <summary> 개체가 장면에서 제거될 때 호출됩니다. </summary>
		/// <param name="pScene"> 대상 장면 개체가 전달됩니다. </param>
		virtual void OnSceneDetached( Scene* pScene );

	public:
		/// <summary> <see cref="GameObject"/> 클래스의 새 인스턴스를 초기화합니다. </summary>
		/// <param name="name"> 개체 이름을 전달합니다. </param>
		GameObject( String name = "SC.Game.GameObject" );
		~GameObject() override;

		/// <summary> (<see cref="IEnumerable"/> 인터페이스에서 구현 됨.) 자식 개체 전체를 반복하는 열거자를 반환합니다. </summary>
		RefPtr<IEnumerator<RefPtr<GameObject>>> GetEnumerator() override;

		/// <summary> (<see cref="ICloneable"/>) 현재 인스턴스의 복사본인 새 개체를 생성합니다. </summary>
		object Clone() override;

		/// <summary> 장면이 시작될 때 장면에 포함된 모든 게임 개체에 실행됩니다. </summary>
		virtual void Start();

		/// <summary> 게임 개체에 대한 논리 갱신을 수행합니다. </summary>
		/// <param name="time"> 게임 시간을 전달합니다. </param>
		/// <param name="input"> 현재 프레임에서 입력 상태를 전달합니다. </param>
		virtual void Update( Time& time, Input& input );

		/// <summary> 게임 개체에 대한 고정 프레임 논리 갱신을 수행합니다. </summary>
		/// <param name="time"> 게임 시간을 전달합니다. </param>
		virtual void FixedUpdate( Time& time );

		/// <summary> 게임 개체에 대한 늦은 논리 갱신을 수행합니다. </summary>
		/// <param name="time"> 게임 시간을 전달합니다. </param>
		/// <param name="input"> 현재 프레임에서 입력 상태를 전달합니다. </param>
		virtual void LateUpdate( Time& time, Input& input );

		/// <summary> 개체에 확장 컴포넌트를 추가합니다. </summary>
		template< class T, class... CtorArgs, std::enable_if_t<where<Component, T>>* = nullptr >
		T* AddComponent( CtorArgs&&... ctorArgs )
		{
			auto value = new T( ctorArgs... );
			AddComponent( typeid( T ).hash_code(), value );
			return value;
		}

		/// <summary> 개체에 추가된 확장 컴포넌트를 가져옵니다. </summary>
		template< class T >
		T* GetComponent()
		{
			return dynamic_cast< T* >( GetComponent( typeid( T ).hash_code(), []( Component* ptr ) -> auto
			{
				T* dynamic = dynamic_cast< T* >( ptr );
				return dynamic != nullptr;
			}
			) );
		}

		/// <summary> 개체에 추가된 확장 컴포넌트를 제거합니다. </summary>
		template< class T >
		void RemoveComponent()
		{
			RemoveComponent( typeid( T ).hash_code(), []( Component* c ) -> bool
				{
					return dynamic_cast< T* >( c ) != nullptr;
				}
			);
		}

		/// <summary> 자식 요소까지 포함하여 개체에 추가된 확장 컴포넌트 목록을 가져옵니다. </summary>
		template< class T >
		std::vector<T*> GetComponentsInChildren()
		{
			using namespace std;

			vector<T*> components;
			// for each childs.
			if ( auto c = GetComponent<T>(); c )
			{
				components.push_back( c );
			}

			for ( int i = 0; i < NumChilds_get(); ++i )
			{
				auto vec = Childs_get( i )->GetComponentsInChildren<T>();
				components.insert( components.end(), vec.begin(), vec.end() );
			}

			return move( components );
		}

		/// <summary> (Visual Studio 전용) 개체의 변환 상태를 가져옵니다. </summary>
		vs_property_get( Game::Transform*, Transform );

		/// <summary> 개체의 변환 상태를 가져옵니다. </summary>
		Game::Transform* Transform_get();

		/// <summary> (Visual Studio 전용) 개체의 부모를 설정하거나 가져옵니다. </summary>
		vs_property( GameObject*, Parent );

		/// <summary> 개체의 부모를 가져옵니다. </summary>
		GameObject* Parent_get();

		/// <summary> 개체의 부모를 설정합니다. </summary>
		void Parent_set( GameObject* value );

		/// <summary> (Visual Studio 전용) 개체가 포함하고 있는 자식 개체의 개수를 가져옵니다. </summary>
		vs_property_get( int, NumChilds );

		/// <summary> 개체가 포함하고 있는 자식 개체의 개수를 가져옵니다. </summary>
		int NumChilds_get();

		/// <summary> (Visual Studio 전용) 개체가 포함하고 있는 자식 개체를 가져옵니다. </summary>
		vs_property_get( GameObject*, Childs )[];
		
		/// <summary> 개체가 포함하고 있는 자식 개체를 가져옵니다. </summary>
		GameObject* Childs_get( int param0 );

	private:
		void Render( RefPtr<Details::CDeviceContext>& deviceContext );
		void AddComponent( std::size_t type_hash, Component* component );
		std::size_t GetComponentIndex( std::size_t beginIndex, std::size_t type_hash, std::function<bool( Component* )> caster );
		Component* GetComponent( std::size_t type_hash, std::function<bool( Component* )> caster );
		void RemoveComponent( std::size_t type_hash, std::function<bool( Component* )> caster );

		void RigidSwap( void* pxRigid );
	};
}