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
 * @file text-fonts-example.cpp
 * @brief Example of various TextLabel each with different font set ups,
 *        enables Testing of Font when the system font changes.
          The first label is free, with no font family set, it could use the default system font and change as it changes.
          The second label has it's font family set via the demo json file. It should not change when the system font changes.
          The third label has it's font family set in code via SetProperty. It also should not change when the system font changes.
          The forth label is not shown until the button along the bottom is pressed, it has no font set so the newly created label should use the system font,
          Pressing the button again resets and unparents that button and then re-adds it.
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <iostream>

// INTERNAL INCLUDES
#include "shared/multi-language-strings.h"
#include "shared/view.h"

using namespace Dali;
using namespace Dali::Toolkit;
using namespace MultiLanguageStrings;

namespace
{
  const char* const LABEL_TEXT = "A Quick Fox";
  const char* const LABEL_TEXT_MIXED = "Fox 구미호";
  const char* const LABEL_TEXT_KOREAN = "구미호";
}

/**
 * @brief The main class of the demo.
 */
class TextFontsExample : public ConnectionTracker
{
public:

  TextFontsExample( Application& application )
  : mApplication( application ),
    mToggle(true)
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &TextFontsExample::Create );
  }

  ~TextFontsExample()
  {
    // Nothing to do here.
  }

  void CreateTextLabel( TextLabel& textLabel, std::string textString, const Vector4& color, bool infoLabel=false )
  {
    textLabel = TextLabel::New( textString );
    textLabel.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    textLabel.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
    textLabel.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::HEIGHT );
    textLabel.SetProperty( TextLabel::Property::MULTI_LINE, true );
    if ( infoLabel )
    {
      textLabel.SetProperty( TextLabel::Property::TEXT_COLOR, Color::WHITE );
      textLabel.SetProperty( TextLabel::Property::POINT_SIZE, 12.0f );
      textLabel.SetProperty( TextLabel::Property::FONT_FAMILY, "SamsungOneUI" );
    }
    else
    {
      textLabel.SetProperty( TextLabel::Property::SHADOW_OFFSET, Vector2( 0.3f, 0.3f ) );
      textLabel.SetProperty( TextLabel::Property::SHADOW_COLOR, Color::BLACK );
      textLabel.SetProperty( TextLabel::Property::TEXT_COLOR, Color::BLUE );
    }
    textLabel.SetBackgroundColor( color );
  }

  void CreateContainer( Control& container, const Vector2 size )
  {
    container = Control::New();
    container.SetSize( size );
    container.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    container.SetDrawMode( DrawMode::OVERLAY_2D );
  }

  void CreateFolderButton( PushButton& button )
  {
    button = PushButton::New();
    button.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    button.SetResizePolicy( ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS );
    button.SetSize( 50.0f, 50.0f );
  }

  bool OnButtonClicked( Toolkit::Button button )
  {
    if ( mLabel4 )
    {
      UnparentAndReset( mLabel4 );
    }

    if ( !mContainer4 )
    {
      CreateContainer ( mContainer4 , mLayoutSize);
      Stage stage = Stage::GetCurrent();
      Vector2 stageSize = stage.GetSize();
      mContainer4.SetPosition( 0, stageSize.height*0.25f*3 );
      stage.Add( mContainer4 );
      // Info
      CreateContainer ( mContainer4Info , mLayoutSize );
      mContainer4Info.SetParentOrigin( ParentOrigin::TOP_RIGHT );
      mContainer4.Add( mContainer4Info );
      CreateTextLabel ( mLabel4Info, "system free", Color::BLACK, true  );
      mContainer4Info.Add ( mLabel4Info );
    }

    if ( mToggle )
    {
      CreateTextLabel ( mLabel4, LABEL_TEXT_KOREAN, Color::WHITE );
      mToggle = false;
    }
    else
    {
      CreateTextLabel ( mLabel4, LABEL_TEXT_MIXED, Color::WHITE );
      mToggle = true;
    }

    mContainer4.Add( mLabel4 );

    return true;
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();
    Vector2 stageSize = stage.GetSize();

    stage.KeyEventSignal().Connect(this, &TextFontsExample::OnKeyEvent);

    CreateFolderButton ( mButton );
    mButton.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
    mButton.ClickedSignal().Connect( this, &TextFontsExample::OnButtonClicked );
    stage.Add( mButton );

    mLayoutSize = Vector2( stageSize.width*0.5f, stageSize.height*0.10f );
    CreateContainer ( mContainer , mLayoutSize);
    CreateContainer ( mContainer2 , mLayoutSize );
    CreateContainer ( mContainer3 , mLayoutSize );

    // Info about Text Label and if font should be fixed or free to change with system
    CreateContainer ( mContainerInfo , mLayoutSize );
    CreateContainer ( mContainer2Info , mLayoutSize );
    CreateContainer ( mContainer3Info , mLayoutSize );
    mContainerInfo.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    mContainer2Info.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    mContainer3Info.SetParentOrigin( ParentOrigin::TOP_RIGHT );
    mContainer.Add( mContainerInfo );
    mContainer2.Add( mContainer2Info );
    mContainer3.Add( mContainer3Info );
    CreateTextLabel ( mLabelInfo, "system free", Color::BLACK, true  );
    CreateTextLabel ( mLabel2Info, "json fixed", Color::BLACK, true  );
    CreateTextLabel ( mLabel3Info, "SetProp fixed", Color::BLACK, true  );
    mContainerInfo.Add( mLabelInfo );
    mContainer2Info.Add( mLabel2Info );
    mContainer3Info.Add( mLabel3Info );

    stage.Add( mContainer );
    stage.Add( mContainer2 );
    stage.Add( mContainer3 );

    CreateTextLabel ( mLabel, LABEL_TEXT, Color::WHITE  );

    CreateTextLabel ( mLabel2, LABEL_TEXT, Color::WHITE  );
    mLabel2.SetStyleName("TextLabelRosemary");

    CreateTextLabel ( mLabel3, LABEL_TEXT, Color::WHITE  );
    mLabel3.SetProperty( TextLabel::Property::FONT_FAMILY, "SamsungOneUI" );

    mContainer.SetPosition( 0, 0 );
    mContainer2.SetPosition( 0, stageSize.height*0.25f );
    mContainer3.SetPosition( 0, stageSize.height*0.25f*2 );

    mContainer.Add( mLabel );
    mContainer2.Add( mLabel2 );
    mContainer3.Add( mLabel3 );
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

  Application& mApplication;

  PushButton mButton;

  TextLabel mLabel;
  TextLabel mLabel2;
  TextLabel mLabel3;

  TextLabel mLabel4;

  Control mContainer;
  Control mContainer2;
  Control mContainer3;
  Control mContainer4;

  Control mContainerInfo;
  Control mContainer2Info;
  Control mContainer3Info;
  Control mContainer4Info;

  TextLabel mLabelInfo;
  TextLabel mLabel2Info;
  TextLabel mLabel3Info;
  TextLabel mLabel4Info;

  Vector2 mLayoutSize;

  bool mToggle;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );
  TextFontsExample test( application );
  application.MainLoop();
  return 0;
}
