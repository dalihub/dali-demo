/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

#include <dali-toolkit/dali-toolkit.h>
#include <dali/integration-api/debug.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali-toolkit/devel-api/drag-drop-detector/drag-and-drop-detector.h>

using namespace Dali;
using Dali::Toolkit::TextLabel;
using namespace Dali::Toolkit;

#ifdef ANDROID
namespace DragAndDropExample
{
#endif

namespace
{
const unsigned int TEXT_LABEL_NUM = 8;
const float TEXT_LABEL_POSITION_X = 100.0f;
const float TEXT_LABEL_POSITION_START_Y = 50.0f;
const float TEXT_LABEL_WIDTH = 250.0f;
const float TEXT_LABEL_HEIGHT = 70.0f;

#if defined(DEBUG_ENABLED)
  Debug::Filter* gDragAndDropFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_DRAG_AND_DROP_EXAMPLE");
#endif
}

//This example shows how to use drag and drop function by several simple TextActors
class DragAndDropExample : public ConnectionTracker
{
public:

  DragAndDropExample( Application& application )
  : mApplication( application ),
    mDragIndex(-1),
    mDragRealIndex(-1),
    mDroppedFinished(false)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &DragAndDropExample::Create );
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    mDragAndDropDetector = Dali::Toolkit::DragAndDropDetector::New();

    // Respond to key events
    stage.KeyEventSignal().Connect( this, &DragAndDropExample::OnKeyEvent );

    TextLabel hintText = TextLabel::New("please drag one textlabel, move and drop on other textlabel");
    hintText.SetPosition(0.0f, 700.0f);
    hintText.SetParentOrigin(ParentOrigin::TOP_LEFT);
    hintText.SetAnchorPoint(AnchorPoint::TOP_LEFT);
    hintText.SetProperty(TextLabel::Property::MULTI_LINE, true);
    stage.Add(hintText);

    Vector4 TEXT_LABEL_COLOR[TEXT_LABEL_NUM] =
    {
      Color::MAGENTA,
      Color::YELLOW,
      Color::CYAN,
      Color::BLUE,
      Color::MAGENTA,
      Color::YELLOW,
      Color::CYAN,
      Color::BLUE
    };

