#ifndef UI_H
#define UI_H

#include <raylib.h>
#include "course.h"

// Bezier curve için point yapısı
typedef struct {
    float x;
    float y;
} Point;

// UI Input state
typedef struct {
    char name[100];
    char code[20];
    char credits[10];
    char prereq[200];
    int active_field;  // 0=name, 1=code, 2=credits, 3=prereq
    bool is_adding_course;
} InputState;

// UI Context - application state'i tutan structure
typedef struct {
    CourseList *courses;
    int width;
    int height;
    InputState input;
    
    // Camera/scroll için
    Vector2 camera_offset;
    float zoom;
    
    // Selected course için
    Course *selected_course;
    Course *hovered_course;
} UIContext;

// Function prototypes
UIContext* ui_create(CourseList *courses);
void ui_run(UIContext *ctx);
void ui_destroy(UIContext *ctx);

// Raylib drawing functions
void draw_ui(UIContext *ctx);
void draw_toolbar(UIContext *ctx);
void draw_add_course_dialog(UIContext *ctx);
void handle_input(UIContext *ctx);

// Bezier curve functions
void draw_bezier_curve(Point p0, Point p1, Point p2, Point p3, Color color);
void calculate_bezier_control_points(Point start, Point end, Point *c1, Point *c2);
void draw_course_connection(Course *from, Course *to, Vector2 offset);
void draw_arrow_head(Point end, Point prev, Color color);

// Drawing helpers
void draw_course_box(Course *course, Vector2 offset, bool is_hovered, bool is_selected);
void draw_all_courses(UIContext *ctx);
void draw_all_connections(UIContext *ctx);
void draw_grid(int width, int height, Vector2 offset);

// Mouse interaction
Course* get_course_at_mouse(CourseList *courses, Vector2 mouse_pos, Vector2 offset);
bool is_point_in_box(Vector2 point, int x, int y, int width, int height);

#endif // UI_H