using namespace SC;
using namespace SC::Game::UI;
using namespace SC::Game::Details;

Brush::Brush( int type ) : Object()
{
	brushConstants = Graphics::mDevice->CreateDynamicBuffer( sizeof( Constants ) );

	frameResource = ( Constants* )brushConstants->pBlock;
	frameResource->Type = type;
	Opacity = 1.0;
}

Brush::~Brush()
{
	GC::Add( App::mFrameIndex, brushConstants.Get(), 2 );
}

double Brush::Opacity_get()
{
	return ( double )frameResource->Opacity;
}

void Brush::Opacity_set( double value )
{
	frameResource->Opacity = ( float )value;
}