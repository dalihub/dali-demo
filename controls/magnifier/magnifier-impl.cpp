/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include "magnifier-impl.h"

// EXTERNAL INCLUDES
#include <dali-toolkit/devel-api/controls/control-devel.h>
#include <dali-toolkit/devel-api/visual-factory/visual-base.h>
#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>
#include <dali-toolkit/public-api/visuals/border-visual-properties.h>
#include <dali-toolkit/public-api/visuals/visual-properties.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/window-devel.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/render-tasks/render-task-list.h>

using Dali::Integration::GetStdString;
using Dali::Integration::ToDaliString;
using Dali::Integration::ToDaliStringView;
using Dali::Integration::ToPropertyValue;
using Dali::Integration::ToStdString;

namespace Dali::Demo::Internal
{
namespace // unnamed namespace
{
Dali::BaseHandle Create()
{
  return Demo::Magnifier::New();
}

// clang-format off
DALI_TYPE_REGISTRATION_BEGIN(Demo::Magnifier, Toolkit::Control, Create)

DALI_PROPERTY_REGISTRATION(Demo, Magnifier, "frameVisibility",      BOOLEAN, FRAME_VISIBILITY    )
DALI_PROPERTY_REGISTRATION(Demo, Magnifier, "magnificationFactor",  FLOAT,   MAGNIFICATION_FACTOR)

DALI_ANIMATABLE_PROPERTY_REGISTRATION(Demo, Magnifier, "sourcePosition",  VECTOR3, SOURCE_POSITION)

DALI_TYPE_REGISTRATION_END()
// clang-format on

const float IMAGE_BORDER_INDENT = 5.0f; ///< Indent of border in pixels.

struct CameraActorPositionConstraint
{
  CameraActorPositionConstraint(const Vector2& windowSize, float defaultCameraDistance = 0.0f)
  : mWindowSize(windowSize),
    mDefaultCameraDistance(defaultCameraDistance)
  {
  }

  void operator()(Vector3& current, const PropertyInputContainer& inputs)
  {
    const Vector3& sourcePosition = inputs[0]->GetVector3();

    current.x = sourcePosition.x + mWindowSize.x * 0.5f;
    current.y = sourcePosition.y + mWindowSize.y * 0.5f;
    current.z = sourcePosition.z + mDefaultCameraDistance;
  }

  Vector2 mWindowSize;
  float   mDefaultCameraDistance;
};

struct RenderTaskViewportPositionConstraint
{
  RenderTaskViewportPositionConstraint(const Vector2& windowSize)
  : mWindowSize(windowSize)
  {
  }

  void operator()(Vector2& current, const PropertyInputContainer& inputs)
  {
    current = inputs[0]->GetVector3(); // World position?

    // should be updated when:
    // Magnifier's world position/size/scale/parentorigin/anchorpoint changes.
    // or Magnifier camera's world position changes.
    Vector3 size = inputs[1]->GetVector3() * inputs[2]->GetVector3(); /* magnifier-size * magnifier-scale */

    // Reposition, and resize viewport to reflect the world bounds of this Magnifier actor.
    current.x += (mWindowSize.width - size.width) * 0.5f;
    current.y += (mWindowSize.height - size.height) * 0.5f;
  }

  Vector2 mWindowSize;
};

struct RenderTaskViewportSizeConstraint
{
  RenderTaskViewportSizeConstraint()
  {
  }

  void operator()(Vector2& current, const PropertyInputContainer& inputs)
  {
    current = inputs[0]->GetVector3() * inputs[1]->GetVector3(); /* magnifier-size * magnifier-scale */
  }
};

} // unnamed namespace

///////////////////////////////////////////////////////////////////////////////////////////////////
// Magnifier
///////////////////////////////////////////////////////////////////////////////////////////////////

Dali::Demo::Magnifier Magnifier::New()
{
  // Create the implementation
  MagnifierPtr magnifier(new Magnifier());

  // Pass ownership to CustomActor via derived handle
  Dali::Demo::Magnifier handle(*magnifier);

  // Second-phase init of the implementation
  // This can only be done after the CustomActor connection has been made...
  magnifier->Initialize();

  return handle;
}

Magnifier::Magnifier()
: Toolkit::ControlImpl(ControlBehaviour(CONTROL_BEHAVIOUR_DEFAULT)),
  mDefaultCameraDistance(1000.f),
  mActorSize(Vector3::ZERO),
  mMagnificationFactor(1.0f)
{
}

void Magnifier::SetSourceActor(Actor actor)
{
  mTaskSourceActor = actor;
  if(mTask)
  {
    mTask.SetSourceActor(actor);
  }
}

Magnifier::~Magnifier()
{
}

void Magnifier::OnInitialize()
{
  Actor self = Self();

  // NOTE:
  // sourceActor is a dummy delegate actor that takes the source property position,
  // and generates a WORLD_POSITION, which is 1 frame behind the source property.
  // This way the constraints for determining the camera position (source) and those
  // for determining viewport position use the same 1 frame old values.
  // A simple i) CameraPos = f(B), ii) B = f(A) set of constraints wont suffice as
  // although CameraPos will use B, which is A's previous value. The constraint will
  // not realise that B is still dirty as far as constraint (i) is concerned.
  // Perhaps this is a bug in the way the constraint system factors into what is dirty
  // and what is not.
  mSourceActor = Actor::New();
  mSourceActor.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER);
  Constraint constraint = Constraint::New<Vector3>(mSourceActor, Actor::Property::POSITION, EqualToConstraint());
  constraint.AddSource(Source(self, Demo::Magnifier::Property::SOURCE_POSITION));
  constraint.Apply();

