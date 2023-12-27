#pragma once
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

    /**
     * \brief Constructs a new vector of all rows having a column with similar values
     * \param column index of the column whose value will be tested
     * \param filter function to be used in testing, returns true when value passes the test
     * \return an array containing copy of rows that pass the test.
     */
    Vector2<String> filter(int column, bool (*filter)(const std::string&value)) const;

    Vector2<String> filter(int column, std::string&value) const;
};

#endif //CSV_H
