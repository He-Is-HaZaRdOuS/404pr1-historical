#ifndef CSV_H
#define CSV_H

#include <string>
#include <vector>

class CSV {
    std::vector<std::vector<std::string>> m_data;
    size_t m_rows;
    size_t m_cols;

public:
    explicit CSV(const char* path, char separator = ',');

    void printRows(const std::string&separator = ",") const;

    [[nodiscard]] size_t getColumnCount() const { return m_cols; }

    [[nodiscard]] size_t getRowCount() const { return m_rows; }
};

#endif //CSV_H
