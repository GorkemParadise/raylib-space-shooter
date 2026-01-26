#include "ui.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BOX_WIDTH 180
#define BOX_HEIGHT 90
#define ARROW_SIZE 12

// Bezier curve noktasını hesapla (t: 0.0 - 1.0)
Vector2 calculate_bezier_point(Point p0, Point p1, Point p2, Point p3, float t) {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;
    
    Vector2 result;
    result.x = uuu * p0.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * p3.x;
    result.y = uuu * p0.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * p3.y;
    
    return result;
}

// Bezier curve çiz - smooth bağlantılar için
void draw_bezier_curve(Point p0, Point p1, Point p2, Point p3, Color color) {
    int segments = 30;  // Smooth curve için segment sayısı
    
    for (int i = 0; i < segments; i++) {
        float t1 = (float)i / segments;
        float t2 = (float)(i + 1) / segments;
        
        Vector2 point1 = calculate_bezier_point(p0, p1, p2, p3, t1);
        Vector2 point2 = calculate_bezier_point(p0, p1, p2, p3, t2);
        
        DrawLineEx(point1, point2, 2.5f, color);
    }
}

// Bezier kontrol noktalarını hesapla
void calculate_bezier_control_points(Point start, Point end, Point *c1, Point *c2) {
    float dx = end.x - start.x;
    float dy = end.y - start.y;
    
    // Horizontal akış için kontrol noktaları
    c1->x = start.x + dx * 0.5f;
    c1->y = start.y;
    
    c2->x = start.x + dx * 0.5f;
    c2->y = end.y;
}

// Ok başı çiz
void draw_arrow_head(Point end, Point prev, Color color) {
    float angle = atan2f(end.y - prev.y, end.x - prev.x);
    
    Vector2 p1 = {
        end.x - ARROW_SIZE * cosf(angle - PI / 6),
        end.y - ARROW_SIZE * sinf(angle - PI / 6)
    };
    Vector2 p2 = {
        end.x - ARROW_SIZE * cosf(angle + PI / 6),
        end.y - ARROW_SIZE * sinf(angle + PI / 6)
    };
    Vector2 tip = {end.x, end.y};
    
    DrawTriangle(p1, tip, p2, color);
}

// Course bağlantısını çiz (Bezier curve ile)
void draw_course_connection(Course *from, Course *to, Vector2 offset) {
    if (!from || !to) return;
    
    // Başlangıç ve bitiş noktaları
    Point start = {
        from->x + BOX_WIDTH / 2 + offset.x,
        from->y + BOX_HEIGHT / 2 + offset.y
    };
    Point end = {
        to->x + BOX_WIDTH / 2 + offset.x,
        to->y + BOX_HEIGHT / 2 + offset.y
    };
    
    // Kontrol noktaları
    Point c1, c2;
    calculate_bezier_control_points(start, end, &c1, &c2);
    
    // Bezier curve çiz
    Color line_color = (Color){52, 152, 219, 200};  // Mavi
    draw_bezier_curve(start, c1, c2, end, line_color);
    
    // Ok başı için son segment'i hesapla
    Vector2 prev_point = calculate_bezier_point(start, c1, c2, end, 0.95f);
    draw_arrow_head(end, (Point){prev_point.x, prev_point.y}, line_color);
}

// Course box çiz
void draw_course_box(Course *course, Vector2 offset, bool is_hovered, bool is_selected) {
    if (!course) return;
    
    int x = course->x + offset.x;
    int y = course->y + offset.y;
    
    // Gradient effect için iki rectangle
    Color bg_color1 = is_selected ? (Color){230, 126, 34, 255} : 
                      is_hovered ? (Color){52, 152, 219, 255} : 
                      (Color){52, 73, 94, 255};
    Color bg_color2 = is_selected ? (Color){211, 84, 0, 255} : 
                      is_hovered ? (Color){41, 128, 185, 255} : 
                      (Color){44, 62, 80, 255};
    
    // Main box
    DrawRectangleRounded((Rectangle){x, y, BOX_WIDTH, BOX_HEIGHT}, 0.15f, 10, bg_color1);
    DrawRectangleRounded((Rectangle){x, y + BOX_HEIGHT/2, BOX_WIDTH, BOX_HEIGHT/2}, 
                         0.15f, 10, bg_color2);
    
    // Border
    DrawRectangleRoundedLines((Rectangle){x, y, BOX_WIDTH, BOX_HEIGHT}, 
                              0.15f, 10, 3, 
                              is_selected ? ORANGE : WHITE);
    
    // Text
    // Course Code
    DrawText(course->code, x + 10, y + 15, 14, WHITE);
    
    // Course Name (truncated)
    char short_name[30];
    strncpy(short_name, course->name, 22);
    short_name[22] = '\0';
    if (strlen(course->name) > 22) {
        strcat(short_name, "...");
    }
    DrawText(short_name, x + 10, y + 38, 11, (Color){220, 220, 220, 255});
    
    // Credits
    char credits_text[20];
    snprintf(credits_text, sizeof(credits_text), "%d credits", course->credits);
    DrawText(credits_text, x + 10, y + 60, 10, (Color){189, 195, 199, 255});
    
    // Prerequisite count indicator
    if (course->prereq_count > 0) {
        DrawCircle(x + BOX_WIDTH - 15, y + 15, 8, RED);
        char count_text[5];
        snprintf(count_text, sizeof(count_text), "%d", course->prereq_count);
        DrawText(count_text, x + BOX_WIDTH - 18, y + 10, 12, WHITE);
    }
}

