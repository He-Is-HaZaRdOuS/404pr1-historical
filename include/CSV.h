#pragma once
#ifndef CSV_H
#define CSV_H

#include "Main.h"
#include "Typedefs.h"
#include "Course.h"
#include "Classroom.h"
#include "Solution.h"

#include <string>

/* function prototypes */
void loadCourses(const char* path, std::vector<Course>* courses);

void loadClassrooms(const char* path, std::vector<Classroom>* classrooms);

void loadBlockedHours(const char* path, Vector2D<std::string>* blockedHours);

class CSV {
private:
    Vector2D<std::string> m_data;
    int64_t m_rows;
    int64_t m_cols;

public:
    explicit CSV(const char* path, char separator = ',');
    CSV() = default;

  void printRows(const std::string&separator = ",") const;

    [[nodiscard]] size_t getColumnCount() const { return m_cols; }

    [[nodiscard]] size_t getRowCount() const { return m_rows; }

    Vector2D<std::string> getData();

    /**
     * \brief Constructs a new vector of all rows having a column with similar values
     * \param column index of the column whose value will be tested
     * \param filter function to be used in testing, returns true when value passes the test
     * \return an array containing copy of rows that pass the test.
     */
    Vector2D<std::string> filter(long column, bool (*filter)(const std::string&value)) const;

    [[nodiscard]] Vector2D<std::string> filter(int column, const std::string &value) const;

    void removeHeader();
};

#endif //CSV_H
