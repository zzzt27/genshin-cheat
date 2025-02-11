#include <pch.h>
#include "ImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <cheat-base/util.h>
#include <cheat-base/render/backend/dx11-hook.h>

std::optional<ImageLoader::ImageData> ImageLoader::GetImage(const std::string& imageName, const char* type)
{
	if (s_Textures.count(imageName) > 0)
		return s_Textures[imageName];

	if (s_Handle == nullptr)
	{
		// LOG_WARNING("Handle not setted up, loading image failed: %s", imageName.c_str());
		return {};
	}

	LPBYTE pDestination;
	DWORD size;

	bool loadResult = util::LoadModuleResource(s_Handle, imageName.c_str(), type, pDestination, size);
	if (!loadResult)
	{
		// LOG_WARNING("Failed to load image: %s from resources", imageName);
		return {};
	}

	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load_from_memory(pDestination, size, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
	{
		// LOG_WARNING("Failed to convert image '%s' to RGBA by 'stb_image.h'", imageName.c_str());
		return {};
	}

	ImageLoader::ImageData imageData = {};

	ID3D11ShaderResourceView* new_texture = NULL;
	int width, height;
	bool textureResult = backend::LoadTextureFromMemory(image_data, image_width, image_height, 
		reinterpret_cast<ID3D11ShaderResourceView**>(&imageData.textureID), &width, &height);
	stbi_image_free(image_data);

	if (!textureResult)
	{
		// LOG_WARNING("Failed to load texture by DX11 for image: %s", imageName.c_str());
		return {};
	}

	imageData.size.x = width;
	imageData.size.y = height;
	s_Textures[imageName] = imageData;
	return imageData;
}

void ImageLoader::SetHandle(HMODULE handle)
{
	s_Handle = handle;
}