// Grid çiz
void draw_grid(int width, int height, Vector2 offset) {
    Color grid_color = (Color){220, 220, 230, 50};
    
    // Vertical lines
    for (int i = 0; i < width + 1000; i += 50) {
        int x = i + ((int)offset.x % 50);
        DrawLine(x, 0, x, height, grid_color);
    }
    
    // Horizontal lines
    for (int i = 0; i < height + 1000; i += 50) {
        int y = i + ((int)offset.y % 50);
        DrawLine(0, y, width, y, grid_color);
    }
}

// Tüm bağlantıları çiz
void draw_all_connections(UIContext *ctx) {
    if (!ctx->courses) return;
    
    CourseNode *current = ctx->courses->head;
    while (current) {
        Course *course = current->course_data;
        
        // Her course'un prerequisite'lerini çiz
        for (int i = 0; i < course->prereq_count; i++) {
            Course *prereq = course->prerequisites[i];
            draw_course_connection(prereq, course, ctx->camera_offset);
        }
        
        current = current->next;
    }
}

// Tüm course'ları çiz
void draw_all_courses(UIContext *ctx) {
    if (!ctx->courses) return;
    
    CourseNode *current = ctx->courses->head;
    while (current) {
        bool is_hovered = (current->course_data == ctx->hovered_course);
        bool is_selected = (current->course_data == ctx->selected_course);
        draw_course_box(current->course_data, ctx->camera_offset, is_hovered, is_selected);
        current = current->next;
    }
}

// Mouse pozisyonundaki course'u bul
Course* get_course_at_mouse(CourseList *courses, Vector2 mouse_pos, Vector2 offset) {
    if (!courses) return NULL;
    
    CourseNode *current = courses->head;
    while (current) {
        Course *course = current->course_data;
        int x = course->x + offset.x;
        int y = course->y + offset.y;
        
        if (mouse_pos.x >= x && mouse_pos.x <= x + BOX_WIDTH &&
            mouse_pos.y >= y && mouse_pos.y <= y + BOX_HEIGHT) {
            return course;
        }
        
        current = current->next;
    }
    
    return NULL;
}

// Point in box kontrolü
bool is_point_in_box(Vector2 point, int x, int y, int width, int height) {
    return point.x >= x && point.x <= x + width &&
           point.y >= y && point.y <= y + height;
}

// Toolbar çiz
void draw_toolbar(UIContext *ctx) {
    // Toolbar background
    DrawRectangle(0, 0, ctx->width, 50, (Color){44, 62, 80, 255});
    DrawRectangle(0, 50, ctx->width, 2, (Color){52, 152, 219, 255});
    
    // Add Course button
    Rectangle add_btn = {10, 10, 120, 30};
    Color btn_color = CheckCollisionPointRec(GetMousePosition(), add_btn) ? 
                      (Color){52, 152, 219, 255} : (Color){41, 128, 185, 255};
    
    DrawRectangleRounded(add_btn, 0.3f, 10, btn_color);
    DrawText("+ Add Course", 20, 18, 14, WHITE);
    
    // Info text
    DrawText("Bezier curves show prerequisites", 150, 20, 12, 
             (Color){189, 195, 199, 255});
    
    // Course count
    char count_text[50];
    snprintf(count_text, sizeof(count_text), "Courses: %d", ctx->courses->count);
    DrawText(count_text, ctx->width - 120, 20, 12, (Color){189, 195, 199, 255});
    
    // Controls info
    DrawText("Mouse Wheel: Zoom | Right Click: Pan", 400, 20, 11, 
             (Color){149, 165, 166, 255});
}

