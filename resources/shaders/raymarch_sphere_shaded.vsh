attribute mediump vec2 aPosition;
uniform   mediump mat4 uMvpMatrix;  // DALi shader builtin
uniform   mediump vec3 uSize; // DALi shader builtin

varying mediump vec2 vTexCoord;
varying mediump vec2 vRayCastCoord;
void main()
{
  mediump vec4 vertexPosition = vec4(aPosition, 0.0, 1.0);
  vertexPosition.xyz *= uSize;

  // In this ray march example we supply the fragment shader with 0..1 UV texture co-ordinates
  // incase someone wants to extend the code and add texture mapping to it.
  // DALi geometry typically ranges from -0.5 to 0.5, hence the +0.5 to make it 0..1

  vTexCoord = aPosition + vec2(0.5);


  // Our UV texture co-ordinates for the quad geometry should now range from 0..1
  //
  // (0,0)
  //    |--------|
  //    |      / |
  //    |    /   |
  //    |  /     |
  //    |/-------|(1,1)
  //
  //
  //
  // We're going to use the UV co-ordinates as our virtual screen / plane of projection on to our
  // raycasted scene.
  //
  // We first modify the range to be from -1,-1 to 1,1 with 0,0 in the centre. Then
  // pass this as a varying value to the fragment shader.
  //
  //
  // (-1,-1)
  //    |--------------|
  //    |       |      |
  //    |       |      |
  //    |_____(0,0)____|
  //    |       |      |
  //    |       |      |
  //    |_______|______|(1,1)
  //
  //

  vRayCastCoord = (2.0 * vTexCoord ) - 1.0 ;

  gl_Position = uMvpMatrix * vertexPosition;
}