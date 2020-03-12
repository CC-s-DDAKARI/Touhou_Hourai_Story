using namespace Touhou;

PlayableCharacterBase::PlayableCharacterBase( String name, String filename ) : GameObject( name )
	, filename( filename )
{
	IsLocked = true;

	AddComponent<MouseEventDispatcher>();
	AddComponent<Animator>();
	{
		var combat = AddComponent<CombatBase>();
		combat->MaxHealthPoint = 500;
		combat->MaxMagicPoint = 10;
		combat->CurrentHealthPoint = 500;
		combat->MaxMagicPoint = 10;
	}
	var rigid = AddComponent<Rigidbody>();
	{
	}
	var collider = AddComponent<BoxCollider>();
	{
		collider->Center = Vector3( 0, 1, 0 );
		collider->Extent = Vector3( 1, 1, 1 );
	}

#if defined( _DEBUG )
	var go = new ColliderBoxVisualizer( "debugVisualizer" );
	go->Parent = this;

	go->Transform->Scale = Vector3( 1, 1, 1 );
	go->Transform->Position = Vector3( 0, 1, 0 );
#endif

	IsLocked = false;
}

void PlayableCharacterBase::LoadAnimatorController()
{
	var controller = new AnimatorController( "AnimatorController" );

	var standClip = AssetBundle::GetItemAs<AnimationClip>( String::Format( "{0}:Stand", filename ) );
	var walkClip = AssetBundle::GetItemAs<AnimationClip>( String::Format( "{0}:Walk", filename ) );

	var standState = new AnimationState( "Stand" );
	standState->Clip = standClip;

	var walkState = new AnimationState( "Walk" );
	walkState->Clip = walkClip;

	controller->AddState( standState, true );
	controller->AddState( walkState );

	AnimationTransitionCondition to_walk( L"Walk", L"Velocity", 0.3, []( AnimVar var ) -> bool
		{
			if ( var.Type == 1 )
			{
				if ( var.Double >= 0.1 )
				{
					return true;
				}
			}
			return false;
		}
	);
	controller->AddTransition( L"Stand", to_walk );

	AnimationTransitionCondition to_stand( L"Stand", L"Velocity", 0.3, []( AnimVar var ) -> bool
		{
			if ( var.Type == 1 )
			{
				if ( var.Double <= 0.9 )
				{
					return true;
				}
			}
			return false;
		}
	);
	controller->AddTransition( L"Walk", to_stand );

	GetComponent<Animator>()->Controller = controller;
}