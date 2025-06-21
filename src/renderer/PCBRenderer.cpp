#include "PCBRenderer.h"
#include "Utils.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include <imgui.h>

// Simple vertex shader
const char* vertex_shader_source = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 projection;
out vec3 vertexColor;

void main() {
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
    vertexColor = aColor;
}
)";

// Simple fragment shader
const char* fragment_shader_source = R"(
#version 330 core
in vec3 vertexColor;
out vec4 FragColor;

uniform float alpha;

void main() {
    FragColor = vec4(vertexColor, alpha);
}
)";

PCBRenderer::PCBRenderer() {
}

PCBRenderer::~PCBRenderer() {
    Cleanup();
}

bool PCBRenderer::Initialize() {
    // Create shader program
    if (!CreateShaderProgram()) {
        LOG_ERROR("Failed to create shader program");
        return false;
    }

    // Create VAO and VBO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    LOG_INFO("PCB Renderer initialized successfully");
    return true;
}

void PCBRenderer::Cleanup() {
    if (vbo) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    }
    if (vao) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    }
    if (shader_program) {
        glDeleteProgram(shader_program);
        shader_program = 0;
    }
}

void PCBRenderer::SetPCBData(std::shared_ptr<BRDFileBase> data) {
    pcb_data = data;
    
    if (pcb_data && pcb_data->IsValid()) {
        LOG_INFO("PCB data set: " + std::to_string(pcb_data->parts.size()) + 
                " parts, " + std::to_string(pcb_data->pins.size()) + " pins");
    }
}

void PCBRenderer::Render(int window_width, int window_height) {
    if (!pcb_data || !pcb_data->IsValid()) {
        LOG_INFO("No PCB data to render");
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        return;
    }

    // Clear screen with PCB green background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Initialize camera if needed (first time rendering)
    static bool camera_initialized = false;
    if (!camera_initialized) {
        ZoomToFit(window_width, window_height);
        camera_initialized = true;
    }
    
    // Calculate screen transform from camera
    float zoom = camera.zoom;
    float offset_x = window_width * 0.5f - camera.x * zoom;
    float offset_y = window_height * 0.5f + camera.y * zoom;  // Mirror Y-axis
    
    // Apply camera transformation

    // Create a fullscreen ImGui window for PCB rendering
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(window_width, window_height));
    
    bool window_open = ImGui::Begin("PCB View", nullptr, 
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                 ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                 ImGuiWindowFlags_NoBackground);

    if (!window_open) {
        LOG_ERROR("Failed to create ImGui window");
        ImGui::End();
        return;
    }

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    if (!draw_list) {
        LOG_ERROR("Failed to get ImGui draw list");
        ImGui::End();
        return;
    }    // Use structured ImGui rendering methods (like original OpenBoardView)
    RenderOutlineImGui(draw_list, zoom, offset_x, offset_y);
    RenderPartsImGui(draw_list, zoom, offset_x, offset_y);
    RenderPinsImGui(draw_list, zoom, offset_x, offset_y);

    ImGui::End();
}

void PCBRenderer::SetCamera(float x, float y, float zoom) {
    camera.x = x;
    camera.y = y;
    camera.zoom = zoom;
}

void PCBRenderer::Pan(float dx, float dy) {
    camera.x += dx / camera.zoom;
    camera.y += dy / camera.zoom;
}

void PCBRenderer::Zoom(float factor, float center_x, float center_y) {
    float old_zoom = camera.zoom;
    
    // Apply zoom factor
    camera.zoom *= factor;
    
    // Limit zoom range
    if (camera.zoom < 0.01f) camera.zoom = 0.01f;
    if (camera.zoom > 100.0f) camera.zoom = 100.0f;
    
    // If a center point is specified, adjust camera so that world point stays in same screen position
    if (center_x != 0.0f || center_y != 0.0f) {
        // The idea: after zooming, we want center_x,center_y to appear at the same screen position
        // Before zoom: screen_pos = (world_point - camera_old) * zoom_old
        // After zoom: screen_pos = (world_point - camera_new) * zoom_new
        // Since screen_pos should be the same: (center - old_camera) * old_zoom = (center - new_camera) * new_zoom
        // Solving for new_camera: new_camera = center - (center - old_camera) * old_zoom / new_zoom
        
        float old_x = camera.x;
        float old_y = camera.y;
        
        camera.x = center_x - (center_x - old_x) * old_zoom / camera.zoom;
        camera.y = center_y - (center_y - old_y) * old_zoom / camera.zoom;
    }
}

void PCBRenderer::ZoomToFit(int window_width, int window_height) {
    if (!pcb_data) return;
    
    BRDPoint min_point, max_point;
    pcb_data->GetBoundingBox(min_point, max_point);
    
    float pcb_width = static_cast<float>(max_point.x - min_point.x);
    float pcb_height = static_cast<float>(max_point.y - min_point.y);
    
    if (pcb_width <= 0 || pcb_height <= 0) return;
    
    // Calculate zoom to fit with margin
    float zoom_x = window_width / (pcb_width * 1.2f);
    float zoom_y = window_height / (pcb_height * 1.2f);
    camera.zoom = std::min(zoom_x, zoom_y);
    
    // Center the view
    camera.x = (min_point.x + max_point.x) * 0.5f;
    camera.y = (min_point.y + max_point.y) * 0.5f;
}

