using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace physx;

void Transform::SetGraphicsRootConstantBufferView( RefPtr<CDeviceContext>& deviceContext )
{
	if ( hasBuffer )
	{
		int frameIndex = GlobalVar.frameIndex;

		deviceContext->pCommandList->SetGraphicsRootConstantBufferView( Slot_Rendering_World, dynamicBuffer[frameIndex]->VirtualAddress );
	}
}

void Transform::CreateBuffer()
{
	if ( !hasBuffer )
	{
		dynamicBuffer[0] = GlobalVar.device->CreateDynamicBuffer( sizeof( Constants ), 256 );
		dynamicBuffer[1] = GlobalVar.device->CreateDynamicBuffer( sizeof( Constants ), 256 );

		hasBuffer = true;
	}
}

Transform::Transform()
{
	scale = Vector3::One;
}

Transform::~Transform()
{
	if ( hasBuffer )
	{
		GC.Add( dynamicBuffer );
	}
}

object Transform::Clone()
{
	RefPtr trans = new Transform();
	trans->position = position;
	trans->scale = scale;
	trans->rotation = rotation;
	return trans;
}

void Transform::Update( Time& time, Input& input )
{
	XMMATRIX parent = XMMatrixIdentity();
	XMMATRIX parentInv = XMMatrixIdentity();

	XMVECTOR pos;
	XMVECTOR scale;
	XMVECTOR quat;

	// 연결된 게임 개체의 부모가 있을 경우 부모의 변환 행렬을 가져옵니다.
	if ( auto p = gameObject->Parent.Get(); p )
	{
		parent = XMLoadFloat4x4( &p->transform->world );
		auto det = XMMatrixDeterminant( parent );
		parentInv = XMMatrixInverse( &det, parent );
	}

	// 리지드바디가 존재할 경우 물리 계산이 완료된 리지드바디에서 위치와 회전 벡터를 가져옵니다.
	if ( gameObject->pxRigidbody )
	{
		auto globalPose = gameObject->pxRigidbody->getGlobalPose();

		pos = ToXMVec( globalPose.p );
		quat = ToXMVec( globalPose.q );

		// 글로벌 벡터에서 로컬 벡터로 변환합니다.
		XMVECTOR depos, descale, dequat;
		XMMatrixDecompose( &descale, &dequat, &depos, parentInv );

		pos += depos;
		quat = XMQuaternionMultiply( quat, dequat );

		// 리지드바디의 정보를 트랜스폼 정보로 가져옵니다.
		this->position = ToVector( pos );
		this->rotation = ToQuat( quat );
	}
	// 그렇지 않을 경우 트랜스폼 개체에서 위치와 회전 벡터를 가져옵니다.
	else
	{
		pos = ToXMVec( position );
		quat = ToXMVec( rotation );
	}
	scale = ToXMVec( this->scale );

	// 개체의 월드 행렬을 계산합니다.
	auto world = XMMatrixTransformation(
		XMVectorZero(),
		XMQuaternionIdentity(),
		scale,
		XMQuaternionIdentity(),
		quat,
		pos
	);

	world = XMMatrixMultiply( world, parent );

	// 값을 저장합니다.
	XMStoreFloat4x4( &this->world, world );

	if ( hasBuffer )
	{
		// 노말 계산을 위한 역전치행렬을 계산합니다.
		auto det = XMMatrixDeterminant( world );

		// 법선 계산을 위해 역전치 행렬을 계산합니다.
		auto worldInvTrp = XMMatrixTranspose( XMMatrixInverse( &det, world ) );

		// 값을 GPU에 출력합니다.
		int frameIndex = GlobalVar.frameIndex;
		auto& frameResource = *( Constants* )dynamicBuffer[frameIndex]->pBlock;
		XMStoreFloat4x4( &frameResource.World, world );
		XMStoreFloat4x4( &frameResource.WorldInvTranspose, worldInvTrp );
	}
}

void Transform::LookAt( RefPtr<Transform> target, Vector3 up )
{
	LookAt( target->Position, up );
}

void Transform::LookAt( Vector3 target, Vector3 up )
{
	auto dir = target - Position;
	Rotation = Quaternion::LookTo( dir, up );
}

void Transform::LookTo( Vector3 direction, Vector3 up )
{
	Rotation = Quaternion::LookTo( direction, up );
}

Vector3 Transform::Position_get()
{
	return position;
}

void Transform::Position_set( Vector3 value )
{
	position = value;

	if ( gameObject->pxRigidbody )
	{
		XMMATRIX parent = XMMatrixIdentity();
		XMVECTOR pos;

		// 연결된 게임 개체의 부모가 있을 경우 부모의 변환 행렬을 가져옵니다.
		if ( auto p = gameObject->Parent.Get(); p )
		{
			parent = XMLoadFloat4x4( &p->transform->world );
		}

		// 위치를 글로벌 좌표로 변환합니다.
		pos = ToXMVec( value );
		pos = XMVector3Transform( pos, parent );

		// 리지드바디의 위치를 설정합니다.
		auto gp = gameObject->pxRigidbody->getGlobalPose();
		gp.p = ToPhysX3( pos );
		gameObject->pxRigidbody->setGlobalPose( gp );
	}
}

Vector3 Transform::Scale_get()
{
	return scale;
}

void Transform::Scale_set( Vector3 value )
{
	scale = value;
}

Quaternion Transform::Rotation_get()
{
	return rotation;
}

void Transform::Rotation_set( Quaternion value )
{
	rotation = value;

	if ( gameObject->pxRigidbody )
	{
		XMMATRIX parent = XMMatrixIdentity();

		// 연결된 게임 개체의 부모가 있을 경우 부모의 변환 행렬을 가져옵니다.
		if ( auto p = gameObject->Parent.Get(); p )
		{
			parent = XMLoadFloat4x4( &p->transform->world );
		}

		// 위치를 글로벌 좌표로 변환합니다.
		XMVECTOR scale, quat, pos;
		XMMatrixDecompose( &scale, &quat, &pos, parent );
		quat = XMQuaternionMultiply( ToXMVec( value ), quat );

		// 리지드바디의 위치를 설정합니다.
		auto gp = gameObject->pxRigidbody->getGlobalPose();
		gp.q = ToPhysX4( quat );
		gameObject->pxRigidbody->setGlobalPose( gp );
	}
}

Vector3 Transform::Forward_get()
{
	return ( rotation * Quaternion( Vector3::Forward, 0 ) * rotation.Conjugate ).V;
}