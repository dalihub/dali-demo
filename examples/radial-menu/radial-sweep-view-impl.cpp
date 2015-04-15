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
void CircleSquareProjection( Vector3& position, Radian angle, float halfSide )
{
  //  135  90   45
  //     +--+--+
  //     | \|/ |
  // 180 +--+--+ 0
  //     | /|\ |
  //     +--+--+
  //  225  270  315
  if( angle >= Dali::ANGLE_45 && angle < Dali::ANGLE_135 )
  {
    position.x = halfSide * cosf(angle) / sinf(angle);
    position.y = -halfSide;
  }
  else if( angle >= Dali::ANGLE_135 && angle < Dali::ANGLE_225 )
  {
    position.x = -halfSide;
    position.y = halfSide * sinf(angle) / cosf(angle);
  }
  else if( angle >= Dali::ANGLE_225 && angle < Dali::ANGLE_315 )
  {
    position.x = -halfSide * cosf(angle) / sinf(angle);
    position.y =  halfSide;
  }
  else
  {
    position.x = halfSide;
    position.y = -halfSide * sinf(angle) / cosf(angle);
  }

  position.z = 0.0f;
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

  void operator()( Vector3& current, const PropertyInputContainer& inputs )
  {
    float degree = fmodf((inputs[0]->GetFloat() + inputs[1]->GetFloat()), 360.0f);
    if(degree < 0.0f)
    {
      degree += 360.0f;
    }

    float startAngle = (90.0f*mSideIndex)-45.0f;
    float endAngle = (90.0f*mSideIndex)+45.0f;
    if(degree < startAngle)
    {
      current = Vector3::ZERO;
    }
    else
    {
      if( degree >= endAngle )
      {
        degree = endAngle;
      }
      CircleSquareProjection( current, Degree(degree), 0.5f );
      current.x = -current.x; // Inverting X makes the animation go anti clockwise from left center
    }
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

  mStencilActor.SetOrientation( Degree(mInitialAngle), Vector3::ZAXIS );
  mStencilActor.SetProperty( mRotationAngleIndex, mInitialSector.degree );

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

  anim.AnimateTo( Property( mStencilActor, mRotationAngleIndex ), mFinalSector.degree, mEasingFunction, TimePeriod( offsetTime, duration ) );
  anim.AnimateTo( Property( mStencilActor, Actor::Property::ORIENTATION ), Quaternion( Radian( mFinalAngle ), Vector3::ZAXIS ), mEasingFunction, TimePeriod( offsetTime, duration ) );

  if( mRotateActorsWithStencil )
  {
    for(unsigned int i=0, count=mLayer.GetChildCount(); i<count; i++)
    {
      Actor actor = mLayer.GetChildAt(i);
      if( actor != mStencilActor )
      {
        anim.AnimateTo( Property( actor, Actor::Property::ORIENTATION ), Quaternion( Radian( Degree( mFinalAngle.degree - mInitialAngle.degree ) ), Vector3::ZAXIS ), mEasingFunction, TimePeriod( offsetTime, duration ) );
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
  mRotationAngleIndex = mStencilActor.RegisterProperty("rotation-angle", Property::Value(initialSector.degree));

  Source srcStart( mStencilActor, mStartAngleIndex );
  Source srcRotation( mStencilActor, mRotationAngleIndex );

  // Constrain the vertices of the square mesh to sweep out a sector as the
  // rotation angle is animated.
  Constraint constraint = Constraint::New<Vector3>( mMesh, mMesh.GetPropertyIndex(1, AnimatableVertex::Property::POSITION), SquareFanConstraint(0) );
  constraint.AddSource( srcStart );
  constraint.AddSource( srcStart );
  constraint.Apply();

  constraint = Constraint::New<Vector3>( mMesh, mMesh.GetPropertyIndex(2, AnimatableVertex::Property::POSITION), SquareFanConstraint(0) );
  constraint.AddSource( srcStart );
  constraint.AddSource( srcRotation );
  constraint.Apply();

  constraint = Constraint::New<Vector3>( mMesh, mMesh.GetPropertyIndex(3, AnimatableVertex::Property::POSITION), SquareFanConstraint(1) );
  constraint.AddSource( srcStart );
  constraint.AddSource( srcRotation );
  constraint.Apply();

  constraint = Constraint::New<Vector3>( mMesh, mMesh.GetPropertyIndex(4, AnimatableVertex::Property::POSITION), SquareFanConstraint(2) );
  constraint.AddSource( srcStart );
  constraint.AddSource( srcRotation );
  constraint.Apply();

  constraint = Constraint::New<Vector3>( mMesh, mMesh.GetPropertyIndex(5, AnimatableVertex::Property::POSITION), SquareFanConstraint(3) );
  constraint.AddSource( srcStart );
  constraint.AddSource( srcRotation );
  constraint.Apply();

  constraint = Constraint::New<Vector3>( mMesh, mMesh.GetPropertyIndex(6, AnimatableVertex::Property::POSITION), SquareFanConstraint(4) );
  constraint.AddSource( srcStart );
  constraint.AddSource( srcRotation );
  constraint.Apply();

  mStencilActor.SetDrawMode( DrawMode::STENCIL );
  mStencilActor.SetPositionInheritanceMode(USE_PARENT_POSITION);
}
