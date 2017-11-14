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
 *
 */

/**
 * @file text-label-example.cpp
 * @brief Basic usage of TextLabel control
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>

// INTERNAL INCLUDES
#include "shared/multi-language-strings.h"
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace MultiLanguageStrings;

/**
 * @brief The main class of the demo.
 */
class TextLabelMultiLanguageExample : public ConnectionTracker
{
public:

  TextLabelMultiLanguageExample( Application& application )
  : mApplication( application ),
    mLastPoint( 0.f )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextLabelMultiLanguageExample::Create );
  }

  ~TextLabelMultiLanguageExample()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();

    stage.KeyEventSignal().Connect(this, &TextLabelMultiLanguageExample::OnKeyEvent);
    stage.SetBackgroundColor( Color::WHITE );

    mTableView = Toolkit::TableView::New( NUMBER_OF_LANGUAGES, 1 );
    mTableView.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    mTableView.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
    mTableView.SetParentOrigin( ParentOrigin::TOP_LEFT );
    mTableView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mTableView.TouchSignal().Connect( this, &TextLabelMultiLanguageExample::OnTouch );
    stage.Add( mTableView );

    for( unsigned int index = 0u; index < NUMBER_OF_LANGUAGES; ++index )
    {
      const Language& language = LANGUAGES[index];

      TextLabel label = TextLabel::New();
      label.SetProperty( TextLabel::Property::MULTI_LINE, true );

      const std::string text = language.languageName + " " + language.languageRomanName + " " + language.text;
      label.SetProperty( TextLabel::Property::TEXT, text );

      mTableView.SetFitHeight( index );
      mTableView.AddChild( label, Toolkit::TableView::CellPosition( index, 0 ) );
    }
  }

  bool OnTouch( Actor actor, const TouchData& event )
  {
    if( 1u == event.GetPointCount() )
    {
      const PointState::Type state = event.GetState( 0 );

      // Clamp to integer values; this is to reduce flicking due to pixel misalignment
      const float localPoint = static_cast<float>( static_cast<int>( event.GetLocalPosition( 0 ).y ) );

      if( PointState::DOWN == state )
      {
        mLastPoint = localPoint;
        mAnimation = Animation::New( 0.25f );
      }
      else if( PointState::MOTION == state )
      {
        if( mAnimation )
        {
          mAnimation.AnimateBy( Property(mTableView, Actor::Property::POSITION), Vector3( 0.f, localPoint - mLastPoint, 0.f ), AlphaFunction::LINEAR );
          mAnimation.Play();
          mLastPoint = localPoint;
        }
      }
    }

    return true;
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:

  Application&   mApplication;
  TableView      mTableView;
  Animation      mAnimation;
  float          mLastPoint;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );
  TextLabelMultiLanguageExample test( application );
  application.MainLoop();
  return 0;
}
