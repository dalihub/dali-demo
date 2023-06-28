This test is to validate the behaviour of UniformBlocks.

It generates 200 actors with the same renderer. The frag shader
has a 1k color table in a uniform block, with a color index property
that is incremented for each actor that's created.

On a 100ms timer, it removes the oldest actor and creates a new actor
with the next color index.

On first touch, it changes the shader, and subtracts the currently indexed
color from the color in the vertex buffer.

This shows that:
    o Many uniform blocks are instantiated on a double-buffered UniformBuffer,
    o Despite being a sparse shader, the full uniform block size is allocated,
    o After touch, if everything is grey, then the color defined in the uniform
      block is the same as the color defined in the vertex buffer.





