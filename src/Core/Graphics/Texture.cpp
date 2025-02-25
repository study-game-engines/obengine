#include <lunasvg.h>

#include <Graphics/Exceptions.hpp>
#include <Graphics/Texture.hpp>
#include <Transform/Rect.hpp>
#include <Utils/Visitor.hpp>

namespace obe::graphics
{
    namespace
    {
        sf::IntRect to_sfml_rect(const transform::AABB& rect)
        {
            const transform::UnitVector position
                = rect.get_position().to<transform::Units::ScenePixels>();
            const transform::UnitVector size
                = rect.get_position().to<transform::Units::ScenePixels>();
            const sf::IntRect sf_rect(position.x, position.y, size.x, size.y);
            return sf_rect;
        }
    }

    void SvgTexture::render() const
    {
        if (!success())
        {
            return;
        }
        const auto bitmap = m_document->renderToBitmap(m_size_hint.width, m_size_hint.height);
        sf::Image image;
        image.create(bitmap.width(), bitmap.height(), bitmap.data());
        m_texture->loadFromImage(image);
    }

    SvgTexture::SvgTexture(const std::string& filename)
        : m_path(filename)
    {
        m_document = lunasvg::Document::loadFromFile(filename);
        m_texture = std::make_unique<sf::Texture>();
        render();
    }

    SvgTexture::SvgTexture(const SvgTexture& texture)
        : SvgTexture(texture.m_path)
    {
        m_size_hint.width = texture.m_size_hint.width;
        m_size_hint.height = texture.m_size_hint.height;
    }

    SvgTexture& SvgTexture::operator=(const SvgTexture& texture)
    {
        m_path = texture.m_path;
        m_document = lunasvg::Document::loadFromFile(m_path);
        m_size_hint.width = texture.m_size_hint.width;
        m_size_hint.height = texture.m_size_hint.height;
        render();

        return *this;
    }

    SvgTexture& SvgTexture::operator=(SvgTexture&& texture) noexcept
    {
        m_path = std::move(texture.m_path);
        m_document = std::move(texture.m_document);
        m_texture = std::move(texture.m_texture);
        m_size_hint.width = texture.m_size_hint.width;
        m_size_hint.height = texture.m_size_hint.height;
        render();

        return *this;
    }

    bool SvgTexture::is_autoscaled() const
    {
        return m_autoscaling;
    }

    void SvgTexture::set_autoscaling(const bool autoscaling)
    {
        m_autoscaling = autoscaling;
    }

    void SvgTexture::set_size_hint(unsigned width, unsigned height)
    {
        if (m_size_hint.width != width || m_size_hint.height != height)
        {
            m_size_hint.width = width;
            m_size_hint.height = height;
            if (m_autoscaling)
            {
                render();
            }
        }
    }

    bool SvgTexture::success() const
    {
        return static_cast<bool>(m_document);
    }

    const sf::Texture& SvgTexture::get_texture() const
    {
        return *m_texture;
    }

    sf::Texture& SvgTexture::get_texture()
    {
        return *m_texture;
    }

    sf::Texture& Texture::get_mutable_texture()
    {
        constexpr static obe::utils::Visitor visitor { [](sf::Texture& texture) -> sf::Texture& {
                                                          return texture;
                                                      },
            [](std::shared_ptr<sf::Texture>& texture) -> sf::Texture& { return *texture; },
            [](const sf::Texture*) -> sf::Texture& { throw exceptions::ReadOnlyTexture("create"); },
            [](SvgTexture& texture) -> sf::Texture& { return texture.get_texture(); } };
        return std::visit(visitor, m_texture);
    }

    const sf::Texture& Texture::get_texture() const
    {
        constexpr static obe::utils::Visitor visitor {
            [](const sf::Texture& texture) -> const sf::Texture& { return texture; },
            [](const std::shared_ptr<sf::Texture>& texture) -> const sf::Texture& {
                return *texture;
            },
            [](const sf::Texture* texture) -> const sf::Texture& { return *texture; },
            [](const SvgTexture& texture) -> const sf::Texture& { return texture.get_texture(); }
        };
        return std::visit(visitor, m_texture);
    }

    Texture Texture::make_shared_texture()
    {
        const std::shared_ptr<sf::Texture> empty = std::make_shared<sf::Texture>();
        return Texture(empty);
    }

    Texture::Texture()
    {
        m_texture = sf::Texture {};
        static_assert(std::is_same_v<decltype(m_texture), TextureWrapper>, "");
        m_pixels.reset();
    }

    Texture::Texture(std::shared_ptr<sf::Texture> texture)
        : m_texture(texture)
    {
        m_pixels.reset();
    }

    Texture::Texture(const sf::Texture& texture)
    {
        m_texture = &texture;
        m_pixels.reset();
    }

    Texture::Texture(const Texture& copy)
    {
        if (std::holds_alternative<sf::Texture>(copy.m_texture))
        {
            m_texture = &std::get<sf::Texture>(copy.m_texture);
        }
        else
        {
            m_texture = copy.m_texture;
        }
        m_pixels.reset();
    }

