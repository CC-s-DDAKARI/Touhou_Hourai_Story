#pragma once

namespace SC::Game
{
	/// <summary> ���� �������� ��� ���� �׸� ���� �⺻ Ŭ������ ��Ÿ���ϴ�. </summary>
	class GameObject : public Assets, virtual public IEnumerable<RefPtr<GameObject>>, virtual public ICloneable, virtual public IDisposable
	{
		friend class Scene;
		friend class Transform;
		friend class Collider;

		Scene* pScene = nullptr;

		using ComponentPair = std::pair<std::size_t, RefPtr<Component>>;
		std::vector<ComponentPair> components;
		RefPtr<Transform> transform;

		std::vector<RefPtr<GameObject>> gameObjects;
		WeakPtr parent;

		sc_game_export_object( physx::PxRigidActor* ) pxRigidbody = nullptr;
		bool isStaticRigid = false;

		void AttachCollider( Collider* pCol );

	protected:
		/// <summary> ��ü���� ������Ʈ�� �߰��Ƿ��� �մϴ�. </summary>
		/// <param name="typeId"> �߰��� ������Ʈ�� Ÿ�� ID(typeof(T))�� ���޵˴ϴ�. </param>
		/// <param name="pComponent"> �߰��� ������Ʈ ��ü�� ���޵˴ϴ�. </param>
		/// <returns> true�� ��ȯ�� ��� ������Ʈ�� �߰��Ǹ�, false�� ��ȯ�� ��� ������Ʈ�� �߰����� �ʽ��ϴ�. </returns>
		virtual bool OnAddComponent( size_t typeId, Component* pComponent );

		/// <summary> ��ü���� ������Ʈ�� ���ŵǷ��� �մϴ�. </summary>
		/// <param name="typeId"> ���ŵ� ������Ʈ�� Ÿ�� ID(typeof(T))�� ���޵˴ϴ�. </param>
		/// <param name="pComponent"> ���ŵ� ������Ʈ ��ü�� ���޵˴ϴ�. </param>
		/// <returns> true�� ��ȯ�� ��� ������Ʈ�� ���ŵǸ�, false�� ��ȯ�� ��� ������Ʈ�� ���ŵ��� �ʽ��ϴ�. </returns>
		virtual bool OnRemoveComponent( size_t typeId, Component* pComponent );

		/// <summary> ��ü�� ��鿡 ������ �� ȣ��˴ϴ�. </summary>
		/// <param name="pScene"> ��� ��� ��ü�� ���޵˴ϴ�. </param>
		virtual void OnSceneAttached( Scene* pScene );

		/// <summary> ��ü�� ��鿡�� ���ŵ� �� ȣ��˴ϴ�. </summary>
		/// <param name="pScene"> ��� ��� ��ü�� ���޵˴ϴ�. </param>
		virtual void OnSceneDetached( Scene* pScene );

	public:
		/// <summary> <see cref="GameObject"/> Ŭ������ �� �ν��Ͻ��� �ʱ�ȭ�մϴ�. </summary>
		/// <param name="name"> ��ü �̸��� �����մϴ�. </param>
		GameObject( String name = "SC.Game.GameObject" );
		~GameObject() override;

		/// <summary> (<see cref="IEnumerable"/> �������̽����� ���� ��.) �ڽ� ��ü ��ü�� �ݺ��ϴ� �����ڸ� ��ȯ�մϴ�. </summary>
		RefPtr<IEnumerator<RefPtr<GameObject>>> GetEnumerator() override;

		/// <summary> (<see cref="ICloneable"/>) ���� �ν��Ͻ��� ���纻�� �� ��ü�� �����մϴ�. </summary>
		object Clone() override;

		/// <summary> (<see cref="IDisposable"/>) ���� �ڿ��� �����մϴ�. </summary>
		void Dispose() override;

		/// <summary> ����� ���۵� �� ��鿡 ���Ե� ��� ���� ��ü�� ����˴ϴ�. </summary>
		virtual void Start();

		/// <summary> ���� ��ü�� ���� �� ������ �����մϴ�. </summary>
		/// <param name="time"> ���� �ð��� �����մϴ�. </param>
		/// <param name="input"> ���� �����ӿ��� �Է� ���¸� �����մϴ�. </param>
		virtual void Update( Time& time, Input& input );

		/// <summary> ���� ��ü�� ���� ���� ������ �� ������ �����մϴ�. </summary>
		/// <param name="time"> ���� �ð��� �����մϴ�. </param>
		virtual void FixedUpdate( Time& time );

		/// <summary> ���� ��ü�� ���� ���� �� ������ �����մϴ�. </summary>
		/// <param name="time"> ���� �ð��� �����մϴ�. </param>
		/// <param name="input"> ���� �����ӿ��� �Է� ���¸� �����մϴ�. </param>
		virtual void LateUpdate( Time& time, Input& input );

