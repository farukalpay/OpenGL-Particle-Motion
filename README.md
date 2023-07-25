[![Hits](https://hits.sh/github.com/farukalpay/OpenGL-Particle-Motion.svg)](https://hits.sh/github.com/farukalpay/OpenGL-Particle-Motion/)
# OpenGL Particle Motion
This project simulates the motion of electrons and protons using Coulomb's Law. The simulation is visually represented on-screen using OpenGL.

![](https://github.com/farukalpay/OpenGL-Particle-Motion/blob/gh-pages/images/simulation.gif)

# How To Use
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
    float direction = (q1 * q2) < 0 ? -1.0f : 1.0f;

    // Convert force to pikometer by multiplying by SCALE
    force1 = direction * forceMagnitude * SCALE / mass1;
    force2 = -direction * forceMagnitude * SCALE / mass2;
}
```
In this function, r represents the distance, which is `|position of q2 - position of q1| / picometer + (2 * half width of the electron/proton images).` The reason for doing `2 * half width of the electron` is to prevent using the center of the image as x=0.

forceMagnitude represents Coulomb's Law which is `k * |Q1 * Q2| / |r * r|`

direction variable is -1.0f if `(q1 * q2) < 0` otherwise direction variable is set to 1.0f. positive value indicates move to right, on the other hand negative value indicates move to left.

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