bool PCBRenderer::CreateShaderProgram() {
    // Compile shaders
    GLuint vertex_shader = CompileShader(vertex_shader_source, GL_VERTEX_SHADER);
    if (!vertex_shader) return false;
    
    GLuint fragment_shader = CompileShader(fragment_shader_source, GL_FRAGMENT_SHADER);
    if (!fragment_shader) {
        glDeleteShader(vertex_shader);
        return false;
    }

    // Create program
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    // Check for linking errors
    GLint success;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(shader_program, 512, nullptr, info_log);
        LOG_ERROR("Shader program linking failed: " + std::string(info_log));
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return false;
    }

    // Delete shaders (they're linked into the program now)
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return true;
}

GLuint PCBRenderer::CompileShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        LOG_ERROR("Shader compilation failed: " + std::string(info_log));
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

void PCBRenderer::SetProjectionMatrix(int window_width, int window_height) {
    camera.aspect_ratio = static_cast<float>(window_width) / window_height;
    
    // Create orthographic projection matrix
    float left = camera.x - window_width / (2.0f * camera.zoom);
    float right = camera.x + window_width / (2.0f * camera.zoom);
    float bottom = camera.y - window_height / (2.0f * camera.zoom);
    float top = camera.y + window_height / (2.0f * camera.zoom);

    // Simple orthographic projection matrix
    float projection[16] = {
        2.0f / (right - left), 0.0f, 0.0f, -(right + left) / (right - left),
        0.0f, 2.0f / (top - bottom), 0.0f, -(top + bottom) / (top - bottom),
        0.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    GLint projection_loc = glGetUniformLocation(shader_program, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, projection);
}

void PCBRenderer::RenderOutline() {
    if (pcb_data->outline_segments.empty()) {
        return;
    }

    GLint alpha_loc = glGetUniformLocation(shader_program, "alpha");
    glUniform1f(alpha_loc, settings.outline_alpha);

    // Render outline segments
    for (const auto& segment : pcb_data->outline_segments) {
        DrawLine(static_cast<float>(segment.first.x), static_cast<float>(segment.first.y),
                static_cast<float>(segment.second.x), static_cast<float>(segment.second.y),
                settings.outline_color.r, settings.outline_color.g, settings.outline_color.b);
    }
}

void PCBRenderer::RenderParts() {
    GLint alpha_loc = glGetUniformLocation(shader_program, "alpha");
    glUniform1f(alpha_loc, settings.part_alpha);

    // Enhanced part rendering with proper outline detection
    for (size_t i = 0; i < pcb_data->parts.size(); ++i) {
        const auto& part = pcb_data->parts[i];
        
        // Get pins for this part
        std::vector<BRDPin> part_pins;
        for (const auto& pin : pcb_data->pins) {
            if (pin.part == i + 1) { // Parts are 1-indexed
                part_pins.push_back(pin);
            }
        }
        
        if (part_pins.empty()) {
            // Draw a simple rectangle for parts with no pins
            float x = static_cast<float>(part.p1.x);
            float y = static_cast<float>(part.p1.y);
            float width = static_cast<float>(part.p2.x - part.p1.x);
            float height = static_cast<float>(part.p2.y - part.p1.y);
            
            if (width > 0 && height > 0) {
                DrawRect(x, y, width, height, 
                        settings.part_color.r, settings.part_color.g, settings.part_color.b);
            }
            continue;
        }
        
        // Calculate part outline based on pins - enhanced logic
        RenderPartOutline(part, part_pins);
    }
}

void PCBRenderer::RenderPartOutline(const BRDPart& part, const std::vector<BRDPin>& part_pins) {
    if (part_pins.empty()) return;
    
    // Calculate bounding box
    float min_x = part_pins[0].pos.x, max_x = part_pins[0].pos.x;
    float min_y = part_pins[0].pos.y, max_y = part_pins[0].pos.y;
    
    for (const auto& pin : part_pins) {
        min_x = std::min(min_x, static_cast<float>(pin.pos.x));
        max_x = std::max(max_x, static_cast<float>(pin.pos.x));
        min_y = std::min(min_y, static_cast<float>(pin.pos.y));
        max_y = std::max(max_y, static_cast<float>(pin.pos.y));
    }
    
    // Calculate distance and pin spacing for component type detection
    float distance = std::sqrt((max_x - min_x) * (max_x - min_x) + (max_y - min_y) * (max_y - min_y));
    int pin_count = part_pins.size();
    
    // Component type detection and outline generation
    float outline_margin = DeterminePinMargin(part, part_pins, distance);
    
    // Generate outline - always use generic for now, can be enhanced later
    RenderGenericComponentOutline(min_x, min_y, max_x, max_y, outline_margin);
}

float PCBRenderer::DeterminePinMargin(const BRDPart& part, const std::vector<BRDPin>& part_pins, float distance) {
    int pin_count = part_pins.size();
      // Enhanced component type detection based on OpenBoardView logic
    if (pin_count < 4 && !part.name.empty() && part.name[0] != 'U' && part.name[0] != 'Q') {
        // 2-3 pin components - likely passives
        if (distance > 52 && distance < 57) {
            return 8.0f; // 0603
        } else if (distance > 247 && distance < 253) {
            return 25.0f; // SMC diode
        } else if (distance > 195 && distance < 199) {
            return 25.0f; // Inductor
        } else if (distance > 165 && distance < 169) {
            return 18.0f; // SMB diode
        } else if (distance > 101 && distance < 109) {
            return 15.0f; // SMA diode / tant cap
        } else if (distance > 108 && distance < 112) {
            return 15.0f; // 1206
        } else if (distance > 64 && distance < 68) {
            return 13.0f; // 0805
        } else if (distance > 18 && distance < 22) {
            return 3.0f; // 0201
        } else if (distance > 28 && distance < 32) {
            return 5.0f; // 0402
        }
    }
    
    // Default margins for other components
    if (pin_count <= 4) return 10.0f;
    if (pin_count <= 16) return 15.0f;
    if (pin_count <= 32) return 20.0f;
    return 25.0f; // Large ICs
}

void PCBRenderer::RenderGenericComponentOutline(float min_x, float min_y, float max_x, float max_y, float margin) {
    float x = min_x - margin;
    float y = min_y - margin;
    float w = (max_x - min_x) + 2 * margin;
    float h = (max_y - min_y) + 2 * margin;
    
    // Draw filled rectangle with transparency
    std::vector<float> vertices = {
        x, y,     settings.part_color.r, settings.part_color.g, settings.part_color.b,
        x+w, y,   settings.part_color.r, settings.part_color.g, settings.part_color.b,
        x+w, y+h, settings.part_color.r, settings.part_color.g, settings.part_color.b,
        x, y,     settings.part_color.r, settings.part_color.g, settings.part_color.b,
        x+w, y+h, settings.part_color.r, settings.part_color.g, settings.part_color.b,
        x, y+h,   settings.part_color.r, settings.part_color.g, settings.part_color.b
    };
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Draw outline with lines
    DrawLine(x, y, x + w, y, settings.part_color.r, settings.part_color.g, settings.part_color.b);
    DrawLine(x + w, y, x + w, y + h, settings.part_color.r, settings.part_color.g, settings.part_color.b);
    DrawLine(x + w, y + h, x, y + h, settings.part_color.r, settings.part_color.g, settings.part_color.b);
    DrawLine(x, y + h, x, y, settings.part_color.r, settings.part_color.g, settings.part_color.b);
}

void PCBRenderer::RenderPins() {
    GLint alpha_loc = glGetUniformLocation(shader_program, "alpha");
    glUniform1f(alpha_loc, settings.pin_alpha);

    for (const auto& pin : pcb_data->pins) {
        float x = static_cast<float>(pin.pos.x);
        float y = static_cast<float>(pin.pos.y);
        
        // Enhanced pin sizing based on component type
        float radius = static_cast<float>(pin.radius);
        if (radius < 1.0f) {
            // Determine pin size based on part type
            if (pin.part > 0 && pin.part <= pcb_data->parts.size()) {
                const auto& part = pcb_data->parts[pin.part - 1];
                
                // Get all pins for this part to determine spacing
                std::vector<BRDPin> part_pins;
                for (const auto& p : pcb_data->pins) {
                    if (p.part == pin.part) {
                        part_pins.push_back(p);
                    }
                }
                
                radius = DeterminePinSize(part, part_pins);
            } else {
                radius = 7.0f; // Default size
            }
        }
        
        // Draw pin with appropriate color
        float pin_r = settings.pin_color.r;
        float pin_g = settings.pin_color.g;
        float pin_b = settings.pin_color.b;
          // Color coding based on net connectivity
        if (!pin.net.empty() && pin.net != "UNCONNECTED") {
            // Connected pins get full color
            DrawCircle(x, y, radius, pin_r, pin_g, pin_b);
        } else {
            // Unconnected pins get dimmed color
            DrawCircle(x, y, radius, pin_r * 0.5f, pin_g * 0.5f, pin_b * 0.5f);
        }
        
        // Draw pin name if available and zoom level is high enough
        if (!pin.name.empty() && camera.zoom > 0.5f) {
            // Draw a small dot to indicate named pin
            DrawCircle(x, y, radius * 0.3f, 1.0f, 1.0f, 1.0f); // White center
        }
    }
}

float PCBRenderer::DeterminePinSize(const BRDPart& part, const std::vector<BRDPin>& part_pins) {
    if (part_pins.empty()) return 7.0f;
    
    int pin_count = part_pins.size();
    
    // Calculate pin spacing for size determination
    if (pin_count >= 2) {
        float min_x = part_pins[0].pos.x, max_x = part_pins[0].pos.x;
        float min_y = part_pins[0].pos.y, max_y = part_pins[0].pos.y;
        
        for (const auto& pin : part_pins) {
            min_x = std::min(min_x, static_cast<float>(pin.pos.x));
            max_x = std::max(max_x, static_cast<float>(pin.pos.x));
            min_y = std::min(min_y, static_cast<float>(pin.pos.y));
            max_y = std::max(max_y, static_cast<float>(pin.pos.y));
        }
        
        float distance = std::sqrt((max_x - min_x) * (max_x - min_x) + (max_y - min_y) * (max_y - min_y));
          // Pin size determination based on OpenBoardView logic
        if (pin_count < 4 && !part.name.empty() && part.name[0] != 'U' && part.name[0] != 'Q') {
            if (distance > 52 && distance < 57) return 15.0f;  // 0603
            if (distance > 247 && distance < 253) return 50.0f; // SMC diode
            if (distance > 195 && distance < 199) return 50.0f; // Inductor
            if (distance > 165 && distance < 169) return 35.0f; // SMB diode
            if (distance > 101 && distance < 109) return 30.0f; // SMA diode
            if (distance > 108 && distance < 112) return 30.0f; // 1206
            if (distance > 64 && distance < 68) return 25.0f;   // 0805
            if (distance > 18 && distance < 22) return 5.0f;    // 0201
            if (distance > 28 && distance < 32) return 10.0f;   // 0402
        }
    }
    
    // Default sizes based on pin count
    if (pin_count <= 2) return 12.0f;
    if (pin_count <= 4) return 10.0f;
    if (pin_count <= 16) return 8.0f;
    if (pin_count <= 32) return 6.0f;
    return 4.0f; // High pin count ICs
}

void PCBRenderer::DrawLine(float x1, float y1, float x2, float y2, float r, float g, float b, float a) {
    // Create line vertices (position + color)
    float vertices[] = {
        x1, y1, r, g, b,
        x2, y2, r, g, b
    };

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_LINES, 0, 2);
}

