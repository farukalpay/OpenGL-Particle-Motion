# OpenGL Particle Motion
[![Hits](https://hits.sh/github.com/farukalpay/OpenGL-Particle-Motion.svg)](https://hits.sh/github.com/farukalpay/OpenGL-Particle-Motion/)

This project simulates the motion of electrons and protons using Coulomb's Law. The simulation is visually represented on-screen using OpenGL.

![](https://github.com/farukalpay/OpenGL-Particle-Motion/blob/gh-pages/images/simulation.gif)
![](https://github.com/farukalpay/OpenGL-Particle-Motion/blob/gh-pages/images/simulation2.gif)

# Requirements
This project requires OpenGL, [freeglut](https://www.transmissionzero.co.uk/software/freeglut-devel/) and [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h) After installing all of these onto your system, the code will run without any issues.

# Code Explanation

```
const float width = 10.0f;  // The width of the image. Adjust as necessary
const float halfWidth = width / 2.0f;  // Half the width, which we'll use often

// Scale factor for positions (1 piksel = 1 pikometer)
const float SCALE = std::pow(10.0f, -12.0f);

// Charges are given in nano-Coulombs
float q1 = 1.0f;  // nano-Coulombs
float q2 = 1.0f; // nano-Coulombs

...
```

In this section of the code, various variables that will be used to simulate the electric forces between two particles are declared and initialized.

```
void updateForces() {
    float r = std::abs(pos2 - pos1) / SCALE + 2 * halfWidth ;
    float forceMagnitude = k * std::abs(q1 * q2) / (r * r);
    float direction = (q1 * q2) > 0 ? -1.0f : 1.0f;

    // Convert force to pikometer by multiplying by SCALE
    force1 = direction * forceMagnitude * SCALE / mass1;
    force2 = -direction * forceMagnitude * SCALE / mass2;
}
```

In this function, r represents the distance, which is `|position of q2 - position of q1| / picometer + (2 * half width of the electron/proton images).` The reason for doing `2 * half width of the electron` is to prevent using the center of the image as x=0.

forceMagnitude represents Coulomb's Law which is `k * |Q1 * Q2| / |r * r|`

direction variable is -1.0f if `(q1 * q2) > 0` otherwise direction variable is set to 1.0f. positive value indicates move to right, on the other hand negative value indicates move to left.

```
force1 = direction * forceMagnitude * SCALE / mass1;
force2 = -direction * forceMagnitude * SCALE / mass2;
```

This calculations provides us with the particles electrostatic force-induced acceleration, and it is scaled appropriately for the graphical display which is picometer.

```
void updateVelocities() {
    // Friction force is proportional to the velocity and in the opposite direction
    float frictionForce1 = friction * std::abs(speed1) * -std::copysign(1.0f, speed1);
    float frictionForce2 = friction * std::abs(speed2) * -std::copysign(1.0f, speed2);
    speed1 += (force1 + frictionForce1) * dt;
    speed2 += (force2 + frictionForce2) * dt;
    pos1 += speed1 * dt;
    pos2 += speed2 * dt;

}
```

```
float frictionForce1 = friction * std::abs(speed1) * -std::copysign(1.0f, speed1);
float frictionForce2 = friction * std::abs(speed2) * -std::copysign(1.0f, speed2);
```

These are the variables i use to take friction into account. In essence, friction is multiplied by the absolute value of the spped, then the signs of the speed variables are obtained and assigned to 1.0f. This is then multiplied by negative sign and the remaining part.

```
speed1 += (force1 + frictionForce1) * dt;
speed2 += (force2 + frictionForce2) * dt;
pos1 += speed1 * dt;
pos2 += speed2 * dt;
```

These lines update the two particles' velocities (speed1 and speed2). The updated velocity is the current velocity multiplied by the time step plus the total force (the sum of the electrostatic and friction forces). This is an application of Newton's second law of motion (F=ma), which takes the form a = F/m, with the velocity change (delta v) equal to a*dt.


`GLuint texture[2];` This line creates an array of two GLuints. This array will hold the references for two textures that will be loaded later.

```
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
```

The LoadTexture function in this code is used to load an image from a file and create a 2D OpenGL texture from the image data. `unsigned char* image = stbi_load(filename, &width, &height, &comp, STBI_rgb_alpha);` In this code the function loads an image using stb_image library's stbi_load function, which returns a pointer to the pixel data.

`GLuint texture; glGenTextures(1, &texture);` This code generates a new OpenGL texture identifier using the glGenTextures function. The first argument 1 is the number of texture names to be generated. Remember, because the indexing begins from 1 it generates 2 pictures.

`glBindTexture(GL_TEXTURE_2D, texture);` This code is for binding the texture object that's referenced by the texture variable to the current active 2D texture unit in OpenGL. GL_TEXTURE_2D specifies that the texture is a 2D texture, texture is the identifier of the texture we want to bind.

`glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);` This code set the minification and magnification filters for the texture, respectively.

```
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP);
```

These codes are setting the wrapping mode for the S and T texture coordinates. S corresponds to the X-axis, and T corresponds to the Y-axis

`glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);` uploads the texture data to the GPU

`GL_TEXTURE_2D` This is the target texture type, `GL_RGBA` This is the internal format of the texture. Which means each pixel has Red, Green, Blue, and Alpha components. `width` and `height` are the dimensions of the texture. The next `GL_RGBA` is the format of the pixel data that we're providing. `GL_UNSIGNED_BYTE` This is the data type of the pixel data. Each component is an unsigned byte, i.e., a value from 0 to 255. `image` This is the actual pixel data, which we loaded earlier with `stbi_load`

`stbi_image_free(image);` This code frees the image data that we loaded into memory with `stbi_load` It is for free memory that the software do not need anymore.

`return texture;` the LoadTexture function returns the texture ID that was generated with `glGenTexture`.

```
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
```

This is a function used in OpenGL to draw a textured 2D quad (rectangle) at a certain position with certain dimensions.

`glBindTexture(GL_TEXTURE_2D, texture);` OpenGL uses this function to render a textured 2D quad. 
`glEnable(GL_TEXTURE_2D);` This enables 2D Texturing
`glBegin(GL_QUADS);` With this, a new quad is defined. This quad will be regarded as including all the vertices specified after this call but before the corresponding `glEnd()`

```
glTexCoord2f(0, 0);
glVertex2f(x - width / 2, y - height / 2);
```

The bottom-left quadrant's first vertex's texture coordinate is set using this codes. The bottom-left corner of the texture is indicated by the texture coordinate (0, 0)
`glVertex2f(x - width / 2, y - height / 2) specifies the position of this vertex.`
`glEnd();` This ends the definition of the quad.
`glDisable(GL_TEXTURE_2D);` This disables 2D texturing.

```
void drawText(const char* text, float x, float y) {
    float currentColor[4];
    glGetFloatv(GL_CURRENT_COLOR, currentColor);

    glColor3f(0.0f, 1.0f, 0.0f); // Set text color green
    glRasterPos2f(x, y);
    int len = strlen(text);
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
    glColor4fv(currentColor);
}
```

This is a function used in OpenGL to draw text at a certain position on the screen.

```
float currentColor[4];
glGetFloatv(GL_CURRENT_COLOR, currentColor);
```

The current drawing color is saved in `float currentColor[4]` array. The current color being used for drawing is obtained and stored in currentColor by `glGetFloatv(GL_CURRENT_COLOR, currentColor)` In short, it prevents the colors of the protons and electrons from being the same as the texts we draw on the screen.

`glColor3f(0.0f, 1.0f, 0.0f);` This code sets the drawing color to green. Three parameters represents RGB (Red, Green, Blue)

`glRasterPos2f(x, y);` This code sets the position where the text should be drawn.

```
int len = strlen(text);
    for (int i = 0; i < len; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
```

These codes draw the text character by character. `strlen(text)` This stores lenght of the text and `glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);` draws the text character by character. Because the `glutBitmapCharacter()` function in GLUT only manages one character at a time, this is necessary. Furthermore, `GLUT_BITMAP_HELVETICA_18` This represents a particular bitmap font Helvetica at a size of 18. `text[i]` This is the i-th character of the string text

`glColor4fv(currentColor);` This code restores the drawing color to what it was before the function was called. 

```
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
```

This is a basic Windows Procedure function which is a component of the Windows API. It handles messages delivered to windows.

```
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

    int argc = 0; 
    char* argv[] = { NULL };
    glutInit(&argc, argv);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-100.0f, 100.0f, -100.0f, 100.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);

    MSG msg = { 0 };
    bool running = true;

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    double freq = static_cast<double>(frequency.QuadPart);
    LARGE_INTEGER startCounter, endCounter;
    QueryPerformanceCounter(&startCounter);

    if (q1 >= 0) {
        texture[0] = LoadTexture("images\\electron.png", 1);
    }
    else {
        texture[0] = LoadTexture("images\\proton.png", 1);
    }

    if (q2 >= 0) {
        texture[1] = LoadTexture("images\\electron.png", 1);
    }
    else {
        texture[1] = LoadTexture("images\\proton.png", 1);
    }
    GLuint background = LoadTexture("images\\space.png", 1);
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

        if (frameTime > 1.0 / 120.0) {
		
            updateForces();
            updateVelocities();

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            DrawImage(background, 0.0f, 0.0f, 800, 600); // assuming 800x600 window size
            DrawImage(texture[0], pos1 + ((q1 > 0) ? halfWidth : -halfWidth), 0.0f, width, width);
            DrawImage(texture[1], pos2 + ((q2 > 0) ? halfWidth : -halfWidth), 0.0f, width, width);

            char text[1024];
            const char* formats[] = {
                "Charge Q1: %.2f nC",
                "Charge Q2: %.2f nC",
                "Speed Q1: %.2e pm/s",
                "Speed Q2: %.2e pm/s",
                "Position Q1: %.2e pm",
                "Position Q2: %.2e pm",
                "Friction: %.2f N * dt / m",
                "Distance between Q1 and Q2: %.4e pm",
                "Force Applied to Q1: %.2e N/m",
                "Force Applied to Q2: %.2e N/m"
            };

            float values[] = {
                q1, q2,  std::abs(speed1 / SCALE * dt),  std::abs(speed2 / SCALE * dt), pos1 / SCALE, pos2 / SCALE, friction, abs(pos1 - pos2) / SCALE, force1 * SCALE, force1 * SCALE, force2 * SCALE
            };

            for (int lineNumber = 0; lineNumber < 10; lineNumber++) {
                char text[1024];
                sprintf_s(text, sizeof(text), formats[lineNumber], values[lineNumber]);

                drawText(text, -100.0f, 95.0f - 8.0f * lineNumber);
            }
			
            SwapBuffers(hdc);
            startCounter = endCounter;
        }
    }

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hglrc);
    ReleaseDC(hwnd, hdc);
    return msg.wParam;
}
```

This is a Windows Application that utilizes the Win32 API and OpenGL for rendering. And `WinMain()` is the entry point for a Windows application.

```
WNDCLASS wc = { };
wc.lpfnWndProc = WindowProcedure;
wc.hInstance = hInstance;
wc.lpszClassName = CLASS_NAME;
```
The window class attributes are contained in a structure called `WNDCLASS` in the Windows API.
Here, a `WNDCLASS` structure is being set up. `hInstance` is a handle to an instance, `lpfnWndProc` is a pointer to the window procedure, and `lpszClassName` is the name of the class.



