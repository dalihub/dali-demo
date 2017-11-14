/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include "shadow-button-impl.h"
#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/scripting/enum-helper.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/devel-api/controls/control-devel.h>

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
  return Demo::ShadowButton::New();
}

DALI_TYPE_REGISTRATION_BEGIN( ShadowButton, Dali::Toolkit::Button, Create );

DALI_PROPERTY_REGISTRATION( Demo, ShadowButton, "activeTransition", ARRAY, ACTIVE_TRANSITION );
DALI_PROPERTY_REGISTRATION( Demo, ShadowButton, "inactiveTransition", ARRAY, INACTIVE_TRANSITION );
DALI_PROPERTY_REGISTRATION( Demo, ShadowButton, "checkTransition", ARRAY, CHECK_TRANSITION );
DALI_PROPERTY_REGISTRATION( Demo, ShadowButton, "uncheckTransition", ARRAY, UNCHECK_TRANSITION );
DALI_PROPERTY_REGISTRATION( Demo, ShadowButton, "backgroundVisual", MAP, BACKGROUND_VISUAL );
DALI_PROPERTY_REGISTRATION( Demo, ShadowButton, "checkboxBgVisual", MAP, CHECKBOX_BG_VISUAL );
DALI_PROPERTY_REGISTRATION( Demo, ShadowButton, "checkboxFgVisual", MAP, CHECKBOX_FG_VISUAL );
DALI_PROPERTY_REGISTRATION( Demo, ShadowButton, "labelVisual", MAP, LABEL_VISUAL );
DALI_PROPERTY_REGISTRATION( Demo, ShadowButton, "checkState", BOOLEAN, ACTIVE_STATE );
DALI_PROPERTY_REGISTRATION( Demo, ShadowButton, "checkState", BOOLEAN, CHECK_STATE );

DALI_TYPE_REGISTRATION_END();

DALI_ENUM_TO_STRING_TABLE_BEGIN( VISUAL_PROPERTIES )
{ "backgroundVisual", Demo::ShadowButton::Property::BACKGROUND_VISUAL },
{ "checkboxBgVisual", Demo::ShadowButton::Property::CHECKBOX_BG_VISUAL},
{ "checkboxFgVisual", Demo::ShadowButton::Property::CHECKBOX_FG_VISUAL},
{ "labelVisual",      Demo::ShadowButton::Property::LABEL_VISUAL}
DALI_ENUM_TO_STRING_TABLE_END( VISUAL_PROPERTIES )

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


Internal::ShadowButton::ShadowButton()
: Control( ControlBehaviour( REQUIRES_STYLE_CHANGE_SIGNALS ) ),
  mCheckState(false),
  mActiveState(false)
{
}

Internal::ShadowButton::~ShadowButton()
{
}

Demo::ShadowButton Internal::ShadowButton::New()
{
  IntrusivePtr<Internal::ShadowButton> impl = new Internal::ShadowButton();
  Demo::ShadowButton handle = Demo::ShadowButton( *impl );
  impl->Initialize();
  return handle;
}

void ShadowButton::SetActiveState( bool active )
{
  if( active != mActiveState )
  {
    mActiveState = active;
    if( active )
    {
      StartTransition( Demo::ShadowButton::Property::ACTIVE_TRANSITION );
    }
    else
    {
      StartTransition( Demo::ShadowButton::Property::INACTIVE_TRANSITION );
    }
  }
}

bool ShadowButton::GetActiveState()
{
  return mActiveState;
}

void ShadowButton::SetCheckState( bool checkState )
{
  mCheckState = checkState;
  DevelControl::EnableVisual( *this, Demo::ShadowButton::Property::CHECKBOX_FG_VISUAL, true );
  if( Self().OnStage() )
  {
    if( checkState )
    {
      StartTransition( Demo::ShadowButton::Property::CHECK_TRANSITION );
    }
    else
    {
      StartTransition( Demo::ShadowButton::Property::UNCHECK_TRANSITION );
    }
  }
  RelayoutRequest();
}

bool ShadowButton::GetCheckState()
{
  return mCheckState;
}