  self.SetProperty(Toolkit::DevelControl::Property::ACCESSIBILITY_ROLE, Dali::Toolkit::Accessibility::Role::CONTAINER);
}

void Magnifier::OnSceneConnection(int depth)
{
  Actor self = Self();

  mWindow = Window::Get(self);

  PositionSize winSize = mWindow.GetPositionSize();
  Vector2      windowSize(winSize.width, winSize.height);

  mWindow.Add(mSourceActor);

  // create the render task this will render content on top of everything
  // based on camera source position.
  InitializeRenderTask();

  // set up some constraints to:
  // i) reposition (dest) frame actor based on magnifier actor's world position (this is 1 frame delayed)
  // ii) reposition and resize (dest) the render task's viewport based on magnifier actor's world position (1 frame delayed) & size.
  // iii) reposition (source) camera actor based on magnifier source actor's world position (this is 1 frame delayed)

  // Apply constraint to camera's position
  // Position our camera at the same distance from its target as the default camera is.
  // The camera position doesn't affect how we render, just what we render (due to near and far clip planes)
  // NOTE: We can't interrogate the default camera's position as it is not known initially (takes 1 frame
  // for value to update).
  // But we can determine the initial position using the same formula:
  // distance = scene.height * 0.5 / tan(FOV * 0.5)

  RenderTaskList taskList   = mWindow.GetRenderTaskList();
  RenderTask     renderTask = taskList.GetTask(0u);
  float          fov        = renderTask.GetCameraActor().GetFieldOfView();
  mDefaultCameraDistance    = (windowSize.height * 0.5f) / tanf(fov * 0.5f);

  // Use a 1 frame delayed source position to determine the camera actor's position.
  // This is necessary as the viewport is determined by the Magnifier's Actor's World position (which is computed
  // at the end of the update cycle i.e. after constraints have been applied.)
  Constraint constraint = Constraint::New<Vector3>(mCameraActor, Actor::Property::POSITION, CameraActorPositionConstraint(windowSize, mDefaultCameraDistance));
  constraint.AddSource(Source(mSourceActor, Actor::Property::WORLD_POSITION));
  constraint.Apply();

  // Apply constraint to render-task viewport position
  constraint = Constraint::New<Vector2>(mTask, RenderTask::Property::VIEWPORT_POSITION, RenderTaskViewportPositionConstraint(windowSize));
  constraint.AddSource(Source(self, Actor::Property::WORLD_POSITION));
  constraint.AddSource(Source(self, Actor::Property::SIZE));
  constraint.AddSource(Source(self, Actor::Property::WORLD_SCALE));
  constraint.Apply();

  // Apply constraint to render-task viewport size
  constraint = Constraint::New<Vector2>(mTask, RenderTask::Property::VIEWPORT_SIZE, RenderTaskViewportSizeConstraint());
  constraint.AddSource(Source(self, Actor::Property::SIZE));
  constraint.AddSource(Source(self, Actor::Property::WORLD_SCALE));
  constraint.Apply();

  Toolkit::ControlImpl::OnSceneConnection(depth);
}

void Magnifier::OnSceneDisconnection()
{
  if(mWindow)
  {
    RenderTaskList taskList = mWindow.GetRenderTaskList();
    if(mTask)
    {
      taskList.RemoveTask(mTask);
      mTask.Reset();
    }
    if(mCameraActor)
    {
      mWindow.Remove(mCameraActor);
      mCameraActor.Reset();
    }
    mWindow.Remove(mSourceActor);
    mWindow.Reset();
  }

  Toolkit::ControlImpl::OnSceneDisconnection();
}

void Magnifier::InitializeRenderTask()
{
  RenderTaskList taskList = mWindow.GetRenderTaskList();

  mTask = taskList.CreateTask();
  mTask.SetInputEnabled(false);
  mTask.SetClearColor(Vector4(0.5f, 0.5f, 0.5f, 1.0f));
  mTask.SetClearEnabled(true);

  mCameraActor = CameraActor::New();
  mCameraActor.SetType(Camera::FREE_LOOK);

  mWindow.Add(mCameraActor);
  mTask.SetCameraActor(mCameraActor);

  if(mTaskSourceActor)
  {
    mTask.SetSourceActor(mTaskSourceActor);
  }

  SetFrameVisibility(true);
}

