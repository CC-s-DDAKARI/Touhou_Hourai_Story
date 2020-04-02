using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

using namespace std;

ComPtr<LargeHeap> Material::pReflectionBuffer;
ComPtr<CShaderResourceView> Material::pShaderResourceView;
int Material::capacity;
vector<bool> Material::locked;
ComPtr<CShaderResourceView> Material::pNullSRV;

void Material::Initialize()
{
	App::Disposing += Dispose;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{ };
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;

	pNullSRV = Graphics::mDevice->CreateShaderResourceView( nullptr, &srvDesc );

	// 버퍼가 없을 경우 초기 버퍼를 생성합니다.
	Realloc( 256 );
}

void Material::Dispose( object sender )
{
	// 전역 개체를 명시적으로 제거합니다.
	pReflectionBuffer = nullptr;
	pShaderResourceView = nullptr;
	capacity = 0;
	pNullSRV = nullptr;
}

void Material::SetGraphicsRootConstantBuffers( RefPtr<CDeviceContext>& deviceContext )
{
	auto pCommandList = deviceContext->pCommandList.Get();

	if ( lock_guard<mutex> lock( mLocker ); mConstantUpdated )
	{
		UpdateConstants();
		mConstantUpdated = false;
	}

	pCommandList->SetGraphicsRootConstantBufferView( Slot_Rendering_Material, mConstantBuffer->GetGPUVirtualAddress() );

	CShaderResourceView* ppSRVs[3]{ };

	if ( diffuseMap && diffuseMap->Lock( deviceContext ) )
	{
		ppSRVs[0] = diffuseMap->pShaderResourceView.Get();
	}
	else ppSRVs[0] = pNullSRV.Get();

	if ( diffuseLayerMap && diffuseLayerMap->Lock( deviceContext ) )
	{
		ppSRVs[1] = diffuseLayerMap->pShaderResourceView.Get();
	}
	else ppSRVs[1] = pNullSRV.Get();

	if ( normalMap && normalMap->Lock( deviceContext ) )
	{
		normalMap->Lock( deviceContext );
		ppSRVs[2] = normalMap->pShaderResourceView.Get();
	}
	else ppSRVs[2] = pNullSRV.Get();

	deviceContext->SetGraphicsRootShaderResources( Slot_Rendering_Textures, ARRAYSIZE( ppSRVs ), ppSRVs );
}

void Material::SetGraphicsRootShaderResources( RefPtr<CDeviceContext>& deviceContext )
{
	if ( auto slot = deviceContext->Slot["Reflection"]; slot != -1 )
	{
		deviceContext->SetGraphicsRootShaderResources( slot, pShaderResourceView );
	}
}

Material::Material( String name ) : Assets( name )
{
	lockIndex = Lock( this );

	XMStoreFloat4x4( &frameResourceConstants.TexWorld, XMMatrixIdentity() );

	mConstantBuffer = HeapAllocator::Alloc( sizeof( Reflection ) );

	frameResourceConstants = { };
	frameResourceConstants.Index = lockIndex;

	Ambient = 1;
	Diffuse = 1;
	Specular = 1;
	SpecExp = 32;
	TexLocation = 0;
	TexScale = 1.0;
}

Material::~Material()
{
	GC::Add( App::mFrameIndex, mConstantBuffer.Get(), 5 );
}

double Material::Ambient_get()
{
	return ( double )frameResourceReflection.Ambient;
}

void Material::Ambient_set( double value )
{
	frameResourceReflection.Ambient = ( float )value;
	UpdateReflection();
}

double Material::Diffuse_get()
{
	return ( double )frameResourceReflection.Diffuse;
}

void Material::Diffuse_set( double value )
{
	frameResourceReflection.Diffuse = ( float )value;
	UpdateReflection();
}

double Material::Specular_get()
{
	return ( double )frameResourceReflection.Specular;
}

void Material::Specular_set( double value )
{
	frameResourceReflection.Specular = ( float )value;
	UpdateReflection();
}

double Material::SpecExp_get()
{
	return ( double )frameResourceReflection.SpecExp;
}