void PCBRenderer::DrawRect(float x, float y, float width, float height, float r, float g, float b, float a) {
    // Create rectangle vertices (as two triangles)
    float vertices[] = {
        // Triangle 1
        x, y, r, g, b,
        x + width, y, r, g, b,
        x, y + height, r, g, b,
        // Triangle 2
        x + width, y, r, g, b,
        x + width, y + height, r, g, b,
        x, y + height, r, g, b
    };

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void PCBRenderer::DrawCircle(float x, float y, float radius, float r, float g, float b, float a) {
    const int segments = 16;
    std::vector<float> vertices;
    
    // Center vertex
    vertices.insert(vertices.end(), {x, y, r, g, b});
    
    // Circle vertices
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0f * 3.14159f * i / segments;
        float px = x + radius * std::cos(angle);
        float py = y + radius * std::sin(angle);
        vertices.insert(vertices.end(), {px, py, r, g, b});
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(vertices.size() / 5));
}

void PCBRenderer::RenderOutlineImGui(ImDrawList* draw_list, float zoom, float offset_x, float offset_y) {
    if (!pcb_data || pcb_data->outline_segments.empty()) {
        LOG_INFO("No outline segments to render");
        return;
    }    // Render board outline
    
    // Set outline color - white/gray for PCB outline
    ImU32 outline_color = IM_COL32(255, 255, 255, 255);
    
    // Adaptive line thickness based on zoom level
    float line_thickness = std::max(1.0f, std::min(4.0f, 2.0f / zoom));  // Thicker when zoomed out
    
    for (const auto& segment : pcb_data->outline_segments) {
        // Transform coordinates from PCB space to screen space with Y-axis mirroring
        ImVec2 p1(segment.first.x * zoom + offset_x, offset_y - segment.first.y * zoom);
        ImVec2 p2(segment.second.x * zoom + offset_x, offset_y - segment.second.y * zoom);
        
        // Draw outline segment
        draw_list->AddLine(p1, p2, outline_color, line_thickness);
    }
    
    // Outline rendering complete
}

