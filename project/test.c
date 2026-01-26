#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "course.h"
#include "student.h"

// Basit ASCII art ile course bağlantılarını göster
void print_course_tree(CourseList *list) {
    printf("\n╔════════════════════════════════════════════════╗\n");
    printf("║        COURSE PREREQUISITE TREE               ║\n");
    printf("╚════════════════════════════════════════════════╝\n\n");
    
    CourseNode *current = list->head;
    
    while (current) {
        Course *course = current->course_data;
        
        printf("┌─────────────────────────────┐\n");
        printf("│ %-28s│\n", course->code);
        printf("│ %-28s│\n", course->name);
        printf("│ Credits: %-18d │\n", course->credits);
        printf("└─────────────────────────────┘\n");
        
        if (course->prereq_count > 0) {
            for (int i = 0; i < course->prereq_count; i++) {
                Course *prereq = course->prerequisites[i];
                if (i < course->prereq_count - 1) {
                    printf("    ├──► %s (%s)\n", prereq->code, prereq->name);
                } else {
                    printf("    └──► %s (%s)\n", prereq->code, prereq->name);
                }
            }
        }
        printf("\n");
        
        current = current->next;
    }
}

// Memory demonstrasyonu
void demonstrate_pointers(void) {
    printf("\n╔════════════════════════════════════════════════╗\n");
    printf("║     POINTER & MEMORY DEMONSTRATION            ║\n");
    printf("╚════════════════════════════════════════════════╝\n\n");
    
    // Single pointer
    int value = 42;
    int *ptr = &value;
    
    printf("1. SINGLE POINTER\n");
    printf("   Variable value: %d\n", value);
    printf("   Variable address: %p\n", (void*)&value);
    printf("   Pointer ptr: %p\n", (void*)ptr);
    printf("   Pointer dereferenced (*ptr): %d\n\n", *ptr);
    
    // Double pointer
    int **ptr_ptr = &ptr;
    
    printf("2. DOUBLE POINTER\n");
    printf("   Pointer to pointer (ptr_ptr): %p\n", (void*)ptr_ptr);
    printf("   Dereferenced once (*ptr_ptr): %p\n", (void*)*ptr_ptr);
    printf("   Dereferenced twice (**ptr_ptr): %d\n\n", **ptr_ptr);
    
    // Dynamic memory
    Course *course = course_create("Test Course", "TEST101", 3);
    
    printf("3. DYNAMIC MEMORY\n");
    printf("   Course struct address: %p\n", (void*)course);
    printf("   Course->name pointer: %p\n", (void*)course->name);
    printf("   Course->name value: %s\n", course->name);
    printf("   Sizeof(Course): %zu bytes\n\n", sizeof(Course));
    
    course_destroy(course);
}

// Union demonstrasyonu
void demonstrate_unions(void) {
    printf("\n╔════════════════════════════════════════════════╗\n");
    printf("║          UNION DEMONSTRATION                  ║\n");
    printf("╚════════════════════════════════════════════════╝\n\n");
    
    CourseProperty prop;
    
    printf("Union size: %zu bytes\n", sizeof(CourseProperty));
    printf("Union address: %p\n\n", (void*)&prop);
    
    printf("Setting int value to 100...\n");
    prop.int_value = 100;
    printf("  int_value: %d\n", prop.int_value);
    printf("  Address: %p\n\n", (void*)&prop.int_value);
    
    printf("Setting float value to 3.14...\n");
    prop.float_value = 3.14f;
    printf("  float_value: %.2f\n", prop.float_value);
    printf("  int_value (corrupted): %d\n", prop.int_value);
    printf("  Address: %p (same as int!)\n\n", (void*)&prop.float_value);
    
    printf("Setting string pointer...\n");
    prop.string_value = "Hello Union!";
    printf("  string_value: %s\n", prop.string_value);
    printf("  float_value (corrupted): %.2f\n", prop.float_value);
    printf("  Address: %p (same memory!)\n\n", (void*)&prop.string_value);
    
    printf("NOTE: All union members share the SAME memory!\n");
}

