Multibrot-3D
============

3D rendering of the Multibrot set

By Wikipedia, a Multibrot set is the set of values in the complex plane whose absolute value remains below some finite value throughout iterations by a member of the general monic univariate polynomial family of recursions

z := z^d + c

where d >= 2 but can also take on a fractional value.

Traditionally, d = 2 which creates the famous Mandelbrot fractal. The Multibrot set is thus an extension and generalization of the Mandelbrot. 

In this project, I construct a 3D rendering of the Multibrot fractal, where the value **d** is the third dimension. I render 2048 different images of the multibrot for different values of **d** in the range of [-1, 33]. Each image is 2048 by 2048 pixels, and is rendered on the Sawtooth High Performance Cluster at Utah State University. I implement optimizations that greatly improve the rendering speed, such as MPI parallelization and bounded area cutoffs. The final 3D perspectives renderings are done using OpenGL on high-end graphics cards with additional optimizations.
