# Instance draw test for Dali::Renderer

This test is to validate the behaviour of DevelRenderer::Property::INSTANCE_COUNT.

Per each frame, 2048 instance render drawed.
We change the first instance index what we using, by Dali::Animation.

At shader, use uniforms from UniformBlocks which pre-defined at first time.
And per each instance, use seperated offset and color value, and render it.

We can test instance rendering works well for 4 kinds of vertex data - Multile Renderer case / Vertex only / 16 bit indecies / 32 bit indecies.
To change the type of geometry, click the screen.

Check how instance rendering is faster than normal multiple draw call case.
