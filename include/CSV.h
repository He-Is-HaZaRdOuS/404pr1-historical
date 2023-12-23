#ifndef CSV_H
#define CSV_H

#include "Typedefs.h"

class CSV {
    Vector2<String> m_data;
    size_t m_rows;
    size_t m_cols;

public:
    explicit CSV(const char* path, char separator = ',');

    void printRows(const std::string&separator = ",") const;

    [[nodiscard]] size_t getColumnCount() const { return m_cols; }

    [[nodiscard]] size_t getRowCount() const { return m_rows; }

    Vector2<String> getData();
};

#endif //CSV_H
