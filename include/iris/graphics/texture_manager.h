#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "core/data_buffer.h"
#include "graphics/pixel_format.h"
#include "graphics/texture.h"

namespace iris
{

/**
 * Abstract class for creating and managing Texture objects. This class handles
 * caching and lifetime management of all created objects. Implementers just
 * need to provide a graphics API specific method for creating Texture objects.
 */
class TextureManager
{
  public:
    virtual ~TextureManager() = default;

    /**
     * Load a texture from the supplied file. Will use ResourceManager.
     *
     * This function uses caching, so loading the same resource more than once
     * will return the same handle.
     *
     * @param resource
     *   File to load.
     *
     * @returns
     *   Pointer to loaded texture.
     */
    Texture *load(const std::string &resource);

    /**
     * Load a texture from a DataBuffer.
     *
     * @param data
     *   Raw data of image, in pixel_format.
     *
     * @param width
     *   Width of image.
     *
     * @param height
     *   Height of image.
     *
     * @param pixel_format
     *   Format of pixel data.
     */
    Texture *load(
        const DataBuffer &data,
        std::uint32_t width,
        std::uint32_t height,
        PixelFormat pixel_format);

    /**
     * Unloaded the supplied texture (if there are no other references to it).
     *
     * Normally we would want textures to stay loaded to avoid excess loads.
     * However in some cases it may be necessary to unload a texture (if we know
     * we don't want to use it again).
     *
     * This function decrements an internal reference count and will only
     * actually unload texture memory if that reference count reaches 0.
     *
     * @param texture
     *   Texture to unload.
     */
    void unload(Texture *texture);

    /**
     * Get a blank 1x1 white texture
     *
     * @returns
     *   Blank texture.
     */
    Texture *blank();

  protected:
    /**
     * Create a Texture object with the provided data.
     *
     * @param data
     *   Raw data of image, in pixel_format.
     *
     * @param width
     *   Width of image.
     *
     * @param height
     *   Height of image.
     *
     * @param pixel_format
     *   Format of pixel data.
     */
    virtual std::unique_ptr<Texture> create(
        const DataBuffer &data,
        std::uint32_t width,
        std::uint32_t height,
        PixelFormat pixel_format) = 0;

    /**
     * Implementors should override this method to provide implementation
     * specific unloading logic. Called automatically when a Texture is being
     * unloaded (after its reference count is zero), default is a no-op.
     *
     * @param texture
     *   Texture about to be unloaded.
     */
    virtual void destroy(Texture *texture);

  private:
    /**
     * Support struct to store a loaded Texture and a reference count.
     */
    struct LoadedTexture
    {
        std::size_t ref_count;
        std::unique_ptr<Texture> texture;
    };

    /** Collection of loaded textures. */
    std::unordered_map<std::string, LoadedTexture> loaded_textures_;
};

}