using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;
using namespace SC::Drawing;

using namespace std;

void Camera::SetGraphicsRootConstantBufferView( RefPtr<CDeviceContext>& deviceContext, int frameIndex )
{
	deviceContext->pCommandList->SetGraphicsRootConstantBufferView( Slot_Rendering_Camera, dynamicBuffer[frameIndex]->VirtualAddress );
}

Camera::Camera()
{
	dynamicBuffer[0] = GlobalVar.device->CreateDynamicBuffer( sizeof( Constants ), 256 );
	dynamicBuffer[1] = GlobalVar.device->CreateDynamicBuffer( sizeof( Constants ), 256 );
}

Camera::~Camera()
{
	GC.Add( dynamicBuffer );
}

object Camera::Clone()
{
	RefPtr ptr = new Camera();
	ptr->clearMode = clearMode;
	ptr->aspectRatio = aspectRatio;
	return ptr;
}

void Camera::LateUpdate( Time& time, Input& input )
{
	int frameIndex = GlobalVar.frameIndex;
	auto& frameResource = *( Constants* )dynamicBuffer[frameIndex]->pBlock;

	XMMATRIX world = XMLoadFloat4x4( &Transform->world );
	auto det = XMMatrixDeterminant( world );
	auto worldInv = XMMatrixInverse( &det, world );

	double asp = aspectRatio;
	if ( asp == 0.0 )
	{
		RECT rc;
		GetClientRect( GlobalVar.hWnd, &rc );
		auto wid = rc.right - rc.left;
		auto hei = rc.bottom - rc.top;
		asp = ( double )wid / hei;
	}

	auto view = worldInv;
	auto proj = XMMatrixPerspectiveFovLH( 0.25f * 3.14f, ( float )asp, 0.1f, 1000.0f );
	auto detProj = XMMatrixDeterminant( proj );

	auto vp = view * proj;

	XMStoreFloat4x4( &frameResource.ViewProj, vp );
	XMStoreFloat4x4( &frameResource.ViewInv, world );
	XMStoreFloat4x4( &frameResource.ProjInv, XMMatrixInverse( &detProj, proj ) );
	
	XMVECTOR scale, rotation, trans;
	XMMatrixDecompose( &scale, &rotation, &trans, world );

	XMStoreFloat3( &frameResource.Pos, trans );

	// 절두체를 계산합니다.
	XMFLOAT4X4 projection;
	XMStoreFloat4x4( &projection, proj );

	float zMin = -projection._43 / projection._33;
	float r = 1000.0f / ( 1000.0f - zMin );

	projection._33 = r;
	projection._43 = -r * zMin;
	proj = XMLoadFloat4x4( &projection );

	XMFLOAT4X4 frustumMatrix;
	XMStoreFloat4x4( &frustumMatrix, XMMatrixMultiply( view, proj ) );

	mPlanes[0].x = frustumMatrix._14 + frustumMatrix._13;
	mPlanes[0].y = frustumMatrix._24 + frustumMatrix._23;
	mPlanes[0].z = frustumMatrix._34 + frustumMatrix._33;
	mPlanes[0].w = frustumMatrix._44 + frustumMatrix._43;

	mPlanes[1].x = frustumMatrix._14 - frustumMatrix._13;
	mPlanes[1].y = frustumMatrix._24 - frustumMatrix._23;
	mPlanes[1].z = frustumMatrix._34 - frustumMatrix._33;
	mPlanes[1].w = frustumMatrix._44 - frustumMatrix._43;

	mPlanes[2].x = frustumMatrix._14 + frustumMatrix._11;
	mPlanes[2].y = frustumMatrix._24 + frustumMatrix._21;
	mPlanes[2].z = frustumMatrix._34 + frustumMatrix._31;
	mPlanes[2].w = frustumMatrix._44 + frustumMatrix._41;

	mPlanes[3].x = frustumMatrix._14 - frustumMatrix._11;
	mPlanes[3].y = frustumMatrix._24 - frustumMatrix._21;
	mPlanes[3].z = frustumMatrix._34 - frustumMatrix._31;
	mPlanes[3].w = frustumMatrix._44 - frustumMatrix._41;

	mPlanes[4].x = frustumMatrix._14 - frustumMatrix._12;
	mPlanes[4].y = frustumMatrix._24 - frustumMatrix._22;
	mPlanes[4].z = frustumMatrix._34 - frustumMatrix._32;
	mPlanes[4].w = frustumMatrix._44 - frustumMatrix._42;

	mPlanes[5].x = frustumMatrix._14 + frustumMatrix._12;
	mPlanes[5].y = frustumMatrix._24 + frustumMatrix._22;
	mPlanes[5].z = frustumMatrix._34 + frustumMatrix._32;
	mPlanes[5].w = frustumMatrix._44 + frustumMatrix._42;
}

