#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <cmath>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "freeglut.lib")

const float width = 10.0f;  // The width of the image. Adjust as necessary
const float halfWidth = width / 2.0f;  // Half the width, which we'll use often

// Scale factor for positions (1 meter = 1.0 * 10^-12 pixels)
const float SCALE = std::pow(10.0f, -12.0f);

// Charges are given in nano-Coulombs
float q1 = 1.0f;  // nano-Coulombs
float q2 = 1.0f; // nano-Coulombs

// Positions are given in meters and scaled to pixels
// Initial positions are in the middle of the screen
float pos1 = 5; // pixels
float pos2 = -5; // pixels

// Coulomb constant (N*m^2/C^2)
const float k = 8.99f * std::pow(10.0f, 9);

// Timestep (seconds)
float dt = 0.01f;

// Initial speeds are given in m/s and scaled to pixels/second
float speed1 = 0.01f * SCALE; // pixels/second
float speed2 = 0.01f * SCALE; // pixels/second

// Masses of particles are identical to an electron (kg)
float mass1 = 9.10938356f * std::pow(10.0f, -31); // kg
float mass2 = 9.10938356f * std::pow(10.0f, -31); // kg

// Friction coefficient
float friction = 2.0f;

float force1;
float force2;

void updateForces() {
    float r = std::abs(pos2 - pos1) / SCALE + 2 * halfWidth;
    float forceMagnitude = k * std::abs(q1 * q2) / (r * r);
    float direction = (q1 * q2) < 0 ? -1.0f : 1.0f;

    // Minimum distance in meters, converted to pixels
    float minimumDistance = 2.0f * SCALE; // pixels
    // Convert force to pixel units by multiplying by SCALE
    force1 = direction * forceMagnitude * SCALE / mass1 - friction * speed1;
    force2 = -direction * forceMagnitude * SCALE / mass2 - friction * speed2;
}


GLuint texture[2];  

//Function to load an image using the STB library
GLuint LoadTexture(const char* filename, int wrap)
{
    int width, height, comp;
    unsigned char* image = stbi_load(filename, &width, &height, &comp, STBI_rgb_alpha);

    if (image == nullptr)
    {
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    stbi_image_free(image);
    return texture;
}

//Function to draw images
void DrawImage(GLuint texture, float x, float y, float width, float height)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(x - width / 2, y - height / 2);
    glTexCoord2f(1, 0);
    glVertex2f(x + width / 2, y - height / 2);
    glTexCoord2f(1, 1);
    glVertex2f(x + width / 2, y + height / 2);
    glTexCoord2f(0, 1);
    glVertex2f(x - width / 2, y + height / 2);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

//Function to drawText
void drawText(const char* text, float x, float y) {
    // Save current color before drawing text
    float currentColor[4];
    glGetFloatv(GL_CURRENT_COLOR, currentColor);

    glColor3f(0.0f, 1.0f, 0.0f); // Set text color green
    glRasterPos2f(x, y);
    int len = strlen(text);
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }

    // Restore the drawing color after rendering the text
    glColor4fv(currentColor);
}


LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    const wchar_t CLASS_NAME[] = L"Sample Window Class";

    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProcedure;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, L"Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    HWND hwnd = CreateWindow(CLASS_NAME, L"Sample Window", WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nShowCmd);

    // Set up OpenGL context
    HDC hdc = GetDC(hwnd);
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;

    int iFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, iFormat, &pfd);

    HGLRC hglrc = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hglrc);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int argc = 0; // GLUT requires the command line argument count...
    char* argv[] = { NULL }; // ...and array, but they don't need to actually have any data
    glutInit(&argc, argv);

    // Set up the orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-100.0f, 100.0f, -100.0f, 100.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);

    MSG msg = { 0 };
    bool running = true;

    // High-resolution timer setup
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    double freq = static_cast<double>(frequency.QuadPart);
    LARGE_INTEGER startCounter, endCounter;
    QueryPerformanceCounter(&startCounter);

    //Declare textures
    texture[0] = LoadTexture("C:\\Users\\faruk\\Desktop\\positive.png", 1);
    texture[1] = LoadTexture("C:\\Users\\faruk\\Desktop\\negative.png", 1);
    GLuint background = LoadTexture("C:\\Users\\faruk\\Desktop\\space.png", 1);
    while (running) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        QueryPerformanceCounter(&endCounter);
        double frameTime = (endCounter.QuadPart - startCounter.QuadPart) / freq;

        if (frameTime > 1.0 / 120.0) { // limit to 60 FPS
            // Update forces
            updateForces();

            // Update velocities and positions
            speed1 += force1 * dt;
            speed2 += force2 * dt;
            pos1 += speed1 * dt;
            pos2 += speed2 * dt;

            // Clear the window
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // In the main loop where you draw the images, adjust the position by half the width
            DrawImage(background, 0.0f, 0.0f, 800, 600); // assuming 800x600 window size
            DrawImage(texture[0], pos1 + ((q1 > 0) ? halfWidth : -halfWidth), 0.0f, width, width);
            DrawImage(texture[1], pos2 + ((q2 > 0) ? halfWidth : -halfWidth), 0.0f, width, width);

    //Draw the text
    char text[1024];
    const char* formats[] = {
        "Charge Q1: %.4f nC",
        "Charge Q2: %.4f nC",
        "Speed Q1: %.4f m/s",
        "Speed Q2: %.4f m/s",
        "Position Q1: %.4f m",
        "Position Q2: %.4f m",
        "Distance between Q1 and Q2: %.4f m",
        "Force Applied to Q1: %.4f N",
        "Force Applied to Q2: %.4f N"
    };

    float values[] = {
        q1, q2,  std::abs(speed1/SCALE),  std::abs(speed2/SCALE), pos1/SCALE, pos2/SCALE, abs(pos1 - pos2)/SCALE, force1, force2
    };

            // Draw each line of text
            for (int lineNumber = 0; lineNumber < 9; lineNumber++) {
                char text[1024];
                sprintf_s(text, sizeof(text), formats[lineNumber], values[lineNumber]);

                // Render the text, setting the y position as a function of lineNumber
                drawText(text, -100.0f, 95.0f - 8.0f * lineNumber);
            }

            // Swap buffers
            SwapBuffers(hdc);

            startCounter = endCounter;
        }
    }

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hglrc);
    ReleaseDC(hwnd, hdc);

    return msg.wParam;
}
