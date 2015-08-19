/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "radial-sweep-view-impl.h"

#include <dali/devel-api/rendering/renderer.h>
#include <sstream>

using namespace Dali;

namespace
{

const char* VERTEX_SHADER_PREFIX( "#define MATH_PI_2 1.570796\n#define MATH_PI_4 0.785398\n" );

const char* VERTEX_SHADER = DALI_COMPOSE_SHADER(
attribute mediump float   aAngleIndex;\n
attribute mediump vec2    aPosition1;\n
attribute mediump vec2    aPosition2;\n
uniform   mediump mat4    uMvpMatrix;\n
uniform   mediump float   uStartAngle;\n
uniform   mediump float   uRotationAngle;\n
\n
void main()\n
{\n
  float currentAngle = uStartAngle + uRotationAngle;\n
  float angleInterval1 =  MATH_PI_4 * aAngleIndex;\n
  vec4 vertexPosition = vec4(0.0, 0.0, 0.0, 1.0);\n
  if( currentAngle >=  angleInterval1)\n
  {\n
    float angleInterval2 =  angleInterval1 + MATH_PI_2;\n
    float angle = currentAngle < angleInterval2 ? currentAngle : angleInterval2;\n
    float delta;\n
    if( mod( aAngleIndex+4.0, 4.0) < 2.0  )\n
    {\n
      delta = 0.5 - 0.5*cos(angle) / sin(angle);\n
    }\n
    else\n
    {\n
      delta = 0.5 + 0.5*sin(angle) / cos(angle);\n
    }\n
    vertexPosition.xy = mix( aPosition1, aPosition2, delta );\n
  }\n
  gl_Position = uMvpMatrix * vertexPosition;\n
}
);

const char* FRAGMENT_SHADER = DALI_COMPOSE_SHADER(
uniform lowp  vec4    uColor;\n
\n
void main()\n
{\n
  gl_FragColor = uColor;\n
}\n
);

float HoldZeroFastEaseInOutHoldOne(float progress)
{
  if( progress < 0.2f)
  {
    return 0.0f;
  }
  else if(progress < 0.5f)
  {
    progress = (progress-0.2) / 0.3f;
    return  progress*progress*progress*0.5f;
  }
  else if(progress < 0.8f)
  {
    progress = ((progress - 0.5f) / 0.3f) - 1.0f;
    return (progress*progress*progress+1.0f) * 0.5f + 0.5f;
  }
  else
  {
    return 1.0f;
  }
}

} // anonymous namespace


RadialSweepView RadialSweepViewImpl::New( )
{
  return New( 2.0f, 100.0f, ANGLE_0, ANGLE_0, ANGLE_0, ANGLE_360 );
}


RadialSweepView RadialSweepViewImpl::New( float duration, float diameter, Radian initialAngle, Radian finalAngle, Radian initialSector, Radian finalSector )
{
  RadialSweepViewImpl* impl= new RadialSweepViewImpl(duration, diameter, initialAngle, finalAngle, initialSector, finalSector);
  RadialSweepView handle = RadialSweepView(*impl);
  return handle;
}

RadialSweepViewImpl::RadialSweepViewImpl( float duration, float diameter, Radian initialAngle, Radian finalAngle, Radian initialSector, Radian finalSector )
: Control( ControlBehaviour( ACTOR_BEHAVIOUR_NONE ) ),
  mDuration(duration),
  mDiameter(diameter),
  mInitialAngle(initialAngle),
  mFinalAngle(finalAngle),
  mInitialSector(initialSector),
  mFinalSector(finalSector),
  mInitialActorAngle(0),
  mFinalActorAngle(0),
  mEasingFunction(HoldZeroFastEaseInOutHoldOne),
  mStartAngleIndex(Property::INVALID_INDEX),
  mRotationAngleIndex(Property::INVALID_INDEX),
  mRotateActorsWithStencil(false),
  mRotateActors(false)
{
}

void RadialSweepViewImpl::SetDuration(float duration)
{
  mDuration = duration;
}

void RadialSweepViewImpl::SetEasingFunction( Dali::AlphaFunction easingFunction )
{
  mEasingFunction = easingFunction;
}

void RadialSweepViewImpl::SetDiameter(float diameter)
{
  mDiameter = diameter;
}

