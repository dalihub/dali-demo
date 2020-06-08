/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
 * @file simple-text-label-example.cpp
 * @brief Basic usage of SimpleTextLabel control
 */

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>

#include <dali/devel-api/text-abstraction/font-client.h>
#include <dali/devel-api/text-abstraction/bitmap-font.h>
#include <dali-toolkit/devel-api/text/bitmap-font.h>

using namespace Dali;
using namespace Dali::Toolkit;

/**
 * @brief The main class of the demo.
 */
class SimpleTextLabelExample : public ConnectionTracker
{
public:

  SimpleTextLabelExample( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &SimpleTextLabelExample::Create );
  }

  ~SimpleTextLabelExample()
  {
    // Nothing to do here.
  }

  /**
   * One-time setup in response to Application InitSignal.
   */
  void Create( Application& application )
  {
    Stage stage = Stage::GetCurrent();

    stage.KeyEventSignal().Connect(this, &SimpleTextLabelExample::OnKeyEvent);

    TextAbstraction::FontClient fontClient = TextAbstraction::FontClient::Get();

    DevelText::BitmapFontDescription fontDescription;
    fontDescription.name = "Digits";
    fontDescription.underlinePosition = 0.f;
    fontDescription.underlineThickness = 0.f;

    fontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0030.png", "0", 34.f, 0.f } );
    fontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0031.png", "1", 34.f, 0.f } );
    fontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0032.png", "2", 34.f, 0.f } );
    fontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0033.png", "3", 34.f, 0.f } );
    fontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0034.png", "4", 34.f, 0.f } );
    fontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0035.png", "5", 34.f, 0.f } );
    fontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0036.png", "6", 34.f, 0.f } );
    fontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0037.png", "7", 34.f, 0.f } );
    fontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0038.png", "8", 34.f, 0.f } );
    fontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0039.png", "9", 34.f, 0.f } );
    fontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u003a.png", ":", 34.f, 0.f } );


    DevelText::BitmapFontDescription colorFontDescription;
    colorFontDescription.name = "DigitsColor";
    colorFontDescription.underlinePosition = 0.f;
    colorFontDescription.underlineThickness = 0.f;
    colorFontDescription.isColorFont = true;

    colorFontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0030_color.png", "0", 34.f, 0.f } );
    colorFontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0031_color.png", "1", 34.f, 0.f } );
    colorFontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0032_color.png", "2", 34.f, 0.f } );
    colorFontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0033_color.png", "3", 34.f, 0.f } );
    colorFontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0034_color.png", "4", 34.f, 0.f } );
    colorFontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0035_color.png", "5", 34.f, 0.f } );
    colorFontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0036_color.png", "6", 34.f, 0.f } );
    colorFontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0037_color.png", "7", 34.f, 0.f } );
    colorFontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0038_color.png", "8", 34.f, 0.f } );
    colorFontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u0039_color.png", "9", 34.f, 0.f } );
    colorFontDescription.glyphs.push_back( { DEMO_IMAGE_DIR "u003a_color.png", ":", 34.f, 0.f } );

    TextAbstraction::BitmapFont bitmapFont;
    TextAbstraction::BitmapFont bitmapColorFont;

    DevelText::CreateBitmapFont( fontDescription, bitmapFont );
    DevelText::CreateBitmapFont( colorFontDescription, bitmapColorFont );

    fontClient.GetFontId( bitmapFont );
    fontClient.GetFontId( bitmapColorFont );

    TextLabel label01 = TextLabel::New();
    label01.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    label01.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    label01.SetProperty( Actor::Property::SIZE, Vector2( 400.f, 50.f ) );
    label01.SetProperty( Actor::Property::POSITION, Vector2( 0.f, -100.f ));
    label01.SetProperty( TextLabel::Property::MULTI_LINE, true );

    label01.SetProperty( TextLabel::Property::ENABLE_MARKUP, true );
    label01.SetProperty( TextLabel::Property::TEXT, "012<color 'value'='green'>345</color>6789:" );
    label01.SetProperty( TextLabel::Property::TEXT_COLOR, Color::RED );
    label01.SetProperty( TextLabel::Property::FONT_FAMILY, "Digits" );

    label01.SetBackgroundColor( Color::BLACK );

    stage.Add( label01 );


    TextLabel  label02 = TextLabel::New();
    label02.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    label02.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    label02.SetProperty( Actor::Property::SIZE, Vector2( 400.f, 50.f ) );
    label02.SetProperty( Actor::Property::POSITION, Vector2( 0.f, -50.f ));
    label02.SetProperty( TextLabel::Property::MULTI_LINE, true );

    label02.SetProperty( TextLabel::Property::TEXT, "0123456789:" );
    label02.SetProperty( TextLabel::Property::TEXT_COLOR, Color::WHITE );
    label02.SetProperty( TextLabel::Property::FONT_FAMILY, "DigitsColor" );

    label02.SetBackgroundColor( Color::BLACK );

    stage.Add( label02 );

    TextLabel  label03 = TextLabel::New();
    label03.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    label03.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    label03.SetProperty( Actor::Property::SIZE, Vector2( 400.f, 50.f ) );
    label03.SetProperty( Actor::Property::POSITION, Vector2( 0.f, 0.f ));
    label03.SetProperty( TextLabel::Property::MULTI_LINE, true );

    label03.SetProperty( TextLabel::Property::TEXT, "0123456789:" );
    label03.SetProperty( TextLabel::Property::FONT_FAMILY, "Digits" );

    label03.SetBackgroundColor( Color::WHITE );

    stage.Add( label03 );

    TextLabel  label04 = TextLabel::New();
    label04.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::CENTER );
    label04.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::CENTER );
    label04.SetProperty( Actor::Property::SIZE, Vector2( 400.f, 50.f ) );
    label04.SetProperty( Actor::Property::POSITION, Vector2( 0.f, 50.f ));
    label04.SetProperty( TextLabel::Property::MULTI_LINE, true );

    label04.SetProperty( TextLabel::Property::TEXT, "0123456789:" );
    label04.SetProperty( TextLabel::Property::FONT_FAMILY, "Digits" );
    label04.SetProperty( TextLabel::Property::TEXT_COLOR, Color::WHITE );

    label04.SetBackgroundColor( Color::BLACK );

    stage.Add( label04 );
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
};

void RunTest( Application& application )
{
  SimpleTextLabelExample test( application );

  application.MainLoop();
}

/** Entry point for Linux & Tizen applications */
int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
