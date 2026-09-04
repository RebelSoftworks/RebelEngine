#ifndef CUTILS_H
#define CUTILS_h

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

struct CUtlSymbol
{
    using UtlSymId_t = unsigned int;

    UtlSymId_t id = INVALID_SYMBOL;

    static constexpr UtlSymId_t INVALID_SYMBOL = static_cast<UtlSymId_t>(-1);

    CUtlSymbol() = default;

    CUtlSymbol(const char* str)
    {
        id = AddString(str);
    }

    const char* String() const
    {
        if (id == INVALID_SYMBOL)
            return "";

        return s_StringTable[id].c_str();
    }

    bool IsValid() const
    {
        return id != INVALID_SYMBOL;
    }

    bool operator==(const CUtlSymbol& other) const
    {
        return id == other.id;
    }

    bool operator!=(const CUtlSymbol& other) const
    {
        return id != other.id;
    }

private:
    static UtlSymId_t AddString(const char* str)
    {
        auto it = s_Lookup.find(str);

        if (it != s_Lookup.end())
            return it->second;

        UtlSymId_t newId = static_cast<UtlSymId_t>(s_StringTable.size());

        s_StringTable.emplace_back(str);
        s_Lookup[str] = newId;

        return newId;
    }

    static std::unordered_map<std::string, UtlSymId_t> s_Lookup;
    static std::vector<std::string> s_StringTable;
};
#endif