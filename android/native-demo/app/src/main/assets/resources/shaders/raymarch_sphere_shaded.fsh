/*
 * Fragment shader for textured quad
 */
varying mediump vec2 vTexCoord;
varying mediump vec2 vRayCastCoord;

uniform mediump float uRadius;
uniform mediump float uAdjuster;

#define CAMERA_Z_POSITION 1.0  // gives us a FOV of 90 degrees if Plane of projection is at Z = 0 with size 2x2
#define SPHERE_Z_POSITION -1.0 // Sphere placed behind Plane of projection
#define SPHERE_RADIUS 0.5


// signed distance function
// returns
// < 0 if inside sphere
// 0 == on sphere surface
// > 1 if outside sphere
mediump float distanceToSphere( mediump vec3 point, mediump vec3 sphereCenter, mediump float radius )
{
  return distance( point, sphereCenter ) - radius;
}

// Simulate a simple spot light ( there's no ambient light in this example)
mediump vec4 lightSphere( mediump vec3 point, mediump vec3 sphereCenter )
{
   // the normal = direction of the vector from the sphere center to the point on the surface of the sphere
   mediump vec3 normal = normalize( point - sphereCenter );

   // Animate the light around the sphere in a circular motion
   mediump vec3 lightDirection = vec3( sin(uAdjuster)+uRadius, cos ( uAdjuster )+uRadius, CAMERA_Z_POSITION  );

   // calculate the dot product to give us the intensity of the light bouncing off the surface
   mediump float value =  dot( normal , lightDirection);

   // add a purple tint to the final color by adjust green channel by 0.84
   return vec4( value, value * 0.843, value , 1.0);

}

void main()
{
  // The fragment shader is called for every pixel that is to be drawn for our
  // quad geometry ( 2 triangles ). The size and number of pixels drawn is
  // determined by the size / position of the quad and the DALi camera position.
  //
  // For this example the vRayCastCoord is currently set to the range -1 to 1 by the Vertex Shader
  //
  // (-1,-1)
  //    |--------------|
  //    |       |      |
  //    |       |      |
  //    |_____(0,0)____|
  //    |       |      |
  //    |       |      |
  //    |_______|______|(1,1)

  mediump vec3 pixelPosition = vec3( vRayCastCoord, 0.0 );

  // uncomment line below to see red / green colors only visible when x > 0, or y > 0
  // gl_FragColor = vec4( pixelPosition, 1.0 ); return;

  // We are going to assume there is a virtual camera infront of the plane of projection
  // Side view:
  //                projection
  //                  plane (2x2)
  //                  /|
  //                /  |
  //              /    |      /----\
  //        Camera---->|     (SPHERE)
  //              \    |      \----/
  //                \  |
  //                  \|
  //      z=1        z=0     z  = -1
  //
  //
  //  Why z=1 for camera? Our projection plane is at z = 0, with plane size 2x2 which gives a 90 degree FOV
  // from the camera to the projection plane
  //
  mediump vec3 cameraPos = vec3( 0.0, 0.0, CAMERA_Z_POSITION );

  // calculate the ray direction from the camera to the pixel on the quad
  mediump vec3 rayDirection = normalize(  pixelPosition - cameraPos );

  // uncomment to visualize the normalized ray direction vector
  // gl_FragColor = vec4( rayDirection, 1.0 ); return;

  // Setup the position on radius of our virtual sphere
  mediump vec3 spherePosition = vec3( 0.0, 0.0, SPHERE_Z_POSITION );
  mediump float sphereRadius = SPHERE_RADIUS + uRadius ; // use uRadius to animate radius from small to large

  // We have the direction of the ray from the camera, now see if it
  // hits our sphere using ray marching
  // starting at a pixel position 0 on our projection plane, step in the direction
  // of ray from the camera to see if it hits our sphere
  // The concept of ray marching is the step size = minimum distance to an object

  mediump vec3 hitPoint = pixelPosition;

  int steps = 5;

  for( int i = 0; i < steps ; ++i )
  {
      // calculate the shortest distance between our hitPoint and the sphere
      mediump float distance = distanceToSphere( hitPoint, spherePosition, sphereRadius );

      // if the distance < 0 then were inside the sphere
      // if the distance > 0 then were outside the sphere
      // if we're close to the edge of the sphere, then draw it
      if( distance < 0.01 )
      {
        gl_FragColor = lightSphere(  hitPoint, spherePosition );
        return;
      }

      // move the hit point along by the distance to the spin the direction of the ray
      hitPoint +=  rayDirection * distance;

  }
  // no hit, color the pixel based on it's x,y position
  gl_FragColor = vec4(pixelPosition.x,pixelPosition.y,0.5,1);

}