void ShadowButton::StartTransition( Property::Index transitionId )
{
  Transitions::iterator iter = FindTransition( transitionId );
  if( iter != mTransitions.end() )
  {
    if( iter->mAnimation )
    {
      iter->mAnimation.Stop();
      iter->mPlaying = false;

      iter->mAnimation.FinishedSignal().Disconnect( this, &ShadowButton::OnTransitionFinished );
    }

    iter->mAnimation = DevelControl::CreateTransition( *this, iter->mTransitionData );
    StoreTargetLayouts( iter->mTransitionData );

    iter->mAnimation.FinishedSignal().Connect( this, &ShadowButton::OnTransitionFinished );
    iter->mAnimation.Play();
    iter->mPlaying = true;
  }
}

void ShadowButton::OnTransitionFinished( Animation& src )
{
  ShadowButton::Transitions::iterator iter = mTransitions.begin();
  for( ; iter != mTransitions.end(); ++iter )
  {
    if( iter->mAnimation == src )
    {
      iter->mPlaying = false;
      iter->mAnimation.Reset(); // Remove the animation when it's finished.
      switch( iter->mTransitionId )
      {
        case Demo::ShadowButton::Property::ACTIVE_TRANSITION:
        {
          // Consider relayouting the text.
          break;
        }
        case Demo::ShadowButton::Property::INACTIVE_TRANSITION:
        {
          // Consider relayouting the text.
          break;
        }
        case Demo::ShadowButton::Property::UNCHECK_TRANSITION:
        {
          DevelControl::EnableVisual( *this, Demo::ShadowButton::Property::CHECKBOX_FG_VISUAL, false );
          break;
        }
      }
      break;
    }
  }
}

void ShadowButton::OnInitialize()
{
  Actor self = Self();
}

void ShadowButton::OnStageConnection( int depth )
{
  Control::OnStageConnection( depth );
}

void ShadowButton::OnStageDisconnection()
{
  Control::OnStageDisconnection();
}

void ShadowButton::OnSizeSet( const Vector3& targetSize )
{
  Control::OnSizeSet( targetSize );
  RelayoutVisuals( Vector2( targetSize ) );
}

void ShadowButton::OnRelayout( const Vector2& targetSize, RelayoutContainer& container )
{
  RelayoutVisuals( targetSize );
}

void ShadowButton::RelayoutVisuals( const Vector2& targetSize )
{
  bool transitioning = false;
  ShadowButton::Transitions::iterator iter = mTransitions.begin();
  for( ; iter != mTransitions.end(); ++iter )
  {
    if( iter->mPlaying == true )
    {
      transitioning = true;
      break;
    }
  }

  if( ! transitioning )
  {
    for( ShadowButton::Transforms::iterator iter = mTransforms.begin();
         iter != mTransforms.end(); ++iter )
    {
      switch( iter->mTransformId )
      {
        case Demo::ShadowButton::Property::BACKGROUND_VISUAL:
        {
          mBackgroundVisual.SetTransformAndSize( iter->mTransform, targetSize );
          break;
        }
        case Demo::ShadowButton::Property::CHECKBOX_BG_VISUAL:
        {
          mCheckboxBgVisual.SetTransformAndSize( iter->mTransform, targetSize );
          break;
        }
        case Demo::ShadowButton::Property::CHECKBOX_FG_VISUAL:
        {
          mCheckboxFgVisual.SetTransformAndSize( iter->mTransform, targetSize );
          break;
        }
        case Demo::ShadowButton::Property::LABEL_VISUAL:
        {
          mLabelVisual.SetTransformAndSize( iter->mTransform, targetSize );
          break;
        }
      }
    }
  }
}

Vector3 ShadowButton::GetNaturalSize()
{
  int width;
  int height;

  Vector2 checkboxBgSize;
  Vector2 checkboxFgSize;
  Vector2 labelSize;
  mCheckboxBgVisual.GetNaturalSize( checkboxBgSize );
  mCheckboxFgVisual.GetNaturalSize( checkboxFgSize );
  mLabelVisual.GetNaturalSize( labelSize );

  width = std::max( checkboxBgSize.x, checkboxFgSize.x ) + labelSize.x;
  height = std::max( std::max( checkboxFgSize.y, checkboxBgSize.y ), labelSize.y );

  return Vector3( width, height, height );
}

