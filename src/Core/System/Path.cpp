#include <System/Path.hpp>
#include <Utils/StringUtils.hpp>

namespace obe::System
{
    Path::Path()
        : m_mounts(MountablePath::Paths())
    {
    }

    Path::Path(const std::vector<MountablePath>& mount)
        : m_mounts(mount)
    {
    }

    Path::Path(const Path& path)
        : m_mounts(path.m_mounts)
    {
        this->m_path = path.toString();
    }

    Path::Path(std::string_view path)
        : m_mounts(MountablePath::Paths())
    {
        m_path = path;
    }

    Path& Path::set(const std::string& path)
    {
        m_path = path;
        return *this;
    }

    Path Path::add(const std::string& path) const
    {
        return Path(
            m_path + ((!m_path.empty() && m_path.back() != '/') ? "/" : "") + path);
    }

    std::string Path::last() const
    {
        std::vector<std::string> splitPath = Utils::String::split(m_path, "/");
        return splitPath.back();
    }

    Path Path::getPath(const unsigned int index)
    {
        if (index < m_mounts.size())
            return Path(m_mounts[index].basePath).add(m_path);
        throw aube::ErrorHandler::Raise("ObEngine.System.Path.UnknownPathAtIndex",
            { { "index", std::to_string(index) }, { "path", m_path } });
    }

    std::string Path::find(PathType pathType) const
    {
        for (const MountablePath& mountedPath : m_mounts)
        {
            if ((pathType == PathType::All || pathType == PathType::File)
                && Utils::File::fileExists(mountedPath.basePath
                    + ((!mountedPath.basePath.empty()) ? "/" : "") + this->m_path))
            {
                return mountedPath.basePath + ((!mountedPath.basePath.empty()) ? "/" : "")
                    + this->m_path;
            }
            else if ((pathType == PathType::All || pathType == PathType::Directory)
                && Utils::File::directoryExists(mountedPath.basePath
                    + ((!mountedPath.basePath.empty()) ? "/" : "") + this->m_path))
            {
                return mountedPath.basePath + ((!mountedPath.basePath.empty()) ? "/" : "")
                    + this->m_path;
            }
        }
        return "";
    }

    std::string Path::toString() const
    {
        return m_path;
    }

    void Path::operator=(const Path& path)
    {
        m_path = path.m_path;
    }
} // namespace obe::System

obe::System::Path operator"" _path(const char* str, std::size_t len)
{
    return obe::System::Path(std::string(str, len));
}

std::string operator""_fs(const char* str, std::size_t len)
{
    return obe::System::Path(std::string(str, len)).find();
}
