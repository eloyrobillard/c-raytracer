# Raytracer in C

## How to

### Dependencies

[raylib](https://www.raylib.com/)

### Start up

Just type `make`.

### Web version

A version runnable from the web is available on itch.io (WIP)

## Log

### Showing spheres on the screen

Step 1 is to show colored spheres on the screen. 
Perspective should be as you'd expect (if sphere A is behind sphere B, sphere B should show up, not sphere A).

#### Setting up the screen

The "screen" requires 3 elements:

- a camera
- a viewport
- a world (something to look at)

<img width="374" alt="image" src="https://github.com/user-attachments/assets/d95c480f-0f79-49ef-aec5-a5776556c149" />

#### Why spheres?

I know, I know. The first thing I do is draw a cube and call it "World", and yet we're here to talk about spheres!

So, why spheres. Because a sphere is a point (the center) + a radius. Very simple.

To make things better, this also implies that all **visible** points (the ones that will show up on the screen) of a sphere are
exactly one radius away from it's center.

Contrast this with a cube: a cube's edges are much farther to its center than points at the center of one of its faces.

<img width="374" alt="image" src="https://github.com/user-attachments/assets/3b2287f4-d0cc-40c1-b78b-d5ac69f81d84" />

See how far the "Far" point is? Nuts!

