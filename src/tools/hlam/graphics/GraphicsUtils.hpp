#pragma once

#include <algorithm>
#include <string_view>

#include <glm/vec3.hpp>

#include "core/shared/Const.hpp"

#include "OpenGL.hpp"

namespace graphics
{
/**
*	Converts an 8 bit image to a 24 bit RGB image.
*/
void Convert8to24Bit( const int iWidth, const int iHeight, const byte* const pData, const byte* const pPalette, byte* const pOutData );

/**
*	Flips an image vertically. This allows conversion between OpenGL and image formats. The image is flipped in place.
*	@param iWidth Image width, in pixels.
*	@param iHeight Image height, in pixels.
*	@param pData Pixel data, in RGB 24 bit.
*/
void FlipImageVertically( const int iWidth, const int iHeight, byte* const pData );

/**
*	Draws a background texture, fitted to the viewport.
*	@param backgroundTexture OpenGL texture id that represents the background texture
*/
void DrawBackground( GLuint backgroundTexture );

/**
*	Sets the projection matrix to the default perspective settings.
*	@param flFOV Field Of View.
*	@param iWidth Width of the viewport
*	@param iHeight Height of the viewport
*/
void SetProjection( const float flFOV, const int iWidth, const int iHeight );

/**
*	Draws a box using an array of 8 vectors as corner points.
*/
void DrawBox( const glm::vec3* const v );

/**
*	@brief Tests if the given filename is a remap name, and returns the remap ranges if so
*/
bool TryGetRemapColors(std::string_view fileName, int& low, int& mid, int& high);

void PaletteHueReplace(byte* palette, int newHue, int start, int end);
}