void PCBRenderer::RenderPartsImGui(ImDrawList* draw_list, float zoom, float offset_x, float offset_y) {
    if (!pcb_data || pcb_data->parts.empty()) {
        LOG_INFO("No parts to render");
        return;
    }

    // Parts rendering
    
    for (size_t i = 0; i < pcb_data->parts.size(); ++i) {
        const auto& part = pcb_data->parts[i];
        
        // Get pins for this part
        std::vector<BRDPin> part_pins;
        for (const auto& pin : pcb_data->pins) {
            if (pin.part == i + 1) { // Parts are 1-indexed
                part_pins.push_back(pin);
            }
        }        if (part_pins.empty()) {
            // Draw a simple rectangle for parts with no pins using part bounds
            float x1 = part.p1.x * zoom + offset_x;
            float y1 = offset_y - part.p1.y * zoom;  // Mirror Y
            float x2 = part.p2.x * zoom + offset_x;
            float y2 = offset_y - part.p2.y * zoom;  // Mirror Y
            
            // Ensure correct rectangle coordinates
            if (y1 > y2) std::swap(y1, y2);  // Fix flipped Y coordinates
            
            // Ensure minimum component size for visibility when zoomed out
            float min_component_size = 4.0f;  // Minimum 4 pixels
            float width = x2 - x1;
            float height = y2 - y1;
            
            if (width < min_component_size && width > 0) {
                float center_x = (x1 + x2) * 0.5f;
                x1 = center_x - min_component_size * 0.5f;
                x2 = center_x + min_component_size * 0.5f;
            }
            
            if (height < min_component_size && height > 0) {
                float center_y = (y1 + y2) * 0.5f;
                y1 = center_y - min_component_size * 0.5f;
                y2 = center_y + min_component_size * 0.5f;
            }
              if (x2 > x1 && y2 > y1) {
                // Use OpenBoardView-style gray color for components
                ImU32 part_fill_color = IM_COL32(105, 105, 105, 255);  // Dim gray
                ImU32 part_outline_color = IM_COL32(169, 169, 169, 255);  // Dark gray outline
                
                // Adaptive outline thickness
                float component_outline_thickness = std::max(1.0f, std::min(3.0f, 2.0f / zoom));
                
                draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), part_fill_color);
                draw_list->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), part_outline_color, 0.0f, 0, component_outline_thickness);                // Add component label - only if text fits completely inside component
                float component_screen_size = std::max(x2 - x1, y2 - y1);
                if (component_screen_size > 30.0f && !part.name.empty()) {
                    ImVec2 text_size = ImGui::CalcTextSize(part.name.c_str());
                    
                    // Check if text fits inside component with margin
                    float text_margin = 4.0f;  // 2px margin on each side
                    if (text_size.x <= (x2 - x1 - text_margin) && text_size.y <= (y2 - y1 - text_margin)) {
                        float text_x = (x1 + x2) * 0.5f;
                        float text_y = (y1 + y2) * 0.5f;
                        
                        ImVec2 text_pos(text_x - text_size.x * 0.5f, text_y - text_size.y * 0.5f);
                        
                        // Clip text rendering to component boundaries
                        draw_list->PushClipRect(ImVec2(x1, y1), ImVec2(x2, y2), true);
                        
                        // Add text background for better readability
                        ImVec2 bg_min = ImVec2(text_pos.x - 1, text_pos.y);
                        ImVec2 bg_max = ImVec2(text_pos.x + text_size.x + 1, text_pos.y + text_size.y);
                        draw_list->AddRectFilled(bg_min, bg_max, IM_COL32(0, 0, 0, 120));
                        
                        draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), part.name.c_str());
                        
                        // Restore clipping
                        draw_list->PopClipRect();
                    }
                }
            }
            continue;
        }
        
        // Calculate bounding box from pins
        float min_x = part_pins[0].pos.x, max_x = part_pins[0].pos.x;
        float min_y = part_pins[0].pos.y, max_y = part_pins[0].pos.y;
        
        for (const auto& pin : part_pins) {
            min_x = std::min(min_x, static_cast<float>(pin.pos.x));
            max_x = std::max(max_x, static_cast<float>(pin.pos.x));
            min_y = std::min(min_y, static_cast<float>(pin.pos.y));
            max_y = std::max(max_y, static_cast<float>(pin.pos.y));
        }
        
        // Add margin around pins to create component body
        float margin = DeterminePinMargin(part, part_pins, 
            std::sqrt((max_x - min_x) * (max_x - min_x) + (max_y - min_y) * (max_y - min_y)));        // Transform to screen coordinates with Y-axis mirroring
        float x1 = (min_x - margin) * zoom + offset_x;
        float y1 = offset_y - (min_y - margin) * zoom;  // Mirror Y
        float x2 = (max_x + margin) * zoom + offset_x;
        float y2 = offset_y - (max_y + margin) * zoom;  // Mirror Y
        
        // Ensure correct rectangle coordinates after Y mirroring
        if (y1 > y2) std::swap(y1, y2);
        
        // Ensure minimum component size for visibility when zoomed out
        float min_component_size = 4.0f;  // Minimum 4 pixels
        float width = x2 - x1;
        float height = y2 - y1;
        
        if (width < min_component_size) {
            float center_x = (x1 + x2) * 0.5f;
            x1 = center_x - min_component_size * 0.5f;
            x2 = center_x + min_component_size * 0.5f;
        }
        
        if (height < min_component_size) {
            float center_y = (y1 + y2) * 0.5f;
            y1 = center_y - min_component_size * 0.5f;
            y2 = center_y + min_component_size * 0.5f;
        }        // Choose component colors with better contrast for zoom-out visibility
        ImU32 part_fill_color, part_outline_color;
        
        // Component type detection based on pin count and name
        int pin_count = part_pins.size();
        char first_char = !part.name.empty() ? part.name[0] : 'X';
        
        // Increase color intensity for better visibility when zoomed out
        float color_boost = (zoom < 0.5f) ? 1.3f : 1.0f;  // Boost colors when zoomed out
        
        if (first_char == 'U' || first_char == 'Q') {
            // ICs - use OpenBoardView-style gray/blue colors with boost
            if (pin_count >= 100) {
                part_fill_color = IM_COL32(90 * color_boost, 90 * color_boost, 90 * color_boost, 0);
                part_outline_color = IM_COL32(120 * color_boost, 120 * color_boost, 120 * color_boost, 255);
            } else if (pin_count >= 20) {
                part_fill_color = IM_COL32(110 * color_boost, 110 * color_boost, 110 * color_boost, 0);
                part_outline_color = IM_COL32(140 * color_boost, 140 * color_boost, 140 * color_boost, 255);
            } else {
                part_fill_color = IM_COL32(130 * color_boost, 130 * color_boost, 130 * color_boost, 0);
                part_outline_color = IM_COL32(160 * color_boost, 160 * color_boost, 160 * color_boost, 255);
            }
        } else if (first_char == 'R') {
            // Resistors - use brown/tan with boost
            part_fill_color = IM_COL32(std::min(255.0f, 139 * color_boost), std::min(255.0f, 69 * color_boost), std::min(255.0f, 19 * color_boost), 0);
            part_outline_color = IM_COL32(std::min(255.0f, 160 * color_boost), std::min(255.0f, 82 * color_boost), std::min(255.0f, 45 * color_boost), 255);
        } else if (first_char == 'C') {
            // Capacitors - use tan/beige with boost
            part_fill_color = IM_COL32(std::min(255.0f, 210 * color_boost), std::min(255.0f, 180 * color_boost), std::min(255.0f, 140 * color_boost), 0);
            part_outline_color = IM_COL32(std::min(255.0f, 139 * color_boost), std::min(255.0f, 119 * color_boost), std::min(255.0f, 101 * color_boost), 255);
        } else if (first_char == 'L') {
            // Inductors - use green with boost
            part_fill_color = IM_COL32(std::min(255.0f, 34 * color_boost), std::min(255.0f, 139 * color_boost), std::min(255.0f, 34 * color_boost), 0);
            part_outline_color = IM_COL32(0, std::min(255.0f, 100 * color_boost), 0, 255);
        } else if (first_char == 'D') {
            // Diodes - use red with boost
            part_fill_color = IM_COL32(std::min(255.0f, 178 * color_boost), std::min(255.0f, 34 * color_boost), std::min(255.0f, 34 * color_boost), 0);
            part_outline_color = IM_COL32(std::min(255.0f, 139 * color_boost), 0, 0, 255);
        } else {
            // Default components - use neutral gray with boost
            part_fill_color = IM_COL32(105 * color_boost, 105 * color_boost, 105 * color_boost, 0);
            part_outline_color = IM_COL32(169 * color_boost, 169 * color_boost, 169 * color_boost, 255);
        }
          // Draw component body with adaptive outline thickness
        float component_outline_thickness = std::max(1.0f, std::min(3.0f, 2.0f / zoom));  // Thicker when zoomed out
        draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), part_fill_color);
        draw_list->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), part_outline_color, 0.0f, 0, component_outline_thickness);        // Add part name - only if text fits completely inside component
        float component_screen_size = std::max(x2 - x1, y2 - y1);
        if (component_screen_size > 30.0f && !part.name.empty()) {
            ImVec2 text_size = ImGui::CalcTextSize(part.name.c_str());
            
            // Check if text fits inside component with margin
            float text_margin = 4.0f;  // 2px margin on each side
            if (text_size.x <= (x2 - x1 - text_margin) && text_size.y <= (y2 - y1 - text_margin)) {
                float text_x = (min_x + max_x) * 0.5f * zoom + offset_x;
                float text_y = offset_y - (min_y + max_y) * 0.5f * zoom;  // Mirror Y
                
                // Calculate text position for centering
                ImVec2 text_pos(text_x - text_size.x * 0.5f, text_y - text_size.y * 0.5f);
                
                // Clip text rendering to component boundaries
                draw_list->PushClipRect(ImVec2(x1, y1), ImVec2(x2, y2), true);
                
                // Add text background for better readability
                ImVec2 bg_min = ImVec2(text_pos.x - 1, text_pos.y);
                ImVec2 bg_max = ImVec2(text_pos.x + text_size.x + 1, text_pos.y + text_size.y);
                draw_list->AddRectFilled(bg_min, bg_max, IM_COL32(0, 0, 0, 120));
                
                // Use high-contrast white text
                draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), part.name.c_str());
                
                // Restore clipping
                draw_list->PopClipRect();
            }
        }
    }
    
    // Parts rendering complete
}

