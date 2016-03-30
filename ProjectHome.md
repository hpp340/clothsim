**_Deformable object engine (C++) based on a SIGGRAPH paper “large steps in cloth simulation”._**

**CODE HIGHLIGHTS:**

- Optimized Baraff and Witkin’s algorithm by simplifying shear and bend force calculations, while maintaining the ODE complexity for damped harmonic oscillations between cloth vertices.

- Simple phong shading implementation using OpenGL.

**COMPILE NOTES:**

- This program was successfully compiled under Visual Studio 2008, C++ with OpenGL 2.0.

- Other dependencies should already be linked to by Visual Studio project properties (including glut and glew OpenGL libraries).

- **IMPORTANT:** There is a large memory leak issue associated with the conjugate gradient class.  The impact is small if time-step is large and number of cloth vertices is small.  Eventually, I will get around to fixing this.