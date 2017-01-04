/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
 */

#include "beat-control-impl.h"
#include <dali-toolkit/dali-toolkit.h>
#include <dali/public-api/object/type-registry-helper.h>
#include <dali-toolkit/devel-api/align-enums.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/visuals/visual-properties-devel.h>

#include <cstdio>

using namespace Dali; // Needed for macros
using namespace Dali::Toolkit;

namespace Demo
{
namespace Internal
{

namespace
{


Dali::BaseHandle Create()
{
  return Demo::BeatControl::New();
}

DALI_TYPE_REGISTRATION_BEGIN( BeatControl, Dali::Toolkit::Control, Create );

DALI_PROPERTY_REGISTRATION( Demo, BeatControl, "bounceTransition", STRING, BOUNCE_TRANSITION );
DALI_PROPERTY_REGISTRATION( Demo, BeatControl, "leftTransition", STRING, LEFT_TRANSITION );
DALI_PROPERTY_REGISTRATION( Demo, BeatControl, "upTransition", STRING, UP_TRANSITION );
DALI_PROPERTY_REGISTRATION( Demo, BeatControl, "beatVisual", MAP, BEAT_VISUAL );
DALI_TYPE_REGISTRATION_END();


Toolkit::TransitionData ConvertPropertyToTransition( const Property::Value& value )
{
  Toolkit::TransitionData transitionData;

  if( value.GetType() == Property::ARRAY )
  {
    transitionData = Toolkit::TransitionData::New( *value.GetArray());
  }
  else if( value.GetType() == Property::MAP )
  {
    transitionData = Toolkit::TransitionData::New( *value.GetMap() );
  }
  return transitionData;
}

} // anonymous namespace


Internal::BeatControl::BeatControl()
: Control( ControlBehaviour( REQUIRES_STYLE_CHANGE_SIGNALS ) )
{
}

Internal::BeatControl::~BeatControl()
{
}

Demo::BeatControl Internal::BeatControl::New()
{
  IntrusivePtr<Internal::BeatControl> impl = new Internal::BeatControl();
  Demo::BeatControl handle = Demo::BeatControl( *impl );
  impl->Initialize();
  return handle;
}


void BeatControl::StartBounceAnimation()
{
  if( mAnimation )
  {
    mAnimation.Stop();
    mAnimation.FinishedSignal().Disconnect( this, &BeatControl::OnBounceAnimationFinished );
    OnBounceAnimationFinished(mAnimation);
  }

  mAnimation = CreateTransition( mBounceTransition );
  mAnimation.FinishedSignal().Connect( this, &BeatControl::OnBounceAnimationFinished );
  mAnimation.Play();
}


void BeatControl::StartXAnimation()
{
  if( mXAnimation )
  {
    mXAnimation.Stop();
    mXAnimation.FinishedSignal().Disconnect( this, &BeatControl::OnXAnimationFinished );
    OnXAnimationFinished(mXAnimation);
  }

  mXAnimation = CreateTransition( mLeftTransition );
  mXAnimation.FinishedSignal().Connect( this, &BeatControl::OnXAnimationFinished );
  mXAnimation.Play();
}

void BeatControl::StartYAnimation()
{
  if( mYAnimation )
  {
    mYAnimation.Stop();
    mYAnimation.FinishedSignal().Disconnect( this, &BeatControl::OnYAnimationFinished );
    OnYAnimationFinished(mYAnimation);
  }

  mYAnimation = CreateTransition( mUpTransition );
  mYAnimation.FinishedSignal().Connect( this, &BeatControl::OnYAnimationFinished );
  mYAnimation.Play();
}


void BeatControl::OnBounceAnimationFinished( Animation& src )
{
  // Do stuff
}
void BeatControl::OnXAnimationFinished( Animation& src )
{
  // Do stuff
}
void BeatControl::OnYAnimationFinished( Animation& src )
{
  // Do stuff
}

void BeatControl::OnInitialize()
{
  Actor self = Self();
}

void BeatControl::OnStageConnection( int depth )
{
  Control::OnStageConnection( depth );
}

void BeatControl::OnStageDisconnection()
{
  Control::OnStageDisconnection();
}

void BeatControl::OnSizeSet( const Vector3& targetSize )
{
  Control::OnSizeSet( targetSize );
  RelayoutVisuals( Vector2( targetSize ) );
}

void BeatControl::OnRelayout( const Vector2& targetSize, RelayoutContainer& container )
{
  RelayoutVisuals( targetSize );
}

void BeatControl::RelayoutVisuals( const Vector2& targetSize )
{
  if( mVisual )
  {
    Vector2 size( targetSize );
    Property::Map transformMap;
    // Make the visual half the size of the control, but leave
    // origin and anchor point at center, position is relative, but Zer0
    transformMap[ DevelVisual::Transform::Property::SIZE ] = Vector2(0.5, 0.5);
    mVisual.SetTransformAndSize( transformMap, size );

    // @todo We must stop this clashing with a transform animation
  }
}

Vector3 BeatControl::GetNaturalSize()
{
  if( mVisual )
  {
    Vector2 naturalSize;
    mVisual.GetNaturalSize(naturalSize);
    return Vector3(naturalSize);
  }
  return Vector3::ZERO;
}

void BeatControl::OnStyleChange( Toolkit::StyleManager styleManager, StyleChange::Type change )
{
  // Chain up.
  Control::OnStyleChange( styleManager, change );
}


///////////////////////////////////////////////////////////
//
// Properties
//

void BeatControl::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Demo::BeatControl beatControl = Demo::BeatControl::DownCast( Dali::BaseHandle( object ) );

  if( beatControl )
  {
    BeatControl& impl = GetImpl( beatControl );
    Actor self = impl.Self();
    switch ( index )
    {
      case Demo::BeatControl::Property::BEAT_VISUAL:
      {
        Property::Map* map = value.GetMap();
        if( map )
        {
          impl.mVisual = Toolkit::VisualFactory::Get().CreateVisual( *map );
          impl.RegisterVisual( Demo::BeatControl::Property::BEAT_VISUAL, impl.mVisual );
        }
        break;
      }
      case Demo::BeatControl::Property::BOUNCE_TRANSITION:
      {
        impl.mBounceTransition = ConvertPropertyToTransition( value );
        break;
      }
      case Demo::BeatControl::Property::LEFT_TRANSITION:
      {
        impl.mLeftTransition = ConvertPropertyToTransition( value );
        break;
      }
      case Demo::BeatControl::Property::UP_TRANSITION:
      {
        impl.mUpTransition = ConvertPropertyToTransition( value );
        break;
      }
    }
  }
}

Property::Value BeatControl::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Property::Value value;

  Demo::BeatControl beatControl = Demo::BeatControl::DownCast( Dali::BaseHandle( object ) );

  if ( beatControl )
  {
    BeatControl& impl = GetImpl( beatControl );
    switch ( propertyIndex )
    {
      case Demo::BeatControl::Property::BEAT_VISUAL:
      {
        if( impl.mVisual )
        {
          Property::Map map;
          impl.mVisual.CreatePropertyMap(map);
          value = map;
        }
        break;
      }
      case Demo::BeatControl::Property::BOUNCE_TRANSITION:
      default:
        break;
    }
  }

  return value;
}


} // Internal
} // Demo
