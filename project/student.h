#ifndef STUDENT_H
#define STUDENT_H

#include "course.h"

#define MAX_STUDENT_NAME 100
#define MAX_ENROLLED_COURSES 20

// Student yapısı
typedef struct Student {
    char *name;
    char *student_id;
    int semester;
    Course **enrolled_courses;  // Pointer to pointer array
    int enrolled_count;
    int total_credits;
} Student;

// Union örneği - farklı student bilgi tipleri için
typedef union StudentInfo {
    int semester_number;
    float gpa;
    char *department;
    struct {
        int year;
        int month;
    } enrollment_date;
} StudentInfo;

// Bitfield örneği - memory optimization
typedef struct StudentFlags {
    unsigned int is_active : 1;      // 1 bit
    unsigned int is_scholarship : 1; // 1 bit
    unsigned int year : 3;           // 3 bit (0-7 yıl)
    unsigned int reserved : 27;      // Geri kalan bitler
} StudentFlags;

// Student functions
Student* student_create(const char *name, const char *id, int semester);
void student_destroy(Student *student);
void student_enroll_course(Student *student, Course *course);
void student_print(const Student *student);
int student_total_credits(const Student *student);

#endif // STUDENT_H