// Örnek dersler
void setup_sample_courses(CourseList *list) {
    printf("\n📚 Setting up sample Computer Science curriculum...\n\n");
    
    // 1. Semester
    Course *cs101 = course_create("Introduction to Programming", "CS101", 4);
    Course *math101 = course_create("Calculus I", "MATH101", 4);
    Course *phys101 = course_create("Physics I", "PHYS101", 3);
    Course *eng101 = course_create("English I", "ENG101", 3);
    
    courselist_add(list, cs101);
    courselist_add(list, math101);
    courselist_add(list, phys101);
    courselist_add(list, eng101);
    
    // 2. Semester
    Course *cs102 = course_create("Data Structures", "CS102", 4);
    course_add_prerequisite(cs102, cs101);
    courselist_add(list, cs102);
    
    Course *math102 = course_create("Calculus II", "MATH102", 4);
    course_add_prerequisite(math102, math101);
    courselist_add(list, math102);
    
    // 3. Semester
    Course *cs201 = course_create("Algorithms", "CS201", 4);
    course_add_prerequisite(cs201, cs102);
    course_add_prerequisite(cs201, math101);
    courselist_add(list, cs201);
    
    Course *cs202 = course_create("Database Systems", "CS202", 3);
    course_add_prerequisite(cs202, cs102);
    courselist_add(list, cs202);
    
    Course *cs203 = course_create("Computer Architecture", "CS203", 4);
    course_add_prerequisite(cs203, cs102);
    courselist_add(list, cs203);
    
    // 4. Semester
    Course *cs301 = course_create("Operating Systems", "CS301", 4);
    course_add_prerequisite(cs301, cs201);
    course_add_prerequisite(cs301, cs203);
    courselist_add(list, cs301);
    
    Course *cs302 = course_create("Computer Networks", "CS302", 3);
    course_add_prerequisite(cs302, cs202);
    course_add_prerequisite(cs302, cs203);
    courselist_add(list, cs302);
    
    // 5. Semester - Advanced
    Course *cs401 = course_create("Machine Learning", "CS401", 4);
    course_add_prerequisite(cs401, cs201);
    course_add_prerequisite(cs401, math102);
    courselist_add(list, cs401);
    
    Course *cs402 = course_create("Compiler Design", "CS402", 3);
    course_add_prerequisite(cs402, cs201);
    course_add_prerequisite(cs402, cs203);
    courselist_add(list, cs402);
    
    printf("✅ Created %d courses with prerequisites!\n", list->count);
}

int main(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║                                                      ║\n");
    printf("║          COURSE PLANNER - C Project                  ║\n");
    printf("║                                                      ║\n");
    printf("║  Features:                                           ║\n");
    printf("║  • Structures & Typedef                              ║\n");
    printf("║  • Unions                                            ║\n");
    printf("║  • Pointers (single, double, triple)                 ║\n");
    printf("║  • Dynamic Memory Management                         ║\n");
    printf("║  • Linked Lists                                      ║\n");
    printf("║                                                      ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");
    
    // Demonstrasyonlar
    demonstrate_pointers();
    demonstrate_unions();
    
    // Course list oluştur
    CourseList *courses = courselist_create();
    setup_sample_courses(courses);
    
    // Course tree'yi göster
    print_course_tree(courses);
    
    // Memory adresleri
    printf("\n╔════════════════════════════════════════════════╗\n");
    printf("║         MEMORY LAYOUT ANALYSIS                 ║\n");
    printf("╚════════════════════════════════════════════════╝\n\n");
    
    printf("CourseList struct address: %p\n", (void*)courses);
    printf("Head pointer value: %p\n", (void*)courses->head);
    printf("Total courses: %d\n\n", courses->count);
    
    int count = 0;
    CourseNode *node = courses->head;
    while (node && count < 3) {
        Course *c = node->course_data;
        printf("Course #%d: %s\n", count + 1, c->code);
        printf("  Node address: %p\n", (void*)node);
        printf("  Course address: %p\n", (void*)c);
        printf("  Name pointer: %p -> \"%s\"\n", (void*)c->name, c->name);
        printf("  Code pointer: %p -> \"%s\"\n", (void*)c->code, c->code);
        printf("  Prerequisites: %d courses\n", c->prereq_count);
        if (c->prereq_count > 0) {
            printf("  Prereq array address: %p\n", (void*)c->prerequisites);
            for (int i = 0; i < c->prereq_count; i++) {
                printf("    [%d] %p -> %s\n", i, 
                       (void*)c->prerequisites[i], 
                       c->prerequisites[i]->code);
            }
        }
        printf("  Next node: %p\n\n", (void*)node->next);
        
        node = node->next;
        count++;
    }
    
    // İstatistikler
    printf("\n╔════════════════════════════════════════════════╗\n");
    printf("║             STATISTICS                         ║\n");
    printf("╚════════════════════════════════════════════════╝\n\n");
    
    printf("Total courses: %d\n", courses->count);
    printf("Memory used:\n");
    printf("  CourseList: %zu bytes\n", sizeof(CourseList));
    printf("  Course struct: %zu bytes\n", sizeof(Course));
    printf("  CourseNode: %zu bytes\n", sizeof(CourseNode));
    printf("  Total for all courses: ~%zu bytes\n\n", 
           courses->count * (sizeof(Course) + sizeof(CourseNode)));
    
    // Temizlik
    printf("🧹 Cleaning up memory...\n");
    courselist_destroy(courses);
    printf("✅ All memory freed successfully!\n\n");
    
    printf("═══════════════════════════════════════════════════════\n");
    printf("To compile and run the full GTK version:\n");
    printf("  1. Install dependencies: sudo ./install_deps.sh\n");
    printf("  2. Compile: make\n");
    printf("  3. Run: ./course_planner\n");
    printf("═══════════════════════════════════════════════════════\n\n");
    
    return 0;
}