		/// <summary> ��ü�� Ȯ�� ������Ʈ�� �߰��մϴ�. </summary>
		template< class T, class... CtorArgs, std::enable_if_t<where<Component, T>>* = nullptr >
		RefPtr<T> AddComponent( CtorArgs&&... ctorArgs )
		{
			RefPtr value = new T( ctorArgs... );
			AddComponent( typeid( T ).hash_code(), value );
			return value;
		}

		/// <summary> ��ü�� �߰��� Ȯ�� ������Ʈ�� �����ɴϴ�. </summary>
		template< class T >
		RefPtr<T> GetComponent()
		{
			return GetComponent( typeid( T ).hash_code(), []( Component* ptr ) -> auto
				{
					T* dynamic = dynamic_cast< T* >( ptr );
					return dynamic != nullptr;
				}
			).TryAs<T>();
		}

		/// <summary> ��ü�� �߰��� Ȯ�� ������Ʈ�� �����ɴϴ�. </summary>
		template< class T >
		T* GetRawComponent()
		{
			return dynamic_cast< T* >( GetRawComponent( typeid( T ).hash_code(), []( Component* ptr ) -> auto
			{
				T* dynamic = dynamic_cast< T* >( ptr );
				return dynamic != nullptr;
			}
			) );
		}

		/// <summary> ��ü�� �߰��� Ȯ�� ������Ʈ�� �����մϴ�. </summary>
		template< class T >
		void RemoveComponent()
		{
			RemoveComponent( typeid( T ).hash_code(), []( Component* c ) -> bool
				{
					return dynamic_cast< T* >( c ) != nullptr;
				}
			);
		}

		/// <summary> �ڽ� ��ұ��� �����Ͽ� ��ü�� �߰��� Ȯ�� ������Ʈ ����� �����ɴϴ�. </summary>
		template< class T >
		std::vector<RefPtr<T>> GetComponentsInChildren()
		{
			using namespace std;

			vector<RefPtr<T>> components;
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

		/// <summary> �ڽ� ��ұ��� �����Ͽ� ��ü�� �߰��� Ȯ�� ������Ʈ ����� �����ɴϴ�. </summary>
		template< class T >
		std::vector<T*> GetRawComponentsInChildren()
		{
			using namespace std;

			vector<T*> components;
			// for each childs.
			if ( auto c = GetRawComponent<T>(); c )
			{
				components.push_back( c );
			}

			for ( int i = 0; i < NumChilds_get(); ++i )
			{
				auto vec = Childs_get( i )->GetRawComponentsInChildren<T>();
				components.insert( components.end(), vec.begin(), vec.end() );
			}

			return move( components );
		}

		/// <summary> (Visual Studio ����) ��ü�� ��ȯ ���¸� �����ɴϴ�. </summary>
		vs_property_get( RefPtr<Game::Transform>, Transform );

		/// <summary> ��ü�� ��ȯ ���¸� �����ɴϴ�. </summary>
		RefPtr<Game::Transform> Transform_get();

		/// <summary> (Visual Studio ����) ��ü�� �θ� �����ϰų� �����ɴϴ�. </summary>
		vs_property( RefPtr<GameObject>, Parent );

		/// <summary> ��ü�� �θ� �����ɴϴ�. </summary>
		RefPtr<GameObject> Parent_get();

		/// <summary> ��ü�� �θ� �����մϴ�. </summary>
		void Parent_set( RefPtr<GameObject> value );

		/// <summary> (Visual Studio ����) ��ü�� �����ϰ� �ִ� �ڽ� ��ü�� ������ �����ɴϴ�. </summary>
		vs_property_get( int, NumChilds );

		/// <summary> ��ü�� �����ϰ� �ִ� �ڽ� ��ü�� ������ �����ɴϴ�. </summary>
		int NumChilds_get();

		/// <summary> (Visual Studio ����) ��ü�� �����ϰ� �ִ� �ڽ� ��ü�� �����ɴϴ�. </summary>
		vs_property_get( RefPtr<GameObject>, Childs )[];
		
		/// <summary> ��ü�� �����ϰ� �ִ� �ڽ� ��ü�� �����ɴϴ�. </summary>
		RefPtr<GameObject> Childs_get( int param0 );

	private:
		void Render( RefPtr<Details::CDeviceContext>& deviceContext );
		void AddComponent( std::size_t type_hash, RefPtr<Component> component );
		std::size_t GetComponentIndex( std::size_t beginIndex, std::size_t type_hash, std::function<bool( Component* )> caster );
		RefPtr<Component> GetComponent( std::size_t type_hash, std::function<bool( Component* )> caster );
		Component* GetRawComponent( std::size_t type_hash, std::function<bool( Component* )> caster );
		void RemoveComponent( std::size_t type_hash, std::function<bool( Component* )> caster );
	};
}