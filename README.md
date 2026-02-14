# Raytracer in C

A first attempt at making a raytracer form scratch using C.

## How to use

### Dependencies

[raylib](https://www.raylib.com/)

### Start up

Just type `make`.

### Web version

A version runnable from the web is available on itch.io (WIP)

## Write up

### Showing spheres on the screen

Step 1 is to show colored spheres on the screen. 
Perspective should be as you'd expect (if sphere A is behind sphere B, sphere B should show up, not sphere A).

#### Setting up the screen

The "screen" requires 3 elements:

- a camera
- a viewport
- a world (something to look at)

<img width="400" alt="image" src="https://github.com/user-attachments/assets/d95c480f-0f79-49ef-aec5-a5776556c149" />

#### Why spheres?

I know, I know. The first thing I do is draw a cube and call it "World", and yet we're here to talk about spheres!

So, why spheres. Because a sphere is a point (the center) + a radius. Very simple.

To make things better, this also implies that all **visible** points (the ones that will show up on the screen) of a sphere are
exactly one radius away from it's center.

Contrast this with a cube: a cube's edges are much farther to its center than points at the center of one of its faces.

<img width="400" alt="image" src="https://github.com/user-attachments/assets/3b2287f4-d0cc-40c1-b78b-d5ac69f81d84" />

See how far the "Far" point is? Nuts!

To sum up:
- figuring out if a point is part of a sphere requires knowing if it's distance to the center equals the radius;
- figuring out if a point is part of a cube requires knowing it's distance to the center, the rotation of the cube in 3D space, and then a bunch of calculations;

#### Figuring out if a point is on a sphere

What's that? "Didn't you say all this requires is checking that the distance of the point is equal to the radius? What do you need a whole section for?"

Because, what, you were gonna check EVERY point to see if it's one radius away from the center? EVERY point of an INFINITE 3D space?

Please, I'd love to see you try that. Myself, I'll resort to something much easier on my computer: computing the position of each point given the sphere's center and a direction going from the camera to the viewport.

"But aren't there INFINITE possible directions to look at? How's that gonna help you?"

Good point. So I'll resort to a small simplification: I'm only gonna set the direction going from the camera to **one of the viewport's pixels**.
No matter how big your viewport, it only has a finite number of pixels, just like your monitor.

The situation is like this:

<img width="400" alt="image" src="https://github.com/user-attachments/assets/a8854c32-fb80-447f-9b30-3c16b8c212d9" />

As you can see, the viewport's "pixels" form a grid. Using the coordinates of each pixel we can create a vector from the camera to that pixel, and then extend it into the world (by scaling it with a scalar).

We can now use the vector's direction to create a line (L) passing through the origin.

Now, from the sphere's perspective, we get three possible situations:

1. L doesn't intersect with the sphere's surface
2. L intersects with the sphere in exactly one point (it's a tangent to the sphere's surface)
3. L goes right into the sphere, intersecting with it's surface in 2 points (on the way in and out)
