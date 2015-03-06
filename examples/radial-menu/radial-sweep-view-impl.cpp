/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

using namespace Dali;

namespace
{

/**
 * Method to project a point on a circle of radius halfSide at given
 * angle onto a square of side 2 * halfSide
 */
Vector3 CircleSquareProjection( Degree angle, float halfSide )
{
  Vector3 position(0.0f, 0.0f, 0.0f);
  Radian angleInRadians(angle);

  //  135  90   45
  //     +--+--+
  //     | \|/ |
  // 180 +--+--+ 0
  //     | /|\ |
  //     +--+--+
  //  225  270  315
  if( angle >= 45.0f && angle < 135.0f )
  {
    position.x = halfSide * cosf(angleInRadians) / sinf(angleInRadians);
    position.y = -halfSide;
  }
  else if( angle >= 135.0f && angle < 225.0f )
  {
    position.x = -halfSide;
    position.y = halfSide * sinf(angleInRadians) / cosf(angleInRadians);
  }
  else if( angle >= 225.0f && angle < 315.0f )
  {
    position.x = -halfSide * cosf(angleInRadians) / sinf(angleInRadians);
    position.y =  halfSide;
  }
  else
  {
    position.x = halfSide;
    position.y = -halfSide * sinf(angleInRadians) / cosf(angleInRadians);
  }
  return position;
}

float HoldZeroFastEaseInOutHoldOne(float progress)
{
  if( progress < 0.2f)
  {
    return 0.0f;
  }
  else if(progress < 0.5f)
  {
    return AlphaFunctions::EaseIn((progress-0.2) / 0.3f) * 0.5f;
  }
  else if(progress < 0.8f)
  {
    return AlphaFunctions::EaseOut((progress - 0.5f) / 0.3f) * 0.5f + 0.5f;
  }
  else
  {
    return 1.0f;
  }
}

struct SquareFanConstraint
{
  SquareFanConstraint(int sideIndex)
  : mSideIndex(sideIndex)
  {
  }

  Vector3 operator()( const Vector3& current, const PropertyInput& start, const PropertyInput& rotation )
  {
    float degree = fmodf((start.GetFloat() + rotation.GetFloat()), 360.0f);
    if(degree < 0.0f)
    {
      degree += 360.0f;
    }

    float startAngle = (90.0f*mSideIndex)-45.0f;
    float endAngle = (90.0f*mSideIndex)+45.0f;
    if(degree < startAngle)
    {
      return Vector3::ZERO;
    }
    else if( degree >= endAngle )
    {
      degree = endAngle;
    }
    Vector3 pos = CircleSquareProjection(Degree(degree), 0.5f);
    pos.x = -pos.x; // Inverting X makes the animation go anti clockwise from left center
    return pos;
  }

  int mSideIndex;
};

} // anonymous namespace


RadialSweepView RadialSweepViewImpl::New( )
{
  return New( 2.0f, 100.0f, Degree(0.0f), Degree(0.0f), Degree(0.0f), Degree(359.999f) );
}


RadialSweepView RadialSweepViewImpl::New( float duration, float diameter, Degree initialAngle, Degree finalAngle, Degree initialSector, Degree finalSector )
{
  RadialSweepViewImpl* impl= new RadialSweepViewImpl(duration, diameter, initialAngle, finalAngle, initialSector, finalSector);
  RadialSweepView handle = RadialSweepView(*impl);
  return handle;
}

RadialSweepViewImpl::RadialSweepViewImpl( float duration, float diameter, Degree initialAngle, Degree finalAngle, Degree initialSector, Degree finalSector )
: Control( CONTROL_BEHAVIOUR_NONE ),
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

void RadialSweepViewImpl::SetInitialAngle( Dali::Degree initialAngle)
{
  mInitialAngle = initialAngle;
}

void RadialSweepViewImpl::SetFinalAngle( Dali::Degree finalAngle)
{
  mFinalAngle = finalAngle;
}

void RadialSweepViewImpl::SetInitialSector( Dali::Degree initialSector)
{
  mInitialSector = initialSector;
}

void RadialSweepViewImpl::SetFinalSector( Dali::Degree finalSector)
{
  mFinalSector = finalSector;
}

void RadialSweepViewImpl::SetInitialActorAngle( Dali::Degree initialAngle )
{
  mInitialActorAngle = initialAngle;
  mRotateActors = true;
}

void RadialSweepViewImpl::SetFinalActorAngle( Dali::Degree finalAngle )
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

Dali::Degree RadialSweepViewImpl::GetInitialAngle( )
{
  return mInitialAngle;
}

Dali::Degree RadialSweepViewImpl::GetFinalAngle( )
{
  return mFinalAngle;
}

Dali::Degree RadialSweepViewImpl::GetInitialSector( )
{
  return mInitialSector;
}

Dali::Degree RadialSweepViewImpl::GetFinalSector( )
{
  return mFinalSector;
}