// Add course dialog çiz
void draw_add_course_dialog(UIContext *ctx) {
    // Overlay
    DrawRectangle(0, 0, ctx->width, ctx->height, (Color){0, 0, 0, 180});
    
    // Dialog box
    int dialog_width = 500;
    int dialog_height = 400;
    int dialog_x = (ctx->width - dialog_width) / 2;
    int dialog_y = (ctx->height - dialog_height) / 2;
    
    DrawRectangleRounded(
        (Rectangle){dialog_x, dialog_y, dialog_width, dialog_height},
        0.05f, 10, (Color){236, 240, 241, 255}
    );
    
    // Title
    DrawText("Add New Course", dialog_x + 20, dialog_y + 20, 20, (Color){44, 62, 80, 255});
    DrawLine(dialog_x, dialog_y + 55, dialog_x + dialog_width, dialog_y + 55, 
             (Color){189, 195, 199, 255});
    
    // Labels ve input fields
    int label_x = dialog_x + 20;
    int input_x = dialog_x + 150;
    int y_offset = dialog_y + 80;
    int field_spacing = 70;
    
    const char *labels[] = {"Course Name:", "Course Code:", "Credits:", "Prerequisites:"};
    char *inputs[] = {ctx->input.name, ctx->input.code, ctx->input.credits, ctx->input.prereq};
    
    for (int i = 0; i < 4; i++) {
        int current_y = y_offset + i * field_spacing;
        
        // Label
        DrawText(labels[i], label_x, current_y, 14, (Color){52, 73, 94, 255});
        
        // Input box
        Rectangle input_box = {input_x, current_y - 5, 300, 30};
        Color box_color = (ctx->input.active_field == i) ? 
                         (Color){52, 152, 219, 255} : (Color){189, 195, 199, 255});
        
        DrawRectangleRoundedLines(input_box, 0.2f, 10, 2, box_color);
        DrawRectangleRounded(input_box, 0.2f, 10, (Color){255, 255, 255, 255});
        
        // Input text
        DrawText(inputs[i], input_x + 10, current_y, 14, (Color){44, 62, 80, 255});
        
        // Cursor
        if (ctx->input.active_field == i && ((GetTime() * 2) - (int)(GetTime() * 2)) < 0.5) {
            int cursor_x = input_x + 10 + MeasureText(inputs[i], 14);
            DrawLine(cursor_x, current_y, cursor_x, current_y + 18, (Color){52, 73, 94, 255});
        }
    }
    
    // Hint text
    DrawText("(Separate prerequisites with commas, e.g., CS101,MATH101)", 
             label_x, y_offset + 3 * field_spacing + 35, 10, 
             (Color){127, 140, 141, 255});
    
    // Buttons
    int btn_y = dialog_y + dialog_height - 60;
    Rectangle cancel_btn = {dialog_x + dialog_width - 220, btn_y, 90, 35};
    Rectangle add_btn = {dialog_x + dialog_width - 115, btn_y, 90, 35};
    
    // Cancel button
    Color cancel_color = CheckCollisionPointRec(GetMousePosition(), cancel_btn) ?
                        (Color){231, 76, 60, 255} : (Color){192, 57, 43, 255});
    DrawRectangleRounded(cancel_btn, 0.3f, 10, cancel_color);
    DrawText("Cancel", cancel_btn.x + 20, cancel_btn.y + 10, 14, WHITE);
    
    // Add button
    Color add_color = CheckCollisionPointRec(GetMousePosition(), add_btn) ?
                     (Color){46, 204, 113, 255} : (Color){39, 174, 96, 255});
    DrawRectangleRounded(add_btn, 0.3f, 10, add_color);
    DrawText("Add", add_btn.x + 28, add_btn.y + 10, 14, WHITE);
}

// Input handling
void handle_input(UIContext *ctx) {
    if (!ctx->input.is_adding_course) return;
    
    // Get active input field
    char *active_input = NULL;
    int max_len = 0;
    
    switch (ctx->input.active_field) {
        case 0: active_input = ctx->input.name; max_len = 99; break;
        case 1: active_input = ctx->input.code; max_len = 19; break;
        case 2: active_input = ctx->input.credits; max_len = 9; break;
        case 3: active_input = ctx->input.prereq; max_len = 199; break;
    }
    
    if (active_input) {
        int len = strlen(active_input);
        
        // Handle character input
        int key = GetCharPressed();
        while (key > 0) {
            if (len < max_len && key >= 32 && key <= 125) {
                active_input[len] = (char)key;
                active_input[len + 1] = '\0';
            }
            key = GetCharPressed();
        }
        
        // Handle backspace
        if (IsKeyPressed(KEY_BACKSPACE) && len > 0) {
            active_input[len - 1] = '\0';
        }
        
        // Tab to next field
        if (IsKeyPressed(KEY_TAB)) {
            ctx->input.active_field = (ctx->input.active_field + 1) % 4;
        }
    }
}

// UI Context oluştur
UIContext* ui_create(CourseList *courses) {
    UIContext *ctx = (UIContext*)safe_malloc(sizeof(UIContext));
    ctx->courses = courses;
    ctx->width = 1200;
    ctx->height = 800;
    ctx->camera_offset = (Vector2){0, 0};
    ctx->zoom = 1.0f;
    ctx->selected_course = NULL;
    ctx->hovered_course = NULL;
    
    // Input state
    memset(&ctx->input, 0, sizeof(InputState));
    ctx->input.active_field = 0;
    ctx->input.is_adding_course = false;
    
    return ctx;
}

