using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace physx;

void Transform::SetGraphicsRootConstantBufferView( RefPtr<CDeviceContext>& deviceContext )
{
	int frameIndex = GlobalVar.frameIndex;

	if ( auto slot = deviceContext->Slot["World"]; slot != -1 )
	{
		deviceContext->pCommandList->SetGraphicsRootConstantBufferView( ( UINT )slot, dynamicBuffer[frameIndex]->VirtualAddress );
	}
}

Transform::Transform()
{
	scale = Vector3::One;

	dynamicBuffer[0] = GlobalVar.device->CreateDynamicBuffer( sizeof( Constants ), 256 );
	dynamicBuffer[1] = GlobalVar.device->CreateDynamicBuffer( sizeof( Constants ), 256 );
}

Transform::~Transform()
{
	GC.Add( dynamicBuffer );
}

object Transform::Clone()
{
	RefPtr trans = new Transform();
	trans->position = position;
	trans->scale = scale;
	trans->rotation = rotation;
	return trans;
}

void Transform::LateUpdate( Time& time, Input& input )
{
	var rigid = GetComponent<Rigidbody>();
	var staticRigid = GetComponent<StaticRigidbody>();

	XMVECTOR pos;
	XMVECTOR scale;
	XMVECTOR quat;

	if ( rigid )
	{
		auto gp = rigid->pxRigidbody->getGlobalPose();
		pos = XMVectorSet( gp.p.x, gp.p.y, gp.p.z, 0 );
		quat = XMVectorSet( gp.q.x, gp.q.y, gp.q.z, gp.q.w );
		pHasRigid = rigid.Get();
	}
	else
	{
		pos = XMVectorSet( ( float )position.X, ( float )position.Y, ( float )position.Z, 0 );
		quat = XMVectorSet( ( float )rotation.X, ( float )rotation.Y, ( float )rotation.Z, ( float )rotation.W );
		pHasRigid = nullptr;
	}
	scale = XMVectorSet( ( float )this->scale.X, ( float )this->scale.Y, ( float )this->scale.Z, 0 );

	int frameIndex = GlobalVar.frameIndex;
	auto& frameResource = *( Constants* )dynamicBuffer[frameIndex]->pBlock;

	XMMATRIX worldPar = XMMatrixIdentity();
	XMMATRIX worldParInv = XMMatrixIdentity();

	if ( Linked->Parent )
	{
		auto& trp = *( Constants* )Linked->Parent->Transform->dynamicBuffer[frameIndex]->pBlock;
		worldPar = XMLoadFloat4x4( &trp.World );

		if ( rigid )
		{
			auto det = XMMatrixDeterminant( worldPar );
			worldParInv = XMMatrixInverse( &det, worldPar );
		}
	}

	XMMATRIX world =
		XMMatrixScalingFromVector( scale ) *
		XMMatrixRotationQuaternion( quat ) *
		XMMatrixTranslationFromVector( pos );

	XMMATRIX worldFinal = world * worldPar;
	XMVECTOR deter = XMMatrixDeterminant( worldFinal );
	XMStoreFloat4x4( &frameResource.World, worldFinal );
	XMStoreFloat4x4( &frameResource.WorldInvTranspose, XMMatrixTranspose( XMMatrixInverse( &deter, worldFinal ) ) );

	if ( rigid )
	{
		XMVECTOR descale, dequat, depos;
		XMMatrixDecompose( &descale, &dequat, &depos, worldParInv );

		pos += depos;
		quat = XMQuaternionMultiply( quat, dequat );

		this->position = Vector3( ( double )XMVectorGetX( pos ), ( double )XMVectorGetY( pos ), ( double )XMVectorGetZ( pos ) );
		this->rotation = Quaternion( ( double )XMVectorGetX( quat ), ( double )XMVectorGetY( quat ), ( double )XMVectorGetZ( quat ), ( double )XMVectorGetW( quat ) );
	}

	if ( staticRigid )
	{
		XMVECTOR descale, dequat, depos;
		XMMatrixDecompose( &descale, &dequat, &depos, worldParInv );

		pos += depos;
		quat = XMQuaternionMultiply( quat, dequat );

		this->position = Vector3( ( double )XMVectorGetX( pos ), ( double )XMVectorGetY( pos ), ( double )XMVectorGetZ( pos ) );
		this->rotation = Quaternion( ( double )XMVectorGetX( quat ), ( double )XMVectorGetY( quat ), ( double )XMVectorGetZ( quat ), ( double )XMVectorGetW( quat ) );

		PxTransform gp;
		gp.p = PxVec3( XMVectorGetX( pos ), XMVectorGetY( pos ), XMVectorGetZ( pos ) );
		gp.q = PxQuat( XMVectorGetX( quat ), XMVectorGetY( quat ), XMVectorGetZ( quat ), XMVectorGetW( quat ) );

		if ( !staticRigid->pxRigidbody )
		{
			staticRigid->pxRigidbody = GlobalVar.pxDevice->createRigidStatic( gp );
		}
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
	if ( pHasRigid )
	{
		auto gp = pHasRigid->pxRigidbody->getGlobalPose();
		gp.p = PxVec3( ( float )value.X, ( float )value.Y, ( float )value.Z );
		pHasRigid->pxRigidbody->setGlobalPose( gp );
	}
	position = value;
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
	if ( pHasRigid )
	{
		auto gp = pHasRigid->pxRigidbody->getGlobalPose();
		gp.q = PxQuat( ( float )value.X, ( float )value.Y, ( float )value.Z, ( float )value.W );
		pHasRigid->pxRigidbody->setGlobalPose( gp );
	}
	rotation = value;
}

Vector3 Transform::Forward_get()
{
	return ( rotation * Quaternion( Vector3::Forward, 0 ) * rotation.Conjugate ).V;
}