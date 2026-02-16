# Raytracer in C

A first attempt at making a raytracer form scratch using C.

## How to use

### Dependencies

[raylib](https://www.raylib.com/)

### Start up

Just type `make`.

### Web version

A version runnable from the web is available on itch.io (WIP)

# Write up

## Showing spheres on the screen

Step 1 is to show colored spheres on the screen. 
Perspective should be as you'd expect (if sphere A is behind sphere B, sphere B should show up, not sphere A).

### Setting the screen up

The "screen" requires 3 elements:

- a camera
- a viewport
- a world (something to look at)

<img width="400" alt="image" src="https://github.com/user-attachments/assets/d95c480f-0f79-49ef-aec5-a5776556c149" />

### Why spheres?

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

### Figuring out if a point is on a sphere

What's that? "Didn't you say all this requires is checking that the distance of the point is equal to the radius? What do you need a whole section for?"

Because, what, you were gonna check EVERY point to see if it's one radius away from the center? EVERY point of an INFINITE 3D space?

Please, I'd love to see you try that. Myself, I'll resort to something much easier on my computer: computing the position of each point given the sphere's center and a direction going from the camera to the viewport.

"But aren't there INFINITE possible directions to look at? How's that gonna help you?"

Good point. So I'll resort to a small simplification: I'm only gonna set the direction going from the camera to **one of the viewport's pixels**.
No matter how big your viewport, it only has a finite number of pixels, just like your monitor.

The situation is like this:

<img width="400" alt="image" src="https://github.com/user-attachments/assets/a8854c32-fb80-447f-9b30-3c16b8c212d9" />

As you can see, the viewport's "pixels" form a grid. Using the coordinates of each pixel we can create a vector from the camera to that pixel, and then extend it into the world (by scaling it with a scalar).

What we're doing is creating a ray (R) starting from the camera.

Now, from the sphere's perspective, we get three possible situations:

1. R doesn't intersect with the sphere's surface
2. R intersects with the sphere in exactly one point (it's a tangent to the sphere's surface)
3. R goes right into the sphere, intersecting with it's surface in 2 points (on the way in and out)

All we need is a way to find if and where the ray and sphere instersect.

### Finding the intersection of a ray and a sphere

Spoiler: we can use a quadratic equation.

How? What we want is to $0 <= k <= 2$ points of the line that will be exactly one radius away from the sphere's center.

The way I chose to do it is by using the following equation: 

```math
\|\lambda \vec{R}_{camera} - \vec{C}_{sphere} \|^2 = r^2
```

Here, we need to solve for $\lambda$, which is the scalar that would scale the "eye of the camera" vector to exactly the position of one of the points on the sphere's surface.

Unfolding the expression to make the quadratic equation appear, we get (I'm skipping over a lot of linear algebra):

```math
\|\vec{R}\|^2 \lambda^2 - 2 (\vec{R} \cdot \vec{C}) \lambda + \|\vec{C}\|^2 - r^2 = 0
```

Delta:

```math
\Delta = 4 (\vec{R} \cdot \vec{C}) ^ 2 - 4 \|\vec{R}\|^2 (\|\vec{C}\|^2 - r^2)
```

If :

- $\Delta < 0$: the ray R does not intersect the sphere
- $\Delta = 0$: R is tangent to the sphere
- $\Delta > 0$: R goes in and out of the sphere, intersecting it's surface in two points

<img width="300" alt="image" src="https://github.com/user-attachments/assets/1fcd3107-cb3f-4b98-b95c-6abd1e97f262" />
<img width="300" alt="image" src="https://github.com/user-attachments/assets/43390a49-c849-4145-8482-1048b9de140f" />
<img width="300" alt="image" src="https://github.com/user-attachments/assets/20da5f9f-cf1a-46d2-a26e-7b2f8bf87702" />




And now we solve for $\lambda$:

```math
\lambda = \dfrac{2 (\vec{R} \cdot \vec{C}) \pm \sqrt{\Delta}}{2\|\vec{R}\Vert^2}
```

### Finalising sphere intersection

Now that we know whether a given ray intersects or not with a sphere, we can draw that information on the screen.

That is, if we figure out which sphere we intersected with, we can paint its color on the corresponding pixel of our viewport/screen. 
If we intersected several spheres with one ray, we can use z ordering to paint the one closest to us.

The result is as follows:

<img width="400"  alt="image" src="https://github.com/user-attachments/assets/214f1e1c-b861-47e2-a660-b05736c6d5b6" />

WOW! THAT'S... utterly unimpressive.

"Wait, weren't these supposed to be spheres? Why do we get circles?"

Yeah... That's because we have no light/shadow stuff going on, so there's no perspective at all. Also explains why everything looks so bland: it's just single colors.

Okay, time to fix this!

## Bringing light to the world

To make our spheres actually look like spheres, we need a way to convey depth to the person looking.

Here we will explore how to simulate light.

### General approach

There are 3 main types of light often used in games:

#### Directional lights

Directional lights are lights coming from the same angle no matter where you are in our scene. One good approximation in our world is the Sun: it is so far away that even if you teleport one kilometer away in any direction, your angle to the Sun won't (visibly) change.

#### Point lights

Point lights are represented by a point in space, meaning they will interact differently with objects depending on where they are.
This is just like a lamp set on the ceiling: if you start moving around it, your shadow's angle to the light will change.

#### Ambient lights

Ambient lights are applied uniformly to every surface in our scene. They are more like a general "tint" you give your objects, for example to make the overall scene brighter.

Now that we have our lights, it's time to figure out how to show light on the screen. For ambient lights this is easy: just apply the light intensity everywhere.
But for direcional and point lights we need to compute the angle the light gets reflected at.

Why? Because all light gets reflected to some degree when it encounters an obstacle, but not all in the same direction. More importantly: not all in OUR direction.

The light that we see is the one that reaches our eye/camera. Therefore we need to check how much light bounced in our direction before we can really know how the scene should look.

<img width="400" alt="illustration of light getting reflected, or not, towards the camera" src="https://github.com/user-attachments/assets/bcc4433a-9db5-4c89-be0b-e50725a1e0b2" />

### Reflecting light

To compute the direction the light gets reflected in, we need to know the angle of the surface it hits. For this we use normal vectors.

The normal vector, noted $$\vec{n}$$ in the picture below, is perpendicular (aka orthogonal) to the surface symbolised by the horizontal line.
Thanks to this, we can reflect the incoming vector on the right by "turning it upwards" such that the reflection will have the same angle to the normal as the original vector:

<img width="400" alt="image" src="https://github.com/user-attachments/assets/8d5301ef-f899-4b7f-8209-f72838542ea6" />

It's the "turning it upwards" part that requires some thinking. Below is the method I chose (IMPORTANT: all vector are of length 1, otherwise this DOES NOT work)

1. Project the tail of the light vector onto $$\vec{n}$$;
2. Compute where the position of the tip of the reflected vector by getting twice the distance of the light's tail to the point we just projected on $$\vec{n}$$;
<img width="400" alt="image" src="https://github.com/user-attachments/assets/ac9d11aa-a9b9-44c8-b86c-8b2c6a4c275f" />

Okay, so far so good. Let's see what this looks like...

<img width="400"  alt="image" src="https://github.com/user-attachments/assets/214f1e1c-b861-47e2-a660-b05736c6d5b6" />

WOW! THAT'S EXACTLY THE SAME AS THE PREVIOUS TIME!

Why?

"Wait, wait! I think I know! It's because we're only looking at light that would reflect EXACTLY in the direction of the camera. But that's a tiny minority of reflections so it looks like there is no light at all!"

CORRECT!

3. Project the tip of the reflected light onto the "camera's gaze" vector (the vector coming from the camera to the specific point of the sphere's surface);
<img width="400" alt="image" src="https://github.com/user-attachments/assets/f02ed27a-9de7-4e19-bb51-663516ce2288" />

4. The projection should be between 0 and 1 unit away from the surface of the sphere. 
<img width="400" alt="image" src="https://github.com/user-attachments/assets/3c902578-dee1-4353-9e82-76887304fdc2" />

### Making the sphere look rugged