void ShadowButton::OnStyleChange( Toolkit::StyleManager styleManager, StyleChange::Type change )
{
  // Chain up.
  Control::OnStyleChange( styleManager, change );
}

ShadowButton::Transitions::iterator ShadowButton::FindTransition( Property::Index index )
{
  bool found = false;
  ShadowButton::Transitions::iterator iter = mTransitions.begin();
  for( ; iter != mTransitions.end(); ++iter )
  {
    if( iter->mTransitionId == index )
    {
      found = true;
      break;
    }
  }
  if( ! found )
  {
    iter = mTransitions.end();
  }
  return iter;
}

ShadowButton::Transforms::iterator ShadowButton::FindTransform( Property::Index index )
{
  bool found = false;
  ShadowButton::Transforms::iterator iter = mTransforms.begin();
  for( ; iter != mTransforms.end(); ++iter )
  {
    if( iter->mTransformId == index )
    {
      found = true;
      break;
    }
  }
  if( ! found )
  {
    iter = mTransforms.end();
  }
  return iter;
}

void ShadowButton::ResetVisual(
  Property::Index        index,
  Visual::Base&          visual,
  const Property::Value& value )
{
  if( visual )
  {
    // we are replacing an existing visual, so force relayout
    RelayoutRequest();
  }
  Property::Map* map = value.GetMap();
  if( map )
  {
    visual = Toolkit::VisualFactory::Get().CreateVisual( *map );

    // Set the appropriate depth index.
    // @todo Should be able to set this from the style sheet
    switch( index )
    {
      case Demo::ShadowButton::Property::BACKGROUND_VISUAL:
      {
        DevelControl::RegisterVisual( *this, index, visual, 0 );
        break;
      }
      case Demo::ShadowButton::Property::CHECKBOX_BG_VISUAL:
      {
        DevelControl::RegisterVisual( *this, index, visual, 1 );
        break;
      }
      case Demo::ShadowButton::Property::CHECKBOX_FG_VISUAL:
      {
        DevelControl::RegisterVisual( *this, index, visual, mCheckState, 2 );
        break;
      }
      case Demo::ShadowButton::Property::LABEL_VISUAL:
      {
        DevelControl::RegisterVisual( *this, index, visual, 1 );
        break;
      }
    }

    // Extract transform maps out of the visual definition and store them
    Property::Value* value = map->Find( Visual::Property::TRANSFORM, "transform");
    if( value )
    {
      Property::Map* transformMap = value->GetMap();
      if( transformMap )
      {
        ShadowButton::Transforms::iterator iter = FindTransform( index );
        if( iter != mTransforms.end() )
        {
          iter->mTransform = *transformMap;
        }
        else
        {
          mTransforms.push_back( Transform( index, *transformMap ) );
        }
      }
    }
  }
}

bool IsTransformProperty( const std::string& property )
{
  const char* transformProperties[]= { "size", "offset", "origin", "anchorPoint", "offsetPolicy", "sizePolicy" };
  const int NUM_TRANSFORM_PROPERTIES = sizeof( transformProperties ) / sizeof( const char * );

  bool found=false;
  for( int i=0; i<NUM_TRANSFORM_PROPERTIES; ++i )
  {
    if( property == transformProperties[i] )
    {
      found = true;
      break;
    }
  }
  return found;
}

