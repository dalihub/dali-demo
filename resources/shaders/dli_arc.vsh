//@verson 100

#ifdef HIGHP
  precision highp float;
#else
  precision mediump float;
#endif

INPUT vec3 aPosition;
INPUT vec2 aTexCoord;

OUTPUT vec2 vUV;

UNIFORM_BLOCK VertBlock
{
UNIFORM vec3 uSize;

UNIFORM mat4 uProjection;
UNIFORM mat4 uModelMatrix;
UNIFORM mat4 uViewMatrix;

UNIFORM vec2 uTilt;
};

void main()
{
  vec4 vPosition = vec4( aPosition * uSize, 1.0);
  vec4 mPosition = uModelMatrix * vPosition;

  mPosition.xy += 0.3 * uTilt * mPosition.z;
  gl_Position = uProjection * uViewMatrix * mPosition;

#ifdef FLIP_V
  vUV = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
#else
  vUV = aTexCoord;
#endif

}
