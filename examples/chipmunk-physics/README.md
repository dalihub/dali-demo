# Chipmunk Physics Example

This is an example showing how to use Chipmunk2D physics library to create and control physics objects in DALi.
It creates a set of balls which act under gravity, and the letters of the DALi logo that are each "suspended" from 
a fixed pivot. Every 7 seconds, a random number of the balls are given a high impulse to bring the whole demo back
to life.

![](./chipmunk.gif)

"wasd" keys move the last touched actor up/down/left/right.
"qe" keys rotate the last touched actor in Z axis
"p" key resets the position/forces on the last touched actor to the origin
Space key toggles the integration state.
"m" key toggles the debug state