void ShadowButton::StoreTargetLayouts( TransitionData transitionData )
{
  // Pseudo code
  // foreach animator in transitionData
  //   if animator{"property"} in [ "size", "offset", "origin", "anchorPoint", "offsetPolicy", "sizePolicy" ]
  //     transforms{ animator{"target"} }->{animator{"property"}} = animator{"targetValue"}


  for( unsigned int i=0; i < transitionData.Count(); ++i )
  {
    Property::Map animator = transitionData.GetAnimatorAt(i);
    Property::Value* target = animator.Find( "target" );
    if( target )
    {
      // Convert to index
      Property::Index index;
      if( Scripting::GetEnumerationProperty( *target, VISUAL_PROPERTIES_TABLE, VISUAL_PROPERTIES_TABLE_COUNT, index ) )
      {
        ShadowButton::Transforms::iterator iter = FindTransform( index );
        if( iter != mTransforms.end() )
        {
          Property::Value* property = animator.Find( "property" );
          if( property )
          {
            std::string propertyString;
            property->Get(propertyString);
            if( IsTransformProperty( propertyString ) )
            {
              Property::Value* targetValue = animator.Find( "targetValue" );
              if( targetValue )
              {
                iter->mTransform[ propertyString ] = *targetValue;
              }
            }
          }
        }
      }
    }
  }
}

void ShadowButton::ResetTransition(
  Property::Index        index,
  const Property::Value& value)
{
  ShadowButton::Transitions::iterator iter = FindTransition( index );
  if( iter != mTransitions.end() )
  {
    // Already exists
    iter->mTransitionData = ConvertPropertyToTransition( value );
    iter->mAnimation.Stop();
    iter->mAnimation.Clear();
  }
  else
  {
    mTransitions.push_back( Transition( index, ConvertPropertyToTransition( value ) ) );
  }
}


void ShadowButton::SetProperty( BaseObject* object, Property::Index index, const Property::Value& value )
{
  Demo::ShadowButton shadowButton = Demo::ShadowButton::DownCast( Dali::BaseHandle( object ) );

  if( shadowButton )
  {
    ShadowButton& impl = GetImpl( shadowButton );
    switch ( index )
    {
      case Demo::ShadowButton::Property::BACKGROUND_VISUAL:
      {
        impl.ResetVisual( index, impl.mBackgroundVisual, value );
        break;
      }
      case Demo::ShadowButton::Property::CHECKBOX_BG_VISUAL:
      {
        impl.ResetVisual( index, impl.mCheckboxBgVisual, value );
        break;
      }
      case Demo::ShadowButton::Property::CHECKBOX_FG_VISUAL:
      {
        impl.ResetVisual( index, impl.mCheckboxFgVisual, value );
        DevelControl::EnableVisual( impl, Demo::ShadowButton::Property::CHECKBOX_FG_VISUAL, impl.mCheckState );
        break;
      }
      case Demo::ShadowButton::Property::LABEL_VISUAL:
      {
        impl.ResetVisual( index, impl.mLabelVisual, value );
        break;
      }
      case Demo::ShadowButton::Property::ACTIVE_TRANSITION:
      case Demo::ShadowButton::Property::INACTIVE_TRANSITION:
      case Demo::ShadowButton::Property::CHECK_TRANSITION:
      case Demo::ShadowButton::Property::UNCHECK_TRANSITION:
      {
        impl.ResetTransition( index, value );
        break;
      }
    }
  }
}

Property::Value ShadowButton::GetProperty( BaseObject* object, Property::Index propertyIndex )
{
  Property::Value value;

  Demo::ShadowButton shadowButton = Demo::ShadowButton::DownCast( Dali::BaseHandle( object ) );

  if ( shadowButton )
  {
    ShadowButton& impl = GetImpl( shadowButton );
    switch ( propertyIndex )
    {
      case Demo::ShadowButton::Property::BACKGROUND_VISUAL:
      {
        Property::Map map;
        impl.mBackgroundVisual.CreatePropertyMap(map);
        value = map;
        break;
      }
      case Demo::ShadowButton::Property::CHECKBOX_BG_VISUAL:
      {
        Property::Map map;
        impl.mCheckboxBgVisual.CreatePropertyMap(map);
        value = map;
        break;
      }
      case Demo::ShadowButton::Property::CHECKBOX_FG_VISUAL:
      {
        Property::Map map;
        impl.mCheckboxFgVisual.CreatePropertyMap(map);
        value = map;
        break;
      }
      case Demo::ShadowButton::Property::LABEL_VISUAL:
      {
        Property::Map map;
        impl.mLabelVisual.CreatePropertyMap(map);
        value = map;
        break;
      }

      default:
        break;
    }
  }

  return value;
}


} // Internal
} // Demo
