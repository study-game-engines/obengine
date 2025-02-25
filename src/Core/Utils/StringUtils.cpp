#include <algorithm>
#include <cctype>

#include <Utils/MathUtils.hpp>
#include <Utils/StringUtils.hpp>

namespace obe::utils::string
{
    std::vector<std::string> split(const std::string& str, const std::string& delimiters)
    {
        std::vector<std::string> tokens;
        std::string::size_type last_pos = str.find_first_not_of(delimiters, 0);
        std::string::size_type pos = str.find_first_of(delimiters, last_pos);
        while (std::string::npos != pos || std::string::npos != last_pos)
        {
            tokens.push_back(str.substr(last_pos, pos - last_pos));
            last_pos = str.find_first_not_of(delimiters, pos);
            pos = str.find_first_of(delimiters, last_pos);
        }
        return tokens;
    }

    int occurences_in_string(const std::string& str, const std::string& occur)
    {
        int occurrences = 0;
        std::string::size_type start = 0;
        while ((start = str.find(occur, start)) != std::string::npos)
        {
            ++occurrences;
            start += occur.length();
        }
        return occurrences;
    }

    bool is_string_alpha(const std::string& str)
    {
        if (!str.empty())
            return std::all_of(str.begin(), str.end(), isalpha);
        return false;
    }

    bool is_string_alpha_numeric(const std::string& str)
    {
        if (!str.empty())
            return std::all_of(str.begin(), str.end(), isalnum);
        return false;
    }

    bool is_string_int(const std::string& str)
    {
        if (!str.empty())
        {
            if (str.substr(0, 1) == "-")
            {
                std::string without_sign = str.substr(1);
                return std::all_of(without_sign.begin(), without_sign.end(), isdigit);
            }
            return std::all_of(str.begin(), str.end(), isdigit);
        }
        return false;
    }

    bool is_string_float(const std::string& str)
    {
        std::string modify_str = str;
        if (!modify_str.empty())
        {
            bool is_float = false;
            if (modify_str.substr(0, 1) == "-")
                modify_str = modify_str.substr(1);
            if (occurences_in_string(modify_str, ".") == 1)
            {
                is_float = true;
                replace_in_place(modify_str, ".", "");
            }
            return (std::all_of(modify_str.begin(), modify_str.end(), isdigit) && is_float);
        }
        return false;
    }

    bool is_string_numeric(const std::string& str)
    {
        return (is_string_float(str) || is_string_int(str));
    }

    void replace_in_place(
        std::string& subject, const std::string& search, const std::string& replace)
    {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != std::string::npos)
        {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
    }

    std::string replace(std::string subject, const std::string& search, const std::string& replace)
    {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != std::string::npos)
        {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return subject;
    }

    bool is_surrounded_by(const std::string& string, const std::string& bet)
    {
        return (string.substr(0, bet.size()) == bet
            && string.substr(string.size() - bet.size(), bet.size()) == bet);
    }

    std::string get_random_key(const std::string& set, const int len)
    {
        std::string r;
        for (int i = 0; i < len; i++)
            r.push_back(set.at(static_cast<size_t>(math::randint(0, 100000) % set.size())));
        return r;
    }

    bool contains(const std::string& string, const std::string& search)
    {
        return (string.find(search) != std::string::npos);
    }

    bool starts_with(const std::string& string, const std::string& search)
    {
        if (string.size() < search.size())
            return false;
        return (std::mismatch(search.begin(), search.end(), string.begin()).first == search.end());
    }

    bool ends_with(const std::string& string, const std::string& search)
    {
        if (string.size() < search.size())
        {
            return false;
        }
        return (
            std::mismatch(search.rbegin(), search.rend(), string.rbegin()).first == search.rend());
    }

    std::size_t distance(std::string_view source, std::string_view target)
    {
        if (source.size() > target.size())
        {
            return distance(target, source);
        }

        const std::size_t min_size = source.size(), max_size = target.size();
        std::vector<std::size_t> lev_dist(min_size + 1);

        for (std::size_t i = 0; i <= min_size; ++i)
        {
            lev_dist[i] = i;
        }

        for (std::size_t j = 1; j <= max_size; ++j)
        {
            std::size_t previous_diagonal = lev_dist[0];
            ++lev_dist[0];

            for (std::size_t i = 1; i <= min_size; ++i)
            {
                const std::size_t previous_diagonal_save = lev_dist[i];
                if (source[i - 1] == target[j - 1])
                {
                    lev_dist[i] = previous_diagonal;
                }
                else
                {
                    lev_dist[i]
                        = std::min(std::min(lev_dist[i - 1], lev_dist[i]), previous_diagonal) + 1;
                }
                previous_diagonal = previous_diagonal_save;
            }
        }

        return lev_dist[min_size];
    }

    std::vector<std::string> sort_by_distance(
        const std::string& source, const std::vector<std::string>& words, std::size_t limit)
    {
        std::vector<std::string> sorted_by_distance = words;
        std::sort(sorted_by_distance.begin(), sorted_by_distance.end(),
            [source](const std::string& s1, const std::string& s2) {
                return utils::string::distance(s1, source) < utils::string::distance(s2, source);
            });
        if (limit && !sorted_by_distance.empty())
        {
            return std::vector<std::string>(sorted_by_distance.begin(),
                sorted_by_distance.begin() + std::min(sorted_by_distance.size() - 1, limit));
        }
        else
            return sorted_by_distance;
    }

    std::string quote(const std::string& source)
    {
        return "\"" + source + "\"";
    }

    std::string titleize(const std::string& source)
    {
        if (!source.empty())
        {
            return static_cast<char>(toupper(source[0])) + source.substr(1);
        }
        return source;
    }
} // namespace obe::utils::string