void RadialSweepViewImpl::SetInitialAngle( Dali::Radian initialAngle)
{
  mInitialAngle = initialAngle;
}

void RadialSweepViewImpl::SetFinalAngle( Dali::Radian finalAngle)
{
  mFinalAngle = finalAngle;
}

void RadialSweepViewImpl::SetInitialSector( Dali::Radian initialSector)
{
  mInitialSector = initialSector;
}

void RadialSweepViewImpl::SetFinalSector( Dali::Radian finalSector)
{
  mFinalSector = finalSector;
}

void RadialSweepViewImpl::SetInitialActorAngle( Dali::Radian initialAngle )
{
  mInitialActorAngle = initialAngle;
  mRotateActors = true;
}

void RadialSweepViewImpl::SetFinalActorAngle( Dali::Radian finalAngle )
{
  mFinalActorAngle = finalAngle;
  mRotateActors = true;
}

float RadialSweepViewImpl::GetDuration( )
{
  return mDuration;
}

float RadialSweepViewImpl::GetDiameter( )
{
  return mDiameter;
}

Dali::Radian RadialSweepViewImpl::GetInitialAngle( )
{
  return mInitialAngle;
}

Dali::Radian RadialSweepViewImpl::GetFinalAngle( )
{
  return mFinalAngle;
}

Dali::Radian RadialSweepViewImpl::GetInitialSector( )
{
  return mInitialSector;
}

Dali::Radian RadialSweepViewImpl::GetFinalSector( )
{
  return mFinalSector;
}

Dali::Radian RadialSweepViewImpl::GetInitialActorAngle( )
{
  return mInitialActorAngle;
}

Dali::Radian RadialSweepViewImpl::GetFinalActorAngle(  )
{
  return mFinalActorAngle;
}

void RadialSweepViewImpl::RotateActorsWithStencil(bool rotate)
{
  mRotateActorsWithStencil = rotate;
}

void RadialSweepViewImpl::Add(Actor actor)
{
  if( ! mLayer )
  {
    mLayer = Layer::New();
    Self().Add(mLayer);
    mLayer.SetSize( Stage::GetCurrent().GetSize() );
    mLayer.SetPositionInheritanceMode(USE_PARENT_POSITION);
  }

  mLayer.Add(actor);
}

void RadialSweepViewImpl::Activate( Animation anim, float offsetTime, float duration )
{
  bool startAnimation=false;
  if( ! anim )
  {
    mAnim = Animation::New( mDuration );
    anim = mAnim;
    startAnimation = true;
  }

  if( ! mStencilActor )
  {
    CreateStencil( mInitialSector );
    mLayer.Add( mStencilActor );
    mStencilActor.SetScale(mDiameter);
  }

  mStencilActor.SetOrientation( mInitialAngle, Vector3::ZAXIS );
  mStencilActor.SetProperty( mRotationAngleIndex, mInitialSector.radian );

  if( mRotateActors )
  {
    for(unsigned int i=0, count=mLayer.GetChildCount(); i<count; i++)
    {
      Actor actor = mLayer.GetChildAt(i);
      if( actor != mStencilActor )
      {
        anim.AnimateTo( Property( actor, Actor::Property::ORIENTATION ), Quaternion( Radian( mInitialActorAngle ), Vector3::ZAXIS ) );
      }
    }
  }

  anim.AnimateTo( Property( mStencilActor, mRotationAngleIndex ), mFinalSector.radian, mEasingFunction, TimePeriod( offsetTime, duration ) );
  anim.AnimateTo( Property( mStencilActor, Actor::Property::ORIENTATION ), Quaternion( Radian( mFinalAngle ), Vector3::ZAXIS ), mEasingFunction, TimePeriod( offsetTime, duration ) );

  if( mRotateActorsWithStencil )
  {
    for(unsigned int i=0, count=mLayer.GetChildCount(); i<count; i++)
    {
      Actor actor = mLayer.GetChildAt(i);
      if( actor != mStencilActor )
      {
        anim.AnimateTo( Property( actor, Actor::Property::ORIENTATION ), Quaternion( Radian( mFinalAngle.radian - mInitialAngle.radian ) , Vector3::ZAXIS ), mEasingFunction, TimePeriod( offsetTime, duration ) );
      }
    }
  }
  else if( mRotateActors )
  {
    for(unsigned int i=0, count=mLayer.GetChildCount(); i<count; i++)
    {
      Actor actor = mLayer.GetChildAt(i);
      if( actor != mStencilActor )
      {
        anim.AnimateTo( Property( actor, Actor::Property::ORIENTATION ), Quaternion( Radian( mFinalActorAngle ), Vector3::ZAXIS ), mEasingFunction, TimePeriod( offsetTime, duration ) );
      }
    }
  }


  if( startAnimation )
  {
    anim.SetLooping(true);
    anim.Play();
  }
}