void PCBRenderer::RenderPinsImGui(ImDrawList* draw_list, float zoom, float offset_x, float offset_y) {
    if (!pcb_data || pcb_data->pins.empty()) {
        LOG_INFO("No pins to render");
        return;
    }    // Pins rendering
    
    // Set pin colors - RED as requested
    ImU32 pin_fill_color = IM_COL32(255, 100, 100, 255);    // Red pin color
    ImU32 pin_outline_color = IM_COL32(220, 70, 70, 255);   // Darker red outline
    
    for (size_t pin_index = 0; pin_index < pcb_data->pins.size(); ++pin_index) {
        const auto& pin = pcb_data->pins[pin_index];
        // Transform pin coordinates to screen space with Y-axis mirroring
        float x = pin.pos.x * zoom + offset_x;
        float y = offset_y - pin.pos.y * zoom;  // Mirror Y        // Calculate pin size based on zoom with minimum size for visibility
        float base_pin_size = 5.0f;  // Base pin size in world units
        float screen_pin_size = base_pin_size * zoom;  // Size in screen pixels
        
        // Ensure pins are always visible with minimum screen size
        float min_screen_size = 2.0f;  // Minimum 2 pixels
        float max_screen_size = 20.0f; // Maximum 20 pixels to prevent huge pins
        
        float pin_radius = std::max(min_screen_size, std::min(max_screen_size, screen_pin_size));
        
        // Determine pin colors based on selection state
        ImU32 current_pin_fill_color = pin_fill_color;
        ImU32 current_pin_outline_color = pin_outline_color;
        float current_outline_thickness = 1.5f;
          // Check if this pin is selected - Yellow glowing effect
        if (selected_pin_index == static_cast<int>(pin_index)) {
            current_pin_fill_color = IM_COL32(255, 255, 100, 255);    // Bright yellow
            current_pin_outline_color = IM_COL32(255, 215, 0, 255);   // Golden yellow
            current_outline_thickness = 3.0f;  // Thicker outline for selection
            
            // Add glowing effect with multiple circles
            float glow_radius = pin_radius + 2.0f;
            draw_list->AddCircle(ImVec2(x, y), glow_radius, IM_COL32(255, 255, 0, 100), 0, 2.0f);
            glow_radius = pin_radius + 4.0f;
            draw_list->AddCircle(ImVec2(x, y), glow_radius, IM_COL32(255, 255, 0, 50), 0, 1.5f);
        }
        // Check if this pin is connected to the selected pin's net - Net highlighting
        else if (selected_pin_index >= 0 && selected_pin_index < static_cast<int>(pcb_data->pins.size()) && 
                 !pin.net.empty() && !pcb_data->pins[selected_pin_index].net.empty() &&
                 pin.net == pcb_data->pins[selected_pin_index].net &&
                 pin.net != "UNCONNECTED" && pin.net != "") {
            // Same yellow color and glow effect as selected pin for connected net
            current_pin_fill_color = IM_COL32(255, 255, 100, 255);    // Bright yellow
            current_pin_outline_color = IM_COL32(255, 215, 0, 255);   // Golden yellow
            current_outline_thickness = 3.0f;  // Thicker outline for net connection
            
            // Add glowing effect with multiple circles (same as selected pin)
            float glow_radius = pin_radius + 2.0f;
            draw_list->AddCircle(ImVec2(x, y), glow_radius, IM_COL32(255, 255, 0, 100), 0, 2.0f);
            glow_radius = pin_radius + 4.0f;
            draw_list->AddCircle(ImVec2(x, y), glow_radius, IM_COL32(255, 255, 0, 50), 0, 1.5f);
        }        // Check if this pin is hovered
        else if (hovered_pin_index == static_cast<int>(pin_index)) {
            current_pin_fill_color = IM_COL32(255, 150, 150, 255);    // Light red highlight
            current_pin_outline_color = IM_COL32(255, 100, 100, 255); // Red outline
            current_outline_thickness = 2.0f;  // Slightly thicker for hover
        }
        
        // Only draw pins if they're visible (not too small)
        if (pin_radius >= 0.5f) {
            // Draw pin as filled circle
            draw_list->AddCircleFilled(ImVec2(x, y), pin_radius, current_pin_fill_color);            // Only draw outline if pins are large enough
            if (pin_radius > 2.0f) {  // Lower threshold for outlines
                draw_list->AddCircle(ImVec2(x, y), pin_radius, current_pin_outline_color, 0, current_outline_thickness);
            }
              // Show BOTH pin number and net name INSIDE the pin circle - adaptive threshold
            if (pin_radius > 12.0f) {  // Show text when pin is large enough on screen
                // Use smaller font size to fit both texts inside
                ImGui::PushFont(nullptr); // Use default smaller font
                
                std::string pin_number = "";
                std::string net_name = "";
                
                // Get pin number
                if (!pin.snum.empty()) {
                    pin_number = pin.snum;
                } else if (!pin.name.empty()) {
                    pin_number = pin.name;
                }
                
                // Get net name (prefer meaningful names)
                if (!pin.net.empty() && pin.net != "UNCONNECTED" && pin.net != "") {
                    if (pin.net.substr(0, 4) != "NET_") {
                        net_name = pin.net;  // Meaningful names (VCC, GND, etc.)
                    } else {
                        net_name = pin.net;  // Show generic NET_ names too
                    }
                }
                
                // Calculate text sizes for positioning
                ImVec2 pin_text_size = pin_number.empty() ? ImVec2(0,0) : ImGui::CalcTextSize(pin_number.c_str());
                ImVec2 net_text_size = net_name.empty() ? ImVec2(0,0) : ImGui::CalcTextSize(net_name.c_str());
                
                float text_spacing = 2.0f; // Space between pin number and net name
                float total_text_height = pin_text_size.y + net_text_size.y + text_spacing;
                
                // Position pin number at TOP of circle (BLACK text)
                if (!pin_number.empty()) {
                    ImVec2 pin_text_pos(
                        x - pin_text_size.x * 0.5f, 
                        y - total_text_height * 0.5f
                    );
                    draw_list->AddText(pin_text_pos, IM_COL32(0, 0, 0, 255), pin_number.c_str());
                }
                
                // Position net name at BOTTOM of circle (RED text)
                if (!net_name.empty()) {
                    ImVec2 net_text_pos(
                        x - net_text_size.x * 0.5f, 
                        y - total_text_height * 0.5f + pin_text_size.y + text_spacing
                    );
                    draw_list->AddText(net_text_pos, IM_COL32(255, 0, 0, 255), net_name.c_str());
                }
                
                ImGui::PopFont();
            }
        }
    }
    
    // Pins rendering complete
}