    for(unsigned int i = 0 ; i < TEXT_LABEL_NUM; i++)
    {
      std::string str = "textlabel ";
      mTextLabel[i] = TextLabel::New(str + std::to_string(i));
      mTextLabel[i].SetParentOrigin(ParentOrigin::TOP_LEFT);
      mTextLabel[i].SetAnchorPoint(AnchorPoint::TOP_LEFT);
      mTextLabel[i].SetName("textlabel " + std::to_string(i));
      mTextLabel[i].SetLeaveRequired(true);
      mTextLabel[i].SetProperty(TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER");
      mTextLabel[i].SetProperty(TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER");
      mTextLabel[i].SetBackgroundColor(TEXT_LABEL_COLOR[i]);

      mTextLabel[i].SetSize(TEXT_LABEL_WIDTH, TEXT_LABEL_HEIGHT);
      mTextLabel[i].SetPosition(TEXT_LABEL_POSITION_X, TEXT_LABEL_POSITION_START_Y + TEXT_LABEL_HEIGHT * i);
      mDragAndDropDetector.Attach(mTextLabel[i]);

      mRect[i] = Rect<float>(TEXT_LABEL_POSITION_X, TEXT_LABEL_POSITION_START_Y + TEXT_LABEL_HEIGHT * i, TEXT_LABEL_WIDTH, TEXT_LABEL_HEIGHT);
      mOrder[i] = i;

      stage.Add(mTextLabel[i]);
    }

    mDragAndDropDetector.StartedSignal().Connect(this, &DragAndDropExample::OnStart);
    mDragAndDropDetector.EnteredSignal().Connect(this, &DragAndDropExample::OnEnter);
    mDragAndDropDetector.ExitedSignal().Connect(this, &DragAndDropExample::OnExit);
    mDragAndDropDetector.MovedSignal().Connect(this, &DragAndDropExample::OnMoved);
    mDragAndDropDetector.DroppedSignal().Connect(this, &DragAndDropExample::OnDropped);
    mDragAndDropDetector.EndedSignal().Connect(this, &DragAndDropExample::OnEnd);
  }

  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if ( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

  void OnStart(Control control, Dali::Toolkit::DragAndDropDetector detector)
  {
    DALI_LOG_INFO(gDragAndDropFilter, Debug::General, "---OnStart---\n");
    DALI_LOG_INFO(gDragAndDropFilter, Debug::General, "---control name is %s---\n", control.GetName().c_str());

    control.SetOpacity(0.1f);
    Vector2 screenPos  = detector.GetCurrentScreenPosition();
    control.ScreenToLocal(mDragLocalPos.x, mDragLocalPos.y,screenPos.x, screenPos.y );
    Rect<float> targetRect(screenPos.x, screenPos.y, 0.0f, 0.0f);

    for(unsigned int i = 0; i < TEXT_LABEL_NUM; i++)
    {
      if(mRect[i].Contains(targetRect))
      {
        mDragIndex = i;
      }
    }

    mDragRealIndex = mOrder[mDragIndex];
  }

  void OnEnter(Control control, Dali::Toolkit::DragAndDropDetector detector)
  {
    DALI_LOG_INFO(gDragAndDropFilter, Debug::General, "---OnEnter---\n");
    DALI_LOG_INFO(gDragAndDropFilter, Debug::General, "---control name is %s---\n", control.GetName().c_str());
  }

  void OnExit(Control control, Dali::Toolkit::DragAndDropDetector detector)
  {
    DALI_LOG_INFO(gDragAndDropFilter, Debug::General, "---OnExit---\n");
    DALI_LOG_INFO(gDragAndDropFilter, Debug::General, "---control name is %s---\n", control.GetName().c_str());
  }

  void OnMoved(Control control, Dali::Toolkit::DragAndDropDetector detector)
  {
    DALI_LOG_INFO(gDragAndDropFilter, Debug::Verbose, "---OnMoved---\n");
    DALI_LOG_INFO(gDragAndDropFilter, Debug::Verbose, "---control name is %s---\n", control.GetName().c_str());
    DALI_LOG_INFO(gDragAndDropFilter, Debug::Verbose, "---coordinate is (%f, %f)---\n", detector.GetCurrentScreenPosition().x, detector.GetCurrentScreenPosition().y);
  }

  void OnDropped(Control control, Dali::Toolkit::DragAndDropDetector detector)
  {
    DALI_LOG_INFO(gDragAndDropFilter, Debug::General, "---OnDropped---\n");
    DALI_LOG_INFO(gDragAndDropFilter, Debug::General, "---control name is %s---\n", control.GetName().c_str());

    Vector2 screenPos  = detector.GetCurrentScreenPosition();
    Rect<float> targetRect(screenPos.x, screenPos.y, 0.0f, 0.0f);
    int droppedIndex = -1;
    for(unsigned int i = 0; i < TEXT_LABEL_NUM; i++)
    {
      if(mRect[i].Contains(targetRect))
      {
        droppedIndex = i;
      }
    }

    Animation mAnimation = Animation::New(0.5f);

    if(droppedIndex > mDragIndex)
    {
      for(int i = mDragIndex + 1; i <= droppedIndex; i++)
      {
        float y = mTextLabel[mOrder[i]].GetCurrentPosition().y;
        mAnimation.AnimateTo(Property(mTextLabel[mOrder[i]], Actor::Property::POSITION), Vector3(TEXT_LABEL_POSITION_X, y - TEXT_LABEL_HEIGHT, 0.0f), AlphaFunction::EASE_OUT);
        mAnimation.Play();
      }

      int tmpId = mOrder[mDragIndex];
      for(int i = mDragIndex; i < droppedIndex; i++)
      {
        mOrder[i] = mOrder[i+1];
      }

      mOrder[droppedIndex] = tmpId;
    }
    else if(droppedIndex < mDragIndex)
    {

      for(int i = mDragIndex - 1; i >= droppedIndex; i--)
      {
        float y = mTextLabel[mOrder[i]].GetCurrentPosition().y;
        mAnimation.AnimateTo(Property(mTextLabel[mOrder[i]], Actor::Property::POSITION), Vector3(TEXT_LABEL_POSITION_X, y + TEXT_LABEL_HEIGHT, 0.0f), AlphaFunction::EASE_OUT);
        mAnimation.Play();
      }

      int tmpId = mOrder[mDragIndex];
      for(int i = mDragIndex; i > droppedIndex; i--)
      {
        mOrder[i] = mOrder[i-1];
      }

      mOrder[droppedIndex] = tmpId;

    }


    Vector2 pos = detector.GetCurrentScreenPosition();
    Vector2 localPos;
    control.GetParent().ScreenToLocal(localPos.x, localPos.y, pos.x, pos.y);

    KeyFrames k0 = KeyFrames::New();
    k0.Add(0.0f, Vector3(localPos.x - mDragLocalPos.x, localPos.y - mDragLocalPos.y, 0.0f));
    k0.Add(1.0f, Vector3(control.GetCurrentPosition().x, control.GetCurrentPosition().y, 0.0f));

    KeyFrames k1 = KeyFrames::New();
    k1.Add(0.0f, 0.1f);
    k1.Add(1.0f, 1.0f);

    Animation dropAnimation = Animation::New(0.5f);
    dropAnimation.FinishedSignal().Connect(this, &DragAndDropExample::DropAnimationFinished);
    dropAnimation.AnimateBetween(Property(mTextLabel[mDragRealIndex], Actor::Property::POSITION), k0, AlphaFunction::EASE_OUT);
    dropAnimation.AnimateBetween(Property(mTextLabel[mDragRealIndex], DevelActor::Property::OPACITY), k1, AlphaFunction::EASE_OUT);
    dropAnimation.Play();

    mDroppedFinished = true;
  }

  void DropAnimationFinished(Animation& animation)
  {
    for(unsigned int i = 0 ; i < TEXT_LABEL_NUM; i++)
    {
      mDragAndDropDetector.Attach(mTextLabel[i]);
    }
  }

  void OnEnd(Control control, Dali::Toolkit::DragAndDropDetector detector)
  {
    DALI_LOG_INFO(gDragAndDropFilter, Debug::General, "---OnEnd---\n");
    DALI_LOG_INFO(gDragAndDropFilter, Debug::General, "---control name is %s---\n", control.GetName().c_str());

    control.SetOpacity(1.0f);

    if(mDroppedFinished)
    {
      mDragAndDropDetector.DetachAll();
    }

    mDroppedFinished = false;
  }

private:
  Application&  mApplication;
  Dali::Toolkit::DragAndDropDetector mDragAndDropDetector;

  TextLabel mTextLabel[TEXT_LABEL_NUM];
  Rect<float> mRect[TEXT_LABEL_NUM];

  int mOrder[TEXT_LABEL_NUM];
  int mDragIndex;
  int mDragRealIndex;

  Vector2 mDragLocalPos;

  bool mDroppedFinished;

};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  DragAndDropExample test( application );
  application.MainLoop();
  return 0;
}

#ifdef ANDROID
}
#endif
