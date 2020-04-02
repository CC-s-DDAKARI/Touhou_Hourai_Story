using namespace SC;
using namespace SC::Game;
using namespace SC::Game::Details;

Light::Light()
{
	mConstantBuffer = HeapAllocator::Alloc( sizeof( LightConstants ) );

	LightColor = Drawing::Color::White;
	Diffuse = 1.0;
}

void Light::SetGraphicsRootConstantBufferView( RefPtr<CDeviceContext>& deviceContext, int frameIndex )
{
	auto& pCommandList = *deviceContext->pCommandList.Get();
	pCommandList.SetGraphicsRootConstantBufferView( Slot_Rendering_Light, mConstantBuffer->GetGPUVirtualAddress() );
}

void Light::SetGraphicsRootShaderResources( RefPtr<CDeviceContext>& deviceContext )
{
	if ( shadowCast )
	{
		if ( auto slot = deviceContext->Slot["ShadowDepth"]; slot != -1 )
		{
			deviceContext->SetGraphicsRootShaderResources( slot, pShaderResourceView );
		}
	}
}

void Light::SetDepthBuffer( void* pResource )
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srv{ };
	srv.Format = DXGI_FORMAT_R32_FLOAT;
	srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv.Texture2D.MipLevels = 1;

	pShaderResourceView = Graphics::mDevice->CreateShaderResourceView( ( ID3D12Resource* )pResource, &srv );
}

Light::~Light()
{
	GC::Add( App::mFrameIndex, mConstantBuffer.Get(), 2 );
	GC::Add( App::mFrameIndex, pShaderResourceView.Get(), 2 );
}

void Light::Update( Time& time, Input& input )
{
	if ( mUpdate )
	{
		auto block = mConstantBuffer->Map();
		memcpy( block, &mFrameResource, sizeof( mFrameResource ) );
		mConstantBuffer->Unmap();

		mUpdate = false;
	}
}

Drawing::Color Light::LightColor_get()
{
	return lightColor;
}

void Light::LightColor_set( Drawing::Color value )
{
	lightColor = value;
	mFrameResource.Color = { lightColor.R, lightColor.G, lightColor.B };
	mUpdate = true;
}

double Light::Ambient_get()
{
	return ( double )mFrameResource.Ambient;
}

void Light::Ambient_set( double value )
{
	mFrameResource.Ambient = ( float )value;
	mUpdate = true;
}

double Light::Diffuse_get()
{
	return ( double )mFrameResource.Diffuse;
}

void Light::Diffuse_set( double value )
{
	mFrameResource.Diffuse = ( float )value;
	mUpdate = true;
}

double Light::Specular_get()
{
	return ( double )mFrameResource.Specular;
}

void Light::Specular_set( double value )
{
	mFrameResource.Specular = ( float )value;
	mUpdate = true;
}

bool Light::IsShadowCast_get()
{
	return shadowCast;
}

void Light::IsShadowCast_set( bool value )
{
	shadowCast = value;
	mFrameResource.ShadowCast = ( int )value;
	ReadyBuffer();
}