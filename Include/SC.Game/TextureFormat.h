#pragma once

namespace SC::Game
{
	/// <summary> 텍스처 형식을 표현합니다. </summary>
	enum class TextureFormat
	{
		/// <summary> 픽셀당 32비트를 사용하는 PRGBA 형식입니다. </summary>
		PRGBA_32bpp,

		/// <summary> 픽셀당 8비트를 사용하는 Gray 형식입니다. </summary>
		Gray_8bpp,
	};
}