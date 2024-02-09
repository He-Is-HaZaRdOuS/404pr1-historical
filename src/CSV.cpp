#include "CSV.h"

#include <iostream>
#include <algorithm>

#include "Typedefs.h"

CSV::CSV(const char* path, const char separator) : m_rows(0), m_cols(-1) {
    FILE* fp = fopen(path, "r");
    if (fp == nullptr)
        throw errno;

    char in;
    std::string val;
    int col = 0;
    m_data.emplace_back();
    while (fscanf(fp, "%c", &in) != EOF) {
        if (in == separator) {
            transform(val.begin(), val.end(), val.begin(), static_cast<int (*)(int)>(&std::toupper));
            m_data.at(m_rows).push_back(val);
            val = "";
            col++;
        }
        else if (in == '\n') {
            m_data.at(m_rows).push_back(val);
            m_rows++;
            col++;
            if (m_cols == -1)
                m_cols = col;
            if (col != m_cols) {
                throw errno;
            }

            col = 0;
            m_data.emplace_back();
            val = "";
        }
        else {
            val += in;
        }
    }
    m_cols = static_cast<long>(m_data.at(0).size());
    m_data.pop_back();
}

void CSV::printRows(const std::string&separator) const {
    for (const auto& row: m_data) {
        const u_int32_t rowSize = row.size();
        for (u_int32_t col = 0; col < rowSize; ++col) {
            std::cout << row.at(col);
            if (col != rowSize - 1)
                std::cout << separator;
        }
        std::cout << std::endl;
    }
}

Vector2D<std::string> CSV::filter(const long column, bool (*filter)(const std::string&)) const {
    auto matches = Vector2D<std::string>(0);
    if (column < 0 || column > m_cols) return matches;
    for (auto&row : m_data) {
        if (filter(row.at(column))) {
            matches.push_back(row);
        }
    }
    return matches;
}


Vector2D<std::string> CSV::filter(int column, const std::string&value) const {
    auto matches = Vector2D<std::string>(0);
    if (column < 0 || column > m_cols) return matches;
    for (auto&row : m_data) {
        if (row.at(column) == value) {
            matches.push_back(row);
        }
    }
    return matches;
}

Vector2D<std::string> CSV::getData() {
    return m_data;
}

void CSV::removeHeader() {
    --m_rows;
    m_data.erase(m_data.begin(), m_data.begin() + 1);
}
