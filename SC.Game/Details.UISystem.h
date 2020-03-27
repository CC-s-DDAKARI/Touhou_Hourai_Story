#pragma once

namespace SC::Game::Details
{
	class UISystem abstract
	{
	public:
		static RefPtr<UI::Canvas> mRootCanvas;
		static RefPtr<Details::VisibleViewStorage> mViewStorage;
		static RefPtr<Details::CDeviceContext> mDeviceContext;

		static D3D12_VIEWPORT mViewport;
		static D3D12_RECT mScissorRect;

		static RefPtr<UI::TextFormat> mDefaultTextFormat;
		static std::set<GlyphBuffer*> mGlyphBuffers;

	public:
		static void Initialize();
		static void Update();
		static void Render( int frameIndex );

	private:
		static void ResizeApp( object sender, Drawing::Point<int> size );
		static void Dispose( object sender );
	};
}