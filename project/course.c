#include "course.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Safe memory allocation - pointer döndürür
void* safe_malloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(1);
    }
    return ptr;
}

// Safe string duplication - dinamik memory'de string kopyalar
char* safe_strdup(const char *str) {
    if (!str) return NULL;
    
    size_t len = strlen(str) + 1;
    char *new_str = (char*)safe_malloc(len);
    memcpy(new_str, str, len);  // Memory copy
    return new_str;
}

// Course oluşturma - dinamik memory allocation
Course* course_create(const char *name, const char *code, int credits) {
    // Course struct için memory allocate et
    Course *course = (Course*)safe_malloc(sizeof(Course));
    
    // String'ler için dinamik memory
    course->name = safe_strdup(name);
    course->code = safe_strdup(code);
    course->credits = credits;
    
    // Prerequisites için başlangıçta NULL
    course->prerequisites = NULL;
    course->prereq_count = 0;
    
    // ID ve pozisyon
    static int next_id = 1;
    course->id = next_id++;
    course->x = 100 + (course->id % 4) * 200;
    course->y = 100 + (course->id / 4) * 150;
    
    return course;
}

// Course memory'sini temizleme
void course_destroy(Course *course) {
    if (!course) return;
    
    // Dinamik string'leri free et
    free(course->name);
    free(course->code);
    
    // Prerequisites array'ini free et (ama Course'ları değil, onlar başka yerde)
    if (course->prerequisites) {
        free(course->prerequisites);
    }
    
    // Course struct'ın kendisini free et
    free(course);
}

// Ön koşul ekleme - pointer array manipülasyonu
void course_add_prerequisite(Course *course, Course *prerequisite) {
    if (!course || !prerequisite) return;
    
    // Array'i genişlet - realloc kullanarak
    course->prereq_count++;
    course->prerequisites = (Course**)realloc(
        course->prerequisites,
        course->prereq_count * sizeof(Course*)
    );
    
    if (!course->prerequisites) {
        fprintf(stderr, "Failed to allocate memory for prerequisites\n");
        exit(1);
    }
    
    // Yeni prerequisite'i ekle (pointer assignment)
    course->prerequisites[course->prereq_count - 1] = prerequisite;
}

// Course bilgilerini yazdır - pointer dereferencing
void course_print(const Course *course) {
    if (!course) return;
    
    printf("\n=== Course Info ===\n");
    printf("Code: %s\n", course->code);
    printf("Name: %s\n", course->name);
    printf("Credits: %d\n", course->credits);
    printf("ID: %d\n", course->id);
    printf("Position: (%d, %d)\n", course->x, course->y);
    
    if (course->prereq_count > 0) {
        printf("Prerequisites:\n");
        for (int i = 0; i < course->prereq_count; i++) {
            // Pointer dereference ile prerequisite'e eriş
            Course *prereq = course->prerequisites[i];
            printf("  - %s (%s)\n", prereq->code, prereq->name);
        }
    }
}

// ===== CourseList (Linked List) Implementation =====

CourseList* courselist_create(void) {
    CourseList *list = (CourseList*)safe_malloc(sizeof(CourseList));
    list->head = NULL;  // Başlangıçta boş liste
    list->count = 0;
    list->id_map = NULL;
    return list;
}

void courselist_destroy(CourseList *list) {
    if (!list) return;
    
    // Linked list'i traverse et ve her node'u temizle
    CourseNode *current = list->head;
    while (current) {
        CourseNode *next = current->next;  // Pointer'ı kaydet
        course_destroy(current->course_data);  // Course'u temizle
        free(current);  // Node'u temizle
        current = next;  // Sonraki node'a geç
    }
    
    if (list->id_map) {
        free(list->id_map);
    }
    
    free(list);
}

// Listeye course ekle - linked list pointer manipulation
void courselist_add(CourseList *list, Course *course) {
    if (!list || !course) return;
    
    // Yeni node oluştur
    CourseNode *new_node = (CourseNode*)safe_malloc(sizeof(CourseNode));
    new_node->course_data = course;  // Pointer assignment
    new_node->next = list->head;     // Yeni node başa eklenir
    
    list->head = new_node;  // Head pointer'ı güncelle
    list->count++;
}

// Code'a göre course bul - pointer comparison
Course* courselist_find_by_code(CourseList *list, const char *code) {
    if (!list || !code) return NULL;
    
    // Linked list'i traverse et
    CourseNode *current = list->head;
    while (current) {
        // String comparison
        if (strcmp(current->course_data->code, code) == 0) {
            return current->course_data;  // Pointer döndür
        }
        current = current->next;  // Sonraki node
    }
    
    return NULL;  // Bulunamadı
}

// ID'ye göre course bul
Course* courselist_find_by_id(CourseList *list, int id) {
    if (!list) return NULL;
    
    CourseNode *current = list->head;
    while (current) {
        if (current->course_data->id == id) {
            return current->course_data;
        }
        current = current->next;
    }
    
    return NULL;
}

// Tüm course'ları yazdır
void courselist_print_all(const CourseList *list) {
    if (!list) return;
    
    printf("\n===== All Courses =====\n");
    printf("Total courses: %d\n", list->count);
    
    CourseNode *current = list->head;
    while (current) {
        course_print(current->course_data);
        current = current->next;
    }
}