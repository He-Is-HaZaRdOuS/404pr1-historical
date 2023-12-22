#include "CSV.h"

#include <iostream>

CSV::CSV(const char* path, const char separator) {
    FILE* fp = fopen(path, "r");
    if (fp == nullptr)
        throw errno;

    char in;
    std::string val;
    m_rows = 0;
    m_cols = -1;
    int col = 0;
    m_data.emplace_back();
    while (fscanf(fp, "%c", &in) != EOF) {
        if (in == separator) {
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
            if (col != m_cols)
                throw errno;
            col = 0;
            m_data.emplace_back();
            val = "";
        }
        else {
            val += in;
        }
    }
    m_cols = m_data.at(0).size();
}

void CSV::printRows(const std::string&separator) const {
    for (auto&i: m_data) {
        for (int j = 0; j < i.size(); ++j) {
            std::cout << i.at(j);
            if (j != i.size() - 1)
                std::cout << separator;
        }
        std::cout << std::endl;
    }
}
