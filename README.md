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

`force1 = direction * forceMagnitude * SCALE / mass1;
force2 = -direction * forceMagnitude * SCALE / mass2;` This calculations provides us with the particles electrostatic force-induced acceleration, and it is scaled appropriately for the graphical display which is picometer.

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
`float frictionForce1 = friction * std::abs(speed1) * -std::copysign(1.0f, speed1);
float frictionForce2 = friction * std::abs(speed2) * -std::copysign(1.0f, speed2);` These are the variables i use to take friction into account. In essence, friction is multiplied by the absolute value of the spped, then the signs of the speed variables are obtained and assigned to 1.0f. This is then multiplied by negative sign and the remaining part.

```
speed1 += (force1 + frictionForce1) * dt;
speed2 += (force2 + frictionForce2) * dt;
pos1 += speed1 * dt;
pos2 += speed2 * dt;
```
These lines update the two particles' velocities (speed1 and speed2). The updated velocity is the current velocity multiplied by the time step plus the total force (the sum of the electrostatic and friction forces). This is an application of Newton's second law of motion (F=ma), which takes the form a = F/m, with the velocity change (delta v) equal to a*dt.