Dali::Degree RadialSweepViewImpl::GetInitialActorAngle( )
{
  return mInitialActorAngle;
}

Dali::Degree RadialSweepViewImpl::GetFinalActorAngle(  )
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
    mStencilActor.SetSize(mDiameter, mDiameter);
  }

  mStencilActor.SetRotation( Degree(mInitialAngle), Vector3::ZAXIS );
  mStencilActor.SetProperty( mRotationAngleIndex, static_cast<float>(mInitialSector) );

  if( mRotateActors )
  {
    for(unsigned int i=0, count=mLayer.GetChildCount(); i<count; i++)
    {
      Actor actor = mLayer.GetChildAt(i);
      if( actor != mStencilActor )
      {
        anim.RotateTo( actor, mInitialActorAngle, Vector3::ZAXIS );
      }
    }
  }

  anim.AnimateTo( Property( mStencilActor, mRotationAngleIndex ), static_cast<float>(mFinalSector), mEasingFunction, TimePeriod( offsetTime, duration) );
  anim.RotateTo( mStencilActor, mFinalAngle, Vector3::ZAXIS, mEasingFunction, offsetTime, duration );

  if( mRotateActorsWithStencil )
  {
    for(unsigned int i=0, count=mLayer.GetChildCount(); i<count; i++)
    {
      Actor actor = mLayer.GetChildAt(i);
      if( actor != mStencilActor )
      {
        anim.RotateTo( actor, Degree(mFinalAngle - mInitialAngle), Vector3::ZAXIS, mEasingFunction, offsetTime, duration );
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
        anim.RotateTo( actor, mFinalActorAngle, Vector3::ZAXIS, mEasingFunction, offsetTime, duration );
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

void RadialSweepViewImpl::CreateStencil( Degree initialSector )
{
  mMaterial = Material::New("Material");
  mMaterial.SetDiffuseColor(Color::WHITE);
  mMaterial.SetAmbientColor(Vector4(0.0, 0.1, 0.1, 1.0));

  // Generate a square mesh with a point at the center:

  AnimatableMesh::Faces faces;
  // Create triangles joining up the verts
  faces.push_back(0); faces.push_back(1); faces.push_back(2);
  faces.push_back(0); faces.push_back(2); faces.push_back(3);
  faces.push_back(0); faces.push_back(3); faces.push_back(4);
  faces.push_back(0); faces.push_back(4); faces.push_back(5);
  faces.push_back(0); faces.push_back(5); faces.push_back(6);

  mMesh = AnimatableMesh::New(7, faces, mMaterial);
  mMesh[0].SetPosition( Vector3(  0.0f,  0.0f, 0.0f ) ); // Center pt

  mStencilActor = MeshActor::New(mMesh);
  mStencilActor.SetCullFace(CullNone); // Allow clockwise & anticlockwise faces

  mStartAngleIndex = mStencilActor.RegisterProperty("start-angle", Property::Value(0.0f));
  mRotationAngleIndex = mStencilActor.RegisterProperty("rotation-angle", Property::Value(initialSector));

  Source srcStart( mStencilActor, mStartAngleIndex );
  Source srcRot( mStencilActor, mRotationAngleIndex );

  // Constrain the vertices of the square mesh to sweep out a sector as the
  // rotation angle is animated.
  mMesh.ApplyConstraint(Constraint::New<Vector3>( mMesh.GetPropertyIndex(1, AnimatableVertex::Property::POSITION),
                                                  srcStart, srcStart, SquareFanConstraint(0)));
  mMesh.ApplyConstraint(Constraint::New<Vector3>( mMesh.GetPropertyIndex(2, AnimatableVertex::Property::POSITION),
                                                  srcStart, srcRot, SquareFanConstraint(0)));
  mMesh.ApplyConstraint(Constraint::New<Vector3>( mMesh.GetPropertyIndex(3, AnimatableVertex::Property::POSITION),
                                                  srcStart, srcRot, SquareFanConstraint(1)));
  mMesh.ApplyConstraint(Constraint::New<Vector3>( mMesh.GetPropertyIndex(4, AnimatableVertex::Property::POSITION),
                                                  srcStart, srcRot, SquareFanConstraint(2)));
  mMesh.ApplyConstraint(Constraint::New<Vector3>( mMesh.GetPropertyIndex(5, AnimatableVertex::Property::POSITION),
                                                  srcStart, srcRot, SquareFanConstraint(3)));
  mMesh.ApplyConstraint(Constraint::New<Vector3>( mMesh.GetPropertyIndex(6, AnimatableVertex::Property::POSITION),
                                                  srcStart, srcRot, SquareFanConstraint(4)));

  mStencilActor.SetDrawMode( DrawMode::STENCIL );
  mStencilActor.SetPositionInheritanceMode(USE_PARENT_POSITION);
}