bool Magnifier::GetFrameVisibility() const
{
  return static_cast<bool>(mFrame);
}

void Magnifier::SetFrameVisibility(bool visible)
{
  if(visible && !mFrame)
  {
    Actor self(Self());

    mFrame = Actor::New();
    mFrame.SetProperty(Actor::Property::INHERIT_POSITION, false);
    mFrame.SetProperty(Actor::Property::INHERIT_SCALE, true);
    mFrame.SetResizePolicy(ResizePolicy::SIZE_FIXED_OFFSET_FROM_PARENT, Dimension::ALL_DIMENSIONS);
    mFrame.SetProperty(DevelActor::Property::SIZE_MODE_FACTOR, Vector3(IMAGE_BORDER_INDENT * 2.f - 2.f, IMAGE_BORDER_INDENT * 2.f - 2.f, 0.0f));

    Toolkit::VisualFactory visualFactory = Toolkit::VisualFactory::Get();

    Property::Map map;
    map[Toolkit::Visual::Property::TYPE]        = Toolkit::Visual::BORDER;
    map[Toolkit::BorderVisual::Property::COLOR] = Color::WHITE;
    map[Toolkit::BorderVisual::Property::SIZE]  = IMAGE_BORDER_INDENT;
    Toolkit::Visual::Base borderVisual          = visualFactory.CreateVisual(map);
    borderVisual.SetOnScene(mFrame);

    Constraint constraint = Constraint::New<Vector3>(mFrame, Actor::Property::POSITION, EqualToConstraint());
    constraint.AddSource(ParentSource(Actor::Property::WORLD_POSITION));
    constraint.Apply();

    self.Add(mFrame);
  }
  else if(!visible && mFrame)
  {
    UnparentAndReset(mFrame);
  }
}

void Magnifier::OnSizeSet(const Vector3& targetSize)
{
  // TODO: Once Camera/CameraActor properties function as proper animatable properties
  // this code can disappear.
  // whenever the size of the magnifier changes, the field of view needs to change
  // to compensate for the new size of the viewport. this cannot be done within
  // a constraint yet as Camera/CameraActor properties are not animatable/constrainable.
  mActorSize = targetSize;
  Update();

  Toolkit::ControlImpl::OnSizeSet(targetSize);
}

float Magnifier::GetMagnificationFactor() const
{
  return mMagnificationFactor;
}

void Magnifier::SetMagnificationFactor(float value)
{
  mMagnificationFactor = value;
  Update();
}

void Magnifier::Update()
{
  if(!mCameraActor)
  {
    return;
  }

  // TODO: Make Camera settings (fieldofview/aspectratio) as animatable constraints.

  // should be updated when:
  // Magnifier's world size/scale changes.
  Actor   self(Self());
  Vector3 worldSize = mActorSize * self.GetCurrentProperty<Vector3>(Actor::Property::WORLD_SCALE);

  // Adjust field of view to scale content

  // size.height / 2
  // |------/
  // |d    /
  // |i   /
  // |s  /
  // |t /
  // |./
  // |/ <--- fov/2 radians.
  //
  DALI_ASSERT_ALWAYS(mDefaultCameraDistance > 0.0f && "Default camera distance should be bigger than zero.");
  DALI_ASSERT_ALWAYS(mMagnificationFactor > 0.0f && "Magnification factor should be bigger than zero.");
  const float fov = atanf(0.5f * worldSize.height / mDefaultCameraDistance / mMagnificationFactor) * 2.0f;
  mCameraActor.SetFieldOfView(fov);

  // Adjust aspect ratio to compensate for rectangular viewports.
  mCameraActor.SetAspectRatio(worldSize.width / worldSize.height);
}

void Magnifier::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  Demo::Magnifier magnifier = Demo::Magnifier::DownCast(Dali::BaseHandle(object));

  if(magnifier)
  {
    Magnifier& magnifierImpl(GetImpl(magnifier));
    switch(index)
    {
      case Demo::Magnifier::Property::FRAME_VISIBILITY:
      {
        magnifierImpl.SetFrameVisibility(value.Get<bool>());
        break;
      }
      case Demo::Magnifier::Property::MAGNIFICATION_FACTOR:
      {
        magnifierImpl.SetMagnificationFactor(value.Get<float>());
        break;
      }
    }
  }
}

Property::Value Magnifier::GetProperty(BaseObject* object, Property::Index index)
{
  Property::Value value;

  Demo::Magnifier magnifier = Demo::Magnifier::DownCast(Dali::BaseHandle(object));

  if(magnifier)
  {
    Magnifier& magnifierImpl(GetImpl(magnifier));
    switch(index)
    {
      case Demo::Magnifier::Property::FRAME_VISIBILITY:
      {
        value = magnifierImpl.GetFrameVisibility();
        break;
      }
      case Demo::Magnifier::Property::MAGNIFICATION_FACTOR:
      {
        value = magnifierImpl.GetMagnificationFactor();
        break;
      }
    }
  }

  return value;
}

} // namespace Dali::Demo::Internal