void RadialSweepViewImpl::Deactivate()
{
  if( mAnim )
  {
    mAnim.Stop();
  }
  // mLayer.Remove( mStencilActor );
  // mStencilActor.Reset();
  // mMesh.Reset();
  // mMaterial.Reset();
}

void RadialSweepViewImpl::CreateStencil( Radian initialSector )
{
  // Create the stencil mesh geometry
  //     3-----2
  //     | \ / |
  //     |  0--1 , 6
  //     | / \ |
  //     4-----5

  struct VertexPosition { float angleIndex; Vector2 position1; Vector2 position2; };
  VertexPosition vertexData[7] = { // With X coordinate inverted to make the animation go anti clockwise from left center
      { 9.f,  Vector2( 0.f, 0.f ),     Vector2( 0.f, 0.f )     }, // center point, keep static
      { 0.f,  Vector2( -0.5f, 0.f ),   Vector2( -0.5f, 0.f )   }, // vertex 1, 0 degree, keep static
      { -1.f, Vector2( -0.5f, 0.5f ),  Vector2( -0.5f, -0.5f ) }, // -45 ~ 45 degrees  ( 0 ~ 45)
      { 1.f,  Vector2( -0.5f, -0.5f ), Vector2( 0.5f, -0.5f )  }, // 45 ~ 135 degrees
      { 3.f,  Vector2( 0.5f, -0.5f ),  Vector2( 0.5f, 0.5f )   }, // 135 ~ 225 degrees
      { 5.f,  Vector2( 0.5f, 0.5f ),   Vector2( -0.5f, 0.5f )  }, // 225 ~ 315 degrees
      { 7.f,  Vector2( -0.5f, 0.5f ),  Vector2( -0.5f, -0.5f ) }  // 315 ~ 405 degrees ( 315 ~ 359.999 )
  };
  Property::Map vertexFormat;
  vertexFormat["aAngleIndex"] = Property::FLOAT;
  vertexFormat["aPosition1"] = Property::VECTOR2;
  vertexFormat["aPosition2"] = Property::VECTOR2;
  PropertyBuffer vertices = PropertyBuffer::New( vertexFormat, 7u );
  vertices.SetData( vertexData );

  unsigned int indexData[15] = { 0,1,2,0,2,3,0,3,4,0,4,5,0,5,6 };
  Property::Map indexFormat;
  indexFormat["indices"] = Property::INTEGER;
  PropertyBuffer indices = PropertyBuffer::New( indexFormat, 15u );
  indices.SetData( indexData );

  Geometry meshGeometry = Geometry::New();
  meshGeometry.AddVertexBuffer( vertices );
  meshGeometry.SetIndexBuffer( indices );

  // Create material
  std::ostringstream vertexShaderStringStream;
  vertexShaderStringStream<<VERTEX_SHADER_PREFIX<<VERTEX_SHADER;
  Shader shader = Shader::New( vertexShaderStringStream.str(), FRAGMENT_SHADER );
  Material material = Material::New( shader );

  // Create renderer
  Renderer renderer = Renderer::New( meshGeometry, material );

  mStencilActor = Actor::New();
  mStencilActor.AddRenderer( renderer );
  mStencilActor.SetSize(1.f, 1.f);

  // register properties
  mStartAngleIndex = mStencilActor.RegisterProperty("uStartAngle", 0.f);
  mRotationAngleIndex = mStencilActor.RegisterProperty("uRotationAngle", initialSector.radian);

  mStencilActor.SetDrawMode( DrawMode::STENCIL );
  mStencilActor.SetPositionInheritanceMode(USE_PARENT_POSITION);
}
