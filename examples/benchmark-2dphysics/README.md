# 2d Physics benchmark Example

This is an example to test the performance of DALI with and without physics enabled.
It first measures performance with physics and collisions off, by animating motion using
property notifications for 30 seconds. Uses N ImageViews, where N defaults to 500

Then, it creates a PhysicsAdaptor and uses zero gravity and a bounding box to achieve a
similar visual result with N ImageViews attached to physics bodies.

N can be changed on the command line.



