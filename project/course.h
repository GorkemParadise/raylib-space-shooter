#ifndef COURSE_H
#define COURSE_H

#include <stddef.h>  // size_t için

#define MAX_NAME_LEN 100
#define MAX_CODE_LEN 20
#define MAX_COURSES 50
#define MAX_PREREQUISITES 5

// Typedef'ler
typedef struct Course Course;
typedef struct CourseNode CourseNode;
typedef struct CourseList CourseList;

// Ders yapısı - pointer ve memory management için
struct Course {
    char *name;              // Dinamik string için pointer
    char *code;              // Dinamik string için pointer
    int credits;
    Course **prerequisites;  // Pointer to pointer array (dinamik ön koşul listesi)
    int prereq_count;
    int id;                  // Unique identifier
    int x, y;               // UI'de pozisyon için koordinatlar
};

// Linked list node yapısı
struct CourseNode {
    Course *course_data;     // Course struct'ına pointer
    CourseNode *next;        // Sonraki node'a pointer
};

// Course list manager
struct CourseList {
    CourseNode *head;        // Liste başına pointer
    int count;
    int *id_map;            // ID mapping için pointer array
};

// Union - farklı veri tiplerini aynı memory'de tutmak için
typedef union {
    int int_value;
    float float_value;
    char *string_value;
    void *pointer_value;
} CourseProperty;

// Function prototypes
Course* course_create(const char *name, const char *code, int credits);
void course_destroy(Course *course);
void course_add_prerequisite(Course *course, Course *prerequisite);
void course_print(const Course *course);

CourseList* courselist_create(void);
void courselist_destroy(CourseList *list);
void courselist_add(CourseList *list, Course *course);
Course* courselist_find_by_code(CourseList *list, const char *code);
Course* courselist_find_by_id(CourseList *list, int id);
void courselist_print_all(const CourseList *list);

// Memory utility functions
void* safe_malloc(size_t size);
char* safe_strdup(const char *str);

#endif // COURSE_H