void Material::SpecExp_set( double value )
{
	frameResourceReflection.SpecExp = ( float )value;
	UpdateReflection();
}

RefPtr<Texture2D> Material::DiffuseMap_get()
{
	return diffuseMap;
}

void Material::DiffuseMap_set( RefPtr<Texture2D> value )
{
	if ( ( bool )diffuseMap != ( bool )value )
	{
		frameResourceConstants.DiffuseMap += ( bool )value ? 1 : -1;
		mConstantUpdated = true;
	}
	diffuseMap = value;
}

RefPtr<Texture2D> Material::DiffuseLayerMap_get()
{
	return diffuseLayerMap;
}

void Material::DiffuseLayerMap_set( RefPtr<Texture2D> value )
{
	if ( ( bool )diffuseLayerMap != ( bool )value )
	{
		frameResourceConstants.DiffuseMap += ( bool )value ? 1 : -1;
		mConstantUpdated = true;
	}
	diffuseLayerMap = value;
}

RefPtr<Texture2D> Material::NormalMap_get()
{
	return normalMap;
}

void Material::NormalMap_set( RefPtr<Texture2D> value )
{
	normalMap = value;
	frameResourceConstants.NormalMap = ( int )( bool )value;
	mConstantUpdated = true;
}

RenderQueueLayer Material::Layer_get()
{
	return layer;
}

void Material::Layer_set( RenderQueueLayer value )
{
	layer = value;
}

Vector2 Material::TexLocation_get()
{
	return Vector2( ( double )frameResourceConstants.TexWorld._41, ( double )frameResourceConstants.TexWorld._42 );
}

void Material::TexLocation_set( Vector2 value )
{
	frameResourceConstants.TexWorld._41 = ( float )value.X;
	frameResourceConstants.TexWorld._42 = ( float )value.Y;
	mConstantUpdated = true;
}

Vector2 Material::TexScale_get()
{
	return Vector2( ( double )frameResourceConstants.TexWorld._11, ( double )frameResourceConstants.TexWorld._22 );
}

void Material::TexScale_set( Vector2 value )
{
	frameResourceConstants.TexWorld._11 = ( float )value.X;
	frameResourceConstants.TexWorld._22 = ( float )value.Y;
	mConstantUpdated = true;
}

int Material::Lock( Material* ptr )
{
	for ( int i = 0; i < capacity; ++i )
	{
		// 빈 공간을 찾았으면 그곳의 인덱스를 반환합니다.
		if ( !locked[i] )
		{
			locked[i] = true;
			return i;
		}
	}

	// 빈 공간을 찾지 못했으면 전체 크기를 늘립니다.
	// 인덱스는 늘린 공간에서 첫 번째 위치입니다.
	return Realloc( capacity + 256 );
}

int Material::Realloc( int capacity )
{
	auto pDevice = Graphics::mDevice->pDevice.Get();
	pReflectionBuffer = new LargeHeap( D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, sizeof( Reflection ) * capacity );

	// 버퍼에 대한 셰이더 자원 서술자를 생성합니다.
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{ };
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.NumElements = capacity;
	srvDesc.Buffer.StructureByteStride = sizeof( Reflection );
	pShaderResourceView = Graphics::mDevice->CreateShaderResourceView( pReflectionBuffer->pResource.Get(), &srvDesc );

	// 캐퍼시티를 다시 설정합니다.
	auto ret = Material::capacity;
	locked.resize( capacity, false );
	Material::capacity = capacity;
	return ret;
}

void Material::UpdateReflection()
{
	D3D12_RANGE range;
	
	range.Begin = sizeof( Reflection ) * lockIndex;
	range.End = range.Begin + sizeof( Reflection );

	Reflection* pBlock = ( Reflection* )pReflectionBuffer->Map();
	pBlock[lockIndex] = frameResourceReflection;
	pReflectionBuffer->Unmap( range );
}

void Material::UpdateConstants()
{
	for ( int i = 0; i < 2; ++i )
	{
		auto block = mConstantBuffer->Map( i );
		memcpy( block, &frameResourceConstants, sizeof( frameResourceConstants ) );
		mConstantBuffer->Unmap( i );
	}
}