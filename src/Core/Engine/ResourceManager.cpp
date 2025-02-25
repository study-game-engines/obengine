#include <Engine/Exceptions.hpp>
#include <Engine/ResourceManager.hpp>
#include <System/Path.hpp>

namespace obe::engine
{
    const graphics::Texture& ResourceManager::get_texture(
        const system::Path& path, bool anti_aliasing)
    {
        const std::string path_as_string = path.to_string();
        if (!m_textures.contains(path_as_string)
            || (!m_textures[path_as_string].first && !anti_aliasing)
            || (!m_textures[path_as_string].second && anti_aliasing))
        {
            graphics::Texture temp_texture = graphics::Texture::make_shared_texture();
            const system::FindResult search_result = path.find();
            const std::string& texture_path = search_result.path();
            debug::Log->debug(
                "[ResourceManager] Loading <Texture> {} from {}", path_as_string, texture_path);

            if (temp_texture.load_from_file(texture_path))
            {
                temp_texture.set_anti_aliasing(anti_aliasing);
                if (!anti_aliasing)
                {
                    m_textures[path_as_string].first
                        = std::make_unique<graphics::Texture>(temp_texture);
                    return *m_textures[path_as_string].first;
                }
                else
                {
                    m_textures[path_as_string].second
                        = std::make_unique<graphics::Texture>(temp_texture);
                    return *m_textures[path_as_string].second;
                }
            }
            else
                throw exceptions::TextureNotFound(texture_path);
        }
        else
        {
            if (anti_aliasing)
            {
                return *m_textures[path_as_string].second;
            }
            else
            {
                return *m_textures[path_as_string].first;
            }
        }
    }

    const graphics::Texture& ResourceManager::get_texture(const system::Path& path)
    {
        return get_texture(path, default_anti_aliasing);
    }

    void ResourceManager::clean()
    {
        for (auto& texture_pair : m_textures)
        {
            if (texture_pair.second.first && texture_pair.second.first->use_count() == 1)
            {
                texture_pair.second.first.reset();
            }
            if (texture_pair.second.second && texture_pair.second.second->use_count() == 1)
            {
                texture_pair.second.second.reset();
            }
        }
    }

    ResourceManager::ResourceManager()
        : default_anti_aliasing(false)
    {
    }

    std::shared_ptr<graphics::Font> ResourceManager::get_font(const std::string& path)
    {
        if (!m_fonts.contains(path))
        {
            const system::FindResult search_result
                = system::Path(path).find(system::PathType::File);
            std::shared_ptr<graphics::Font> new_font = std::make_shared<graphics::Font>();
            new_font->load_from_file(search_result);

            if (search_result.success())
            {
                debug::Log->debug(
                    "[ResourceManager] Loading <Font> {} from {}", path, search_result.path());
                m_fonts[path] = move(new_font);
            }
            else
                throw exceptions::FontNotFound(path, system::MountablePath::string_paths());
        }
        return m_fonts[path];
    }

    void ResourceManagedObject::remove_resource_manager()
    {
        m_resources = nullptr;
    }

    void ResourceManagedObject::attach_resource_manager(ResourceManager& resources)
    {
        m_resources = &resources;
    }
} // namespace obe::graphics