// Pin selection functionality
bool PCBRenderer::HandleMouseClick(float screen_x, float screen_y, int window_width, int window_height) {
    if (!pcb_data || pcb_data->pins.empty()) {
        return false;
    }
    
    // Convert screen coordinates to world coordinates
    float world_x, world_y;
    ScreenToWorld(screen_x, screen_y, world_x, world_y, window_width, window_height);
    
    // Check if click is near any pin
    float click_radius = 20.0f / camera.zoom; // Adjust click tolerance based on zoom
    
    for (size_t i = 0; i < pcb_data->pins.size(); ++i) {
        const auto& pin = pcb_data->pins[i];
        float dx = world_x - pin.pos.x;
        float dy = world_y - pin.pos.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        // Calculate pin visual radius
        float pin_radius = std::max(3.0f, 5.0f * camera.zoom) / camera.zoom;
        
        if (distance <= pin_radius + click_radius) {
            // Pin clicked - toggle selection
            if (selected_pin_index == static_cast<int>(i)) {
                selected_pin_index = -1; // Deselect if already selected
            } else {
                selected_pin_index = static_cast<int>(i); // Select this pin
            }
            return true; // Consumed the click
        }
    }
    
    // Click on empty area - deselect
    selected_pin_index = -1;
    return false; // Click not consumed
}