    bool Texture::create(unsigned int width, unsigned int height)
    {
        m_pixels.reset();
        return get_mutable_texture().create(width, height);
    }

    bool Texture::load_from_file(const std::string& filename)
    {
        if (utils::string::ends_with(filename, ".svg"))
        {
            m_texture = SvgTexture(filename);
            return std::get<SvgTexture>(m_texture).success();
        }
        m_pixels.reset();
        return get_mutable_texture().loadFromFile(filename);
    }

    bool Texture::load_from_file(const std::string& filename, const transform::AABB& rect)
    {
        const sf::IntRect sf_rect = to_sfml_rect(rect);
        if (utils::string::ends_with(filename, ".svg"))
        {
            m_texture = SvgTexture(filename);
            // TODO: Implement load_from_file(path, rect)
            return std::get<SvgTexture>(m_texture).success();
        }
        m_pixels.reset();
        return get_mutable_texture().loadFromFile(filename, sf_rect);
    }

    bool Texture::load_from_image(const sf::Image& image)
    {
        m_pixels.reset();
        return get_mutable_texture().loadFromImage(image);
    }

    transform::UnitVector Texture::get_size() const
    {
        const sf::Vector2u texture_size = get_texture().getSize();
        return transform::UnitVector(texture_size.x, texture_size.y, transform::Units::ScenePixels);
    }

    void Texture::set_size_hint(unsigned int width, unsigned int height)
    {
        if (std::holds_alternative<SvgTexture>(m_texture))
        {
            m_pixels.reset();
            std::get<SvgTexture>(m_texture).set_size_hint(width, height);
        }
    }

    bool Texture::is_autoscaled() const
    {
        if (std::holds_alternative<SvgTexture>(m_texture))
        {
            return std::get<SvgTexture>(m_texture).is_autoscaled();
        }
        return false;
    }

    void Texture::set_autoscaling(bool autoscaling)
    {
        if (std::holds_alternative<SvgTexture>(m_texture))
        {
            m_pixels.reset();
            return std::get<SvgTexture>(m_texture).set_autoscaling(autoscaling);
        }
    }

    void Texture::set_anti_aliasing(bool anti_aliasing)
    {
        get_mutable_texture().setSmooth(anti_aliasing);
        m_pixels.reset();
    }

    bool Texture::is_anti_aliased() const
    {
        return get_texture().isSmooth();
    }

    void Texture::set_repeated(bool repeated)
    {
        get_mutable_texture().setRepeated(repeated);
    }

    bool Texture::is_repeated() const
    {
        return get_texture().isRepeated();
    }

    void Texture::reset()
    {
        m_texture = sf::Texture {};
        m_pixels.reset();
    }

    unsigned int Texture::use_count() const
    {
        if (std::holds_alternative<sf::Texture>(m_texture))
        {
            return 1;
        }
        if (std::holds_alternative<std::shared_ptr<sf::Texture>>(m_texture))
        {
            return std::get<std::shared_ptr<sf::Texture>>(m_texture).use_count();
        }
        if (std::holds_alternative<const sf::Texture*>(m_texture))
        {
            return 0;
        }
        return 0;
    }

    bool Texture::is_vector() const
    {
        return std::holds_alternative<SvgTexture>(m_texture);
    }

    bool Texture::is_bitmap() const
    {
        return !is_vector();
    }

    Texture::operator sf::Texture&()
    {
        return get_mutable_texture();
    }

    Texture::operator const sf::Texture&() const
    {
        return get_texture();
    }

    Texture& Texture::operator=(const Texture& copy)
    {
        if (std::holds_alternative<sf::Texture>(copy.m_texture))
        {
            m_texture = &std::get<sf::Texture>(copy.m_texture);
        }
        else
        {
            m_texture = copy.m_texture;
        }
        m_pixels.reset();
        return *this;
    }

    Texture& Texture::operator=(const sf::Texture& texture)
    {
        m_texture = &texture;
        return *this;
    }

    Texture& Texture::operator=(std::shared_ptr<sf::Texture> texture)
    {
        m_texture = texture;
        return *this;
    }

    TexturePart Texture::make_texture_part() const
    {
        auto size = this->get_size();
        return TexturePart(*this, transform::AABB(transform::UnitVector(0, 0), size));
    }

    Color Texture::get_pixel(uint32_t x, uint32_t y) const
    {
        if (!m_pixels)
        {
            m_pixels = get_texture().copyToImage();
        }
        const auto image_size = m_pixels->getSize();
        if (x >= image_size.x || y >= image_size.y)
        {
            throw exceptions::InvalidTexturePixelCoord(x, y, image_size.x, image_size.y);
        }
        sf::Color pixel = m_pixels.value().getPixel(x, y);
        return Color(pixel);
    }

    TexturePart::TexturePart(const Texture& texture, transform::AABB rect)
        : m_texture(texture)
        , m_rect(rect)
    {
    }

    const Texture& TexturePart::get_texture() const
    {
        return m_texture;
    }

    const transform::AABB& TexturePart::get_texture_rect() const
    {
        return m_rect;
    }

    transform::UnitVector TexturePart::get_size() const
    {
        return m_rect.get_size();
    }
} //namespace obe::graphics
