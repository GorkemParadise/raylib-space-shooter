#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "course.h"
#include "student.h"
#include "ui.h"

// Terminal'den course bilgisi al
Course* get_course_from_terminal(CourseList *list) {
    char name[MAX_NAME_LEN];
    char code[MAX_CODE_LEN];
    int credits;
    char prereq_input[200];
    
    printf("\n=== Add New Course ===\n");
    
    printf("Course Name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;  // Newline'ı kaldır
    
    printf("Course Code: ");
    fgets(code, sizeof(code), stdin);
    code[strcspn(code, "\n")] = 0;
    
    printf("Credits: ");
    scanf("%d", &credits);
    getchar();  // Newline'ı tüket
    
    // Course oluştur - dynamic memory allocation
    Course *new_course = course_create(name, code, credits);
    
    printf("Prerequisites (comma separated codes, or press Enter for none): ");
    fgets(prereq_input, sizeof(prereq_input), stdin);
    prereq_input[strcspn(prereq_input, "\n")] = 0;
    
    // Prerequisites'leri parse et
    if (strlen(prereq_input) > 0) {
        char *token = strtok(prereq_input, ",");
        while (token) {
            // Whitespace'i trim et
            while (*token == ' ') token++;
            char *end = token + strlen(token) - 1;
            while (end > token && *end == ' ') end--;
            *(end + 1) = '\0';
            
            // Prerequisite course'u bul
            Course *prereq = courselist_find_by_code(list, token);
            if (prereq) {
                course_add_prerequisite(new_course, prereq);
                printf("  ✓ Added prerequisite: %s\n", prereq->code);
            } else {
                printf("  ✗ Warning: Course '%s' not found\n", token);
            }
            
            token = strtok(NULL, ",");
        }
    }
    
    return new_course;
}

// Terminal menüsü
void print_menu(void) {
    printf("\n╔════════════════════════════════════════╗\n");
    printf("║      COURSE PLANNER - Main Menu        ║\n");
    printf("╠════════════════════════════════════════╣\n");
    printf("║ 1. Add Course (Terminal)               ║\n");
    printf("║ 2. List All Courses                    ║\n");
    printf("║ 3. Find Course by Code                 ║\n");
    printf("║ 4. Show Memory Addresses (Debug)       ║\n");
    printf("║ 5. Open GUI (GTK)                      ║\n");
    printf("║ 6. Exit                                ║\n");
    printf("╚════════════════════════════════════════╝\n");
    printf("Choice: ");
}

// Memory address'leri göster - pointer debugging
void show_memory_addresses(CourseList *list) {
    printf("\n=== MEMORY ADDRESS DEBUGGING ===\n");
    printf("CourseList address: %p\n", (void*)list);
    printf("Head pointer: %p\n", (void*)list->head);
    printf("Count: %d\n\n", list->count);
    
    CourseNode *current = list->head;
    int index = 0;
    
    while (current) {
        Course *course = current->course_data;
        
        printf("--- Course #%d ---\n", index++);
        printf("Node address: %p\n", (void*)current);
        printf("Course struct address: %p\n", (void*)course);
        printf("Name pointer: %p -> \"%s\"\n", (void*)course->name, course->name);
        printf("Code pointer: %p -> \"%s\"\n", (void*)course->code, course->code);
        printf("Prerequisites array: %p\n", (void*)course->prerequisites);
        
        if (course->prereq_count > 0) {
            printf("Prerequisites:\n");
            for (int i = 0; i < course->prereq_count; i++) {
                Course *prereq = course->prerequisites[i];
                printf("  [%d] %p -> %s\n", i, (void*)prereq, prereq->code);
            }
        }
        
        printf("Next node: %p\n\n", (void*)current->next);
        current = current->next;
    }
}

// Örnek dersler ekle
void add_sample_courses(CourseList *list) {
    printf("\n📚 Adding sample courses...\n");
    
    // 1. Dönem dersleri
    Course *cs101 = course_create("Introduction to Programming", "CS101", 4);
    Course *math101 = course_create("Calculus I", "MATH101", 4);
    Course *phys101 = course_create("Physics I", "PHYS101", 3);
    
    courselist_add(list, cs101);
    courselist_add(list, math101);
    courselist_add(list, phys101);
    
    // 2. Dönem dersleri (prerequisites var)
    Course *cs102 = course_create("Data Structures", "CS102", 4);
    course_add_prerequisite(cs102, cs101);  // CS101 prerequisite
    courselist_add(list, cs102);
    
    Course *math102 = course_create("Calculus II", "MATH102", 4);
    course_add_prerequisite(math102, math101);
    courselist_add(list, math102);
    
    // 3. Dönem dersleri (multiple prerequisites)
    Course *cs201 = course_create("Algorithms", "CS201", 4);
    course_add_prerequisite(cs201, cs102);  // CS102 prerequisite
    course_add_prerequisite(cs201, math101); // MATH101 prerequisite
    courselist_add(list, cs201);
    
    Course *cs202 = course_create("Database Systems", "CS202", 3);
    course_add_prerequisite(cs202, cs102);
    courselist_add(list, cs202);
    
    // 4. Dönem dersleri
    Course *cs301 = course_create("Operating Systems", "CS301", 4);
    course_add_prerequisite(cs301, cs201);
    course_add_prerequisite(cs301, cs102);
    courselist_add(list, cs301);
    
    Course *cs302 = course_create("Computer Networks", "CS302", 3);
    course_add_prerequisite(cs302, cs202);
    courselist_add(list, cs302);
    
    // 5. Dönem - Advanced
    Course *cs401 = course_create("Machine Learning", "CS401", 4);
    course_add_prerequisite(cs401, cs201);
    course_add_prerequisite(cs401, math102);
    courselist_add(list, cs401);
    
    printf("✓ Sample courses added successfully!\n");
    printf("Total courses: %d\n", list->count);
}

// Union kullanım örneği
void demonstrate_union(void) {
    printf("\n=== UNION DEMONSTRATION ===\n");
    
    CourseProperty prop;
    
    // Union'ın aynı memory alanını paylaştığını göster
    printf("Union size: %zu bytes\n", sizeof(CourseProperty));
    printf("Union address: %p\n\n", (void*)&prop);
    
    // int olarak kullan
    prop.int_value = 42;
    printf("As int: %d (address: %p)\n", prop.int_value, (void*)&prop.int_value);
    
    // float olarak kullan (aynı memory'yi override eder)
    prop.float_value = 3.14f;
    printf("As float: %.2f (address: %p)\n", prop.float_value, (void*)&prop.float_value);
    printf("Note: int value is now corrupted: %d\n", prop.int_value);
    
    // string pointer olarak kullan
    prop.string_value = "Hello, Union!";
    printf("As string: %s (address: %p)\n", prop.string_value, (void*)&prop.string_value);
    printf("Note: float value is now corrupted: %.2f\n\n", prop.float_value);
}

// Typedef kullanım örneği
void demonstrate_typedef(void) {
    printf("\n=== TYPEDEF DEMONSTRATION ===\n");
    
    // Typedef sayesinde "struct Course" yerine sadece "Course" yazabiliriz
    Course *course = course_create("Example", "EX101", 3);
    
    printf("Without typedef, we would write:\n");
    printf("  struct Course *course;\n\n");
    
    printf("With typedef, we write:\n");
    printf("  Course *course;\n\n");
    
    printf("Course address: %p\n", (void*)course);
    printf("Course type size: %zu bytes\n", sizeof(Course));
    
    course_destroy(course);
}

int main(int argc, char *argv[]) {
    printf("╔══════════════════════════════════════════════╗\n");
    printf("║   COURSE PLANNER - C Programming Project    ║\n");
    printf("║   Features: Structures, Pointers, Memory    ║\n");
    printf("║   GTK UI with Bezier Curves                 ║\n");
    printf("╚══════════════════════════════════════════════╝\n");
    
    // CourseList oluştur - dynamic memory
    CourseList *course_list = courselist_create();
    
    // Demonstrasyonlar
    demonstrate_typedef();
    demonstrate_union();
    
    // Örnek dersler ekle
    add_sample_courses(course_list);
    
    int choice;
    int running = 1;
    
    while (running) {
        print_menu();
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            while (getchar() != '\n');  // Buffer'ı temizle
            continue;
        }
        getchar();  // Newline'ı tüket
        
        switch (choice) {
            case 1: {
                Course *new_course = get_course_from_terminal(course_list);
                courselist_add(course_list, new_course);
                printf("\n✓ Course added successfully!\n");
                course_print(new_course);
                break;
            }
            
            case 2: {
                courselist_print_all(course_list);
                break;
            }
            
            case 3: {
                char code[MAX_CODE_LEN];
                printf("\nEnter course code: ");
                fgets(code, sizeof(code), stdin);
                code[strcspn(code, "\n")] = 0;
                
                Course *found = courselist_find_by_code(course_list, code);
                if (found) {
                    course_print(found);
                } else {
                    printf("Course not found!\n");
                }
                break;
            }
            
            case 4: {
                show_memory_addresses(course_list);
                break;
            }
            
            case 5: {
                printf("\n🚀 Launching GTK UI...\n");
                UIContext *ui_ctx = ui_create(course_list);
                ui_run(ui_ctx);
                ui_destroy(ui_ctx);
                break;
            }
            
            case 6: {
                printf("\nExiting...\n");
                running = 0;
                break;
            }
            
            default:
                printf("Invalid choice!\n");
        }
    }
    
    // Memory'yi temizle
    printf("\n🧹 Cleaning up memory...\n");
    courselist_destroy(course_list);
    
    printf("Goodbye! 👋\n");
    return 0;
}