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
#include "dali-toolkit/devel-api/controls/web-view/web-view.h"
#include <dali/integration-api/debug.h>
#include <unistd.h>

using namespace Dali;

namespace{

}

class WebViewController : public ConnectionTracker
{
public:

  WebViewController( Application& application )
  : mApplication( application )
  , mUrlPointer( 0 )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &WebViewController::Create );
  }

  ~WebViewController()
  {
    // Nothing to do here;
  }

  const char* GetNextUrl()
  {
    static const unsigned int kUrlSize = 3;
    static const char* kUrls[kUrlSize] = {
      "https://webkit.org/blog-files/3d-transforms/poster-circle.html",
      "https://www.amazon.com",
      "https://www.google.com"
    };
    mUrlPointer %= kUrlSize;
    return kUrls[mUrlPointer++];
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Vector4(0.2, 0.6, 1, 1) );

    float width = stage.GetSize().width;
    float height = stage.GetSize().height;
    float fontSize = width * 0.02f;

    mWebView = Toolkit::WebView::New( "ko-KR", "Asia/Seoul" );
    mWebView.SetParentOrigin( Dali::ParentOrigin::CENTER );
    mWebView.SetAnchorPoint( Dali::AnchorPoint::CENTER );
    mWebView.SetPosition( 0, 0 );
    mWebView.SetSize( width, height );
    mWebView.PageLoadStartedSignal().Connect( this, &WebViewController::OnPageLoadStarted );
    mWebView.PageLoadFinishedSignal().Connect( this, &WebViewController::OnPageLoadFinished );

    std::string url = GetNextUrl();
    mWebView.LoadUrl( url );
    stage.Add(mWebView);

    mAddressLabel = Toolkit::TextLabel::New( url );
    mAddressLabel.SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mAddressLabel.SetProperty( Toolkit::TextLabel::Property::POINT_SIZE, fontSize );
    mAddressLabel.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
    mAddressLabel.SetBackgroundColor( Vector4( 0, 0, 0, 0.5f ) );
    mAddressLabel.TouchedSignal().Connect( this, &WebViewController::OnTouchText );
    stage.Add( mAddressLabel );

    // Respond to key events
    stage.KeyEventSignal().Connect( this, &WebViewController::OnKeyEvent );
    Toolkit::KeyboardFocusManager::Get().SetCurrentFocusActor( mWebView );
  }

  void OnPageLoadStarted( Toolkit::WebView view, const std::string& url )
  {
    mAddressLabel.SetProperty( Toolkit::TextLabel::Property::TEXT, "Loading" );
  }

  void OnPageLoadFinished( Toolkit::WebView view, const std::string& url )
  {
    mAddressLabel.SetProperty( Toolkit::TextLabel::Property::TEXT, url.c_str() );
  }

  bool OnTouchText( Actor actor, const TouchEvent& event )
  {
    if ( event.GetPoint( 0 ).state == TouchPoint::Up )
    {
      std::string url = GetNextUrl();
      mAddressLabel.SetProperty(Toolkit::TextLabel::Property::TEXT, "Waiting" );
      mWebView.LoadUrl( url );
    }

    return true;
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

private:
  Application&  mApplication;
  Toolkit::WebView mWebView;
  Toolkit::TextLabel mAddressLabel;
  unsigned int mUrlPointer;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  WebViewController test( application );
  application.MainLoop();
  return 0;
}

#ifdef ANDROID
}
#endif
