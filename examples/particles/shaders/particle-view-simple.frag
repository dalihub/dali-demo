//@version 100

// Shader for an unlit, unfogged, textured mesh.

precision mediump float;

UNIFORM_BLOCK VanillaFrag
{
UNIFORM vec4 uColor;
};

void main()
{
 gl_FragColor = uColor;
}