Ray Camera::ScreenSpaceToRay( Point<double> screenPoint )
{
	int frameIndex = GlobalVar.frameIndex;

	XMMATRIX world = XMLoadFloat4x4( &Transform->world );
	auto det = XMMatrixDeterminant( world );
	auto worldInv = XMMatrixInverse( &det, world );

	double asp = aspectRatio;
	if ( asp == 0.0 )
	{
		RECT rc;
		GetClientRect( GlobalVar.hWnd, &rc );
		auto wid = rc.right - rc.left;
		auto hei = rc.bottom - rc.top;
		asp = ( double )wid / hei;
	}

	auto view_ = worldInv;
	auto proj_ = XMMatrixPerspectiveFovLH( 0.25f * 3.14f, ( float )asp, 0.1f, 1000.0f );

	XMFLOAT4X4 viewInv;
	XMFLOAT4X4 proj;
	XMStoreFloat4x4( &proj, proj_ );
	det = XMMatrixDeterminant( view_ );
	auto inv = XMMatrixInverse( &det, view_ );
	XMStoreFloat4x4( &viewInv, inv );

	Vector3 viewSpace;
	viewSpace.X = ( screenPoint.X - proj._31 ) / proj._11;
	viewSpace.Y = ( screenPoint.Y - proj._32 ) / proj._22;
	viewSpace.Z = 1.0;

	Vector3 dir;
	Vector3 org;

	dir.X = viewSpace.X * viewInv._11 + viewSpace.Y * viewInv._21 + viewSpace.Z * viewInv._31;
	dir.Y = viewSpace.X * viewInv._12 + viewSpace.Y * viewInv._22 + viewSpace.Z * viewInv._32;
	dir.Z = viewSpace.X * viewInv._13 + viewSpace.Y * viewInv._23 + viewSpace.Z * viewInv._33;

	org.X = viewInv._41;
	org.Y = viewInv._42;
	org.Z = viewInv._43;

	Ray ray;
	ray.Origin = org;
	ray.Direction = dir.Normalized;
	ray.MinDepth = 0.1;
	ray.MaxDepth = 1000.0;

	return ray;
}

bool Camera::IntersectPoint( Vector3 point )
{
	auto pos = XMVectorSet( ( float )point.X, ( float )point.Y, ( float )point.Z, 1.0f );

	for ( int i = 0; i < 6; ++i )
	{
		auto plane = XMLoadFloat4( &mPlanes[i] );
		if ( XMVectorGetX( XMPlaneDotCoord( plane, pos ) ) < 0.0f )
			return false;
	}

	return true;
}

bool Camera::IntersectSphere( Sphere sphere )
{
	auto pos = XMVectorSet( ( float )sphere.Center.X, ( float )sphere.Center.Y, ( float )sphere.Center.Z, 1.0f );

	for ( int i = 0; i < 6; ++i )
	{
		auto plane = XMLoadFloat4( &mPlanes[i] );
		if ( XMVectorGetX( XMPlaneDotCoord( plane, pos ) ) < -( float )sphere.Radius )
			return false;
	}

	return true;
}

RefPtr<CameraClearMode> Camera::ClearMode_get()
{
	return clearMode;
}

void Camera::ClearMode_set( RefPtr<CameraClearMode> value )
{
	clearMode = value;
}

double Camera::AspectRatio_get()
{
	return aspectRatio;
}

void Camera::AspectRatio_set( double value )
{
	aspectRatio = value;
}