void PCBRenderer::ClearSelection() {
    selected_pin_index = -1;
}

int PCBRenderer::GetHoveredPin(float screen_x, float screen_y, int window_width, int window_height) {
    if (!pcb_data || pcb_data->pins.empty()) {
        return -1;
    }
    
    // Convert screen coordinates to world coordinates
    float world_x, world_y;
    ScreenToWorld(screen_x, screen_y, world_x, world_y, window_width, window_height);
    
    // Check if mouse is near any pin
    float hover_radius = 15.0f / camera.zoom; // Slightly smaller than click radius
    
    for (size_t i = 0; i < pcb_data->pins.size(); ++i) {
        const auto& pin = pcb_data->pins[i];
        float dx = world_x - pin.pos.x;
        float dy = world_y - pin.pos.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        // Calculate pin visual radius
        float pin_radius = std::max(3.0f, 5.0f * camera.zoom) / camera.zoom;
        
        if (distance <= pin_radius + hover_radius) {
            return static_cast<int>(i);
        }
    }
    
    return -1; // No pin hovered
}

// Coordinate conversion methods
void PCBRenderer::ScreenToWorld(float screen_x, float screen_y, float& world_x, float& world_y,
                               int window_width, int window_height) {
    // Convert screen coordinates to world coordinates using camera transform
    world_x = camera.x + (screen_x - window_width * 0.5f) / camera.zoom;
    world_y = camera.y + (window_height * 0.5f - screen_y) / camera.zoom;
}

void PCBRenderer::WorldToScreen(float world_x, float world_y, float& screen_x, float& screen_y,
                               int window_width, int window_height) {
    // Convert world coordinates to screen coordinates using camera transform
    screen_x = (world_x - camera.x) * camera.zoom + window_width * 0.5f;
    screen_y = window_height * 0.5f - (world_y - camera.y) * camera.zoom;
}
