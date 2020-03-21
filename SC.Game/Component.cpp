using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

Component::Component()
{

}

void Component::Render( RefPtr<CDeviceContext>& deviceContext, int frameIndex )
{

}

void Component::Awake()
{

}

void Component::Start()
{

}

void Component::Update( Time& time, Input& input )
{

}

void Component::FixedUpdate( Time& time )
{

}

void Component::LateUpdate( Time& time, Input& input )
{

}

GameObject* Component::Linked_get()
{
	return gameObject;
}

RefPtr<Transform> Component::Transform_get()
{
	return Linked->Transform;
}

bool Component::IsEnabled_get()
{
	return isEnabled;
}

void Component::IsEnabled_set( bool value )
{
	isEnabled = value;
}