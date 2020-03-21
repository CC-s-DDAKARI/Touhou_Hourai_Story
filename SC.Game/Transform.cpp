using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace physx;

void Transform::SetGraphicsRootConstantBufferView( RefPtr<CDeviceContext>& deviceContext, int frameIndex, int fixedFrameIndex )
{
	if ( hasBuffer )
	{
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
	XMMATRIX world;

	if ( updated )
	{
		updated = false;
	}

	bool flag = hasUpdate;
	if ( ( gameObject && gameObject->parent && gameObject->parent->transform->updated ) || gameObject->pxRigidbody )
	{
		flag = true;
	}

	if ( flag )
	{
		XMMATRIX parent = XMMatrixIdentity();
		XMMATRIX parentInv = XMMatrixIdentity();

		XMVECTOR pos;
		XMVECTOR scale;
		XMVECTOR quat;

		// ����� ���� ��ü�� �θ� ���� ��� �θ��� ��ȯ ����� �����ɴϴ�.
		if ( auto p = gameObject->Parent; p )
		{
			parent = XMLoadFloat4x4( &p->transform->world );
			auto det = XMMatrixDeterminant( parent );
			parentInv = XMMatrixInverse( &det, parent );
		}

		// ������ٵ� ������ ��� ���� ����� �Ϸ�� ������ٵ𿡼� ��ġ�� ȸ�� ���͸� �����ɴϴ�.
		if ( gameObject->pxRigidbody )
		{
			auto globalPose = gameObject->pxRigidbody->getGlobalPose();

			pos = ToXMVec( globalPose.p );
			quat = ToXMVec( globalPose.q );

			// �۷ι� ���Ϳ��� ���� ���ͷ� ��ȯ�մϴ�.
			XMVECTOR depos, descale, dequat;
			XMMatrixDecompose( &descale, &dequat, &depos, parentInv );

			pos += depos;
			quat = XMQuaternionMultiply( quat, dequat );

			// ������ٵ��� ������ Ʈ������ ������ �����ɴϴ�.
			this->position = ToVector( pos );
			this->rotation = ToQuat( quat );
		}
		// �׷��� ���� ��� Ʈ������ ��ü���� ��ġ�� ȸ�� ���͸� �����ɴϴ�.
		else
		{
			pos = ToXMVec( position );
			quat = ToXMVec( rotation );
		}
		scale = ToXMVec( this->scale );

		// ��ü�� ���� ����� ����մϴ�.
		world = XMMatrixTransformation(
			XMVectorZero(),
			XMQuaternionIdentity(),
			scale,
			XMQuaternionIdentity(),
			quat,
			pos
		);

		world = XMMatrixMultiply( world, parent );

		// ���� �����մϴ�.
		XMStoreFloat4x4( &this->world, world );

		hasUpdate = false;
		updated = true;
	}
	else
	{
		world = XMLoadFloat4x4( &this->world );
	}

	if ( hasBuffer )
	{
		// �븻 ����� ���� ����ġ����� ����մϴ�.
		auto det = XMMatrixDeterminant( world );

		hasUpdate = false;

		// ���� ����� ���� ����ġ ����� ����մϴ�.
		auto worldInvTrp = XMMatrixTranspose( XMMatrixInverse( &det, world ) );

		// ���� GPU�� ����մϴ�.
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

		// ����� ���� ��ü�� �θ� ���� ��� �θ��� ��ȯ ����� �����ɴϴ�.
		if ( auto p = gameObject->Parent; p )
		{
			parent = XMLoadFloat4x4( &p->transform->world );
		}

		// ��ġ�� �۷ι� ��ǥ�� ��ȯ�մϴ�.
		pos = ToXMVec( value );
		pos = XMVector3Transform( pos, parent );

		// ������ٵ��� ��ġ�� �����մϴ�.
		auto gp = gameObject->pxRigidbody->getGlobalPose();
		gp.p = ToPhysX3( pos );
		gameObject->pxRigidbody->setGlobalPose( gp );
	}

	hasUpdate = true;
}

Vector3 Transform::Scale_get()
{
	return scale;
}

void Transform::Scale_set( Vector3 value )
{
	scale = value;

	hasUpdate = true;
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

		// ����� ���� ��ü�� �θ� ���� ��� �θ��� ��ȯ ����� �����ɴϴ�.
		if ( auto p = gameObject->Parent; p )
		{
			parent = XMLoadFloat4x4( &p->transform->world );
		}

		// ��ġ�� �۷ι� ��ǥ�� ��ȯ�մϴ�.
		XMVECTOR scale, quat, pos;
		XMMatrixDecompose( &scale, &quat, &pos, parent );
		quat = XMQuaternionMultiply( ToXMVec( value ), quat );

		// ������ٵ��� ��ġ�� �����մϴ�.
		auto gp = gameObject->pxRigidbody->getGlobalPose();
		gp.q = ToPhysX4( quat );
		gameObject->pxRigidbody->setGlobalPose( gp );
	}

	hasUpdate = true;
}

Vector3 Transform::Forward_get()
{
	return ( rotation * Quaternion( Vector3::Forward, 0 ) * rotation.Conjugate ).V;
}