// UI çalıştır
void ui_run(UIContext *ctx) {
    InitWindow(ctx->width, ctx->height, "Course Planner - Raylib + Bezier Curves");
    SetTargetFPS(60);
    
    Vector2 drag_start = {0, 0};
    bool is_dragging = false;
    
    while (!WindowShouldClose()) {
        // Input handling
        Vector2 mouse_pos = GetMousePosition();
        
        // Dialog input
        if (ctx->input.is_adding_course) {
            handle_input(ctx);
            
            int dialog_width = 500;
            int dialog_height = 400;
            int dialog_x = (ctx->width - dialog_width) / 2;
            int dialog_y = (ctx->height - dialog_height) / 2;
            
            // Click input fields
            for (int i = 0; i < 4; i++) {
                Rectangle input_box = {
                    dialog_x + 150, 
                    dialog_y + 75 + i * 70, 
                    300, 30
                };
                if (CheckCollisionPointRec(mouse_pos, input_box) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    ctx->input.active_field = i;
                }
            }
            
            // Buttons
            Rectangle cancel_btn = {dialog_x + dialog_width - 220, dialog_y + dialog_height - 60, 90, 35};
            Rectangle add_btn = {dialog_x + dialog_width - 115, dialog_y + dialog_height - 60, 90, 35};
            
            if (CheckCollisionPointRec(mouse_pos, cancel_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                ctx->input.is_adding_course = false;
                memset(&ctx->input, 0, sizeof(InputState));
            }
            
            if (CheckCollisionPointRec(mouse_pos, add_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                // Add course
                if (strlen(ctx->input.name) > 0 && strlen(ctx->input.code) > 0) {
                    int credits = atoi(ctx->input.credits);
                    if (credits <= 0) credits = 3;
                    
                    Course *new_course = course_create(ctx->input.name, ctx->input.code, credits);
                    
                    // Parse prerequisites
                    if (strlen(ctx->input.prereq) > 0) {
                        char prereq_copy[200];
                        strcpy(prereq_copy, ctx->input.prereq);
                        char *token = strtok(prereq_copy, ",");
                        
                        while (token) {
                            while (*token == ' ') token++;
                            char *end = token + strlen(token) - 1;
                            while (end > token && *end == ' ') end--;
                            *(end + 1) = '\0';
                            
                            Course *prereq = courselist_find_by_code(ctx->courses, token);
                            if (prereq) {
                                course_add_prerequisite(new_course, prereq);
                            }
                            
                            token = strtok(NULL, ",");
                        }
                    }
                    
                    courselist_add(ctx->courses, new_course);
                    printf("Added: %s (%s)\n", new_course->name, new_course->code);
                }
                
                ctx->input.is_adding_course = false;
                memset(&ctx->input, 0, sizeof(InputState));
            }
        } else {
            // Add course button
            Rectangle add_btn = {10, 10, 120, 30};
            if (CheckCollisionPointRec(mouse_pos, add_btn) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                ctx->input.is_adding_course = true;
            }
            
            // Camera panning
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                drag_start = mouse_pos;
                is_dragging = true;
            }
            if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
                is_dragging = false;
            }
            if (is_dragging) {
                Vector2 delta = Vector2Subtract(mouse_pos, drag_start);
                ctx->camera_offset = Vector2Add(ctx->camera_offset, delta);
                drag_start = mouse_pos;
            }
            
            // Mouse hover
            ctx->hovered_course = get_course_at_mouse(ctx->courses, mouse_pos, ctx->camera_offset);
            
            // Select course
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && mouse_pos.y > 50) {
                ctx->selected_course = ctx->hovered_course;
                if (ctx->selected_course) {
                    printf("\n=== Selected Course ===\n");
                    course_print(ctx->selected_course);
                }
            }
        }
        
        // Drawing
        BeginDrawing();
        ClearBackground((Color){236, 240, 241, 255});
        
        if (!ctx->input.is_adding_course) {
            // Draw grid
            draw_grid(ctx->width, ctx->height, ctx->camera_offset);
            
            // Draw connections first (behind)
            draw_all_connections(ctx);
            
            // Draw courses
            draw_all_courses(ctx);
            
            // Draw toolbar (always on top)
            draw_toolbar(ctx);
        } else {
            // Draw dialog
            draw_add_course_dialog(ctx);
        }
        
        // FPS
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, ctx->height - 25, 12, DARKGRAY);
        
        EndDrawing();
    }
    
    CloseWindow();
}

// UI temizle
void ui_destroy(UIContext *ctx) {
    if (ctx) {
        free(ctx);
    }
}