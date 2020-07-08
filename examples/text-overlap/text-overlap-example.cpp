
#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/actors/actor-devel.h>
#include "text-overlap-example.h"

#include <iostream>

using namespace Dali;
using namespace Dali::Toolkit;

static const int NUMBER_OF_LABELS(2);



namespace Demo
{

TextOverlapController::TextOverlapController( Application& app )
: mApplication( app ),
  mTopmostLabel( 1 )
{
  app.InitSignal().Connect( this, &TextOverlapController::Create );
  app.TerminateSignal().Connect( this, &TextOverlapController::Destroy );
}

void TextOverlapController::Create( Application& app )
{
  Window window = app.GetWindow();
  window.KeyEventSignal().Connect( this, &TextOverlapController::OnKeyEvent );

  Vector2 windowSize = window.GetSize();

  mLabels[0] = TextLabel::New("Text Label 1");
  mLabels[1] = TextLabel::New("Text Label 2");

  mLabels[0].SetProperty( Dali::Actor::Property::NAME,"Label1");
  mLabels[1].SetProperty( Dali::Actor::Property::NAME,"Label2");

  mLabels[0].SetProperty( Dali::DevelActor::Property::SIBLING_ORDER, 1 );
  mLabels[1].SetProperty( Dali::DevelActor::Property::SIBLING_ORDER, 2 );

  mLabels[0].SetProperty( Control::Property::BACKGROUND, Color::RED );
  mLabels[1].SetProperty( Control::Property::BACKGROUND, Color::YELLOW );

  for(int i=0; i<NUMBER_OF_LABELS; ++i )
  {
    mLabels[i].SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    mLabels[i].SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT );
    mLabels[i].SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT );
    mLabels[i].SetProperty( Actor::Property::POSITION, Vector2( 0, (i*2+1) * windowSize.height * 0.25f ));
  }

  window.Add( mLabels[0] );
  window.Add( mLabels[1] );

  mSwapButton = PushButton::New();
  mSwapButton.SetProperty( Button::Property::LABEL, "Swap depth order");
  mSwapButton.SetProperty( Actor::Property::PARENT_ORIGIN, ParentOrigin::BOTTOM_CENTER );
  mSwapButton.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::BOTTOM_CENTER );
  mSwapButton.ClickedSignal().Connect( this, &TextOverlapController::OnClicked );
  mSwapButton.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  mSwapButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
  window.Add( mSwapButton );


  Layer rootLayer = window.GetRootLayer();
  rootLayer.SetProperty( Dali::Actor::Property::NAME,"RootLayer");

  mPanDetector = PanGestureDetector::New();
  mPanDetector.Attach( rootLayer );
  mPanDetector.AddAngle( Radian(-0.5f * Math::PI ));
  mPanDetector.AddAngle( Radian( 0.5f * Math::PI ));
  mPanDetector.DetectedSignal().Connect( this, &TextOverlapController::OnPan );
}

void TextOverlapController::OnPan( Actor actor, const PanGesture& gesture )
{
  if( ! mGrabbedActor || gesture.state == PanGesture::Started )
  {
    for( int i=0; i<NUMBER_OF_LABELS; ++i )
    {
      Vector3 position = mLabels[i].GetCurrentProperty< Vector3 >( Actor::Property::POSITION );
      Vector3 size = mLabels[i].GetCurrentProperty< Vector3 >( Actor::Property::SIZE );
      if( gesture.position.y > position.y - size.y * 0.5f &&
          gesture.position.y <= position.y + size.y * 0.5f )
      {
        mGrabbedActor = mLabels[i];
        break;
      }
    }
  }
  else if( mGrabbedActor && gesture.state == PanGesture::Continuing )
  {
    Vector2 windowSize = mApplication.GetWindow().GetSize();
    Vector3 size = mGrabbedActor.GetCurrentProperty< Vector3 >( Actor::Property::SIZE );
    float y = Clamp( gesture.position.y, size.y * 0.5f, windowSize.y - size.y*0.5f );
    mGrabbedActor.SetProperty( Actor::Property::POSITION, Vector2( 0, y ));
  }
  else
  {
    mGrabbedActor.Reset();
  }
}

void TextOverlapController::Destroy( Application& app )
{
  mPanDetector.DetachAll();
  UnparentAndReset(mLabels[0]);
  UnparentAndReset(mLabels[1]);
  mGrabbedActor.Reset();
}

bool TextOverlapController::OnClicked( Button button )
{
  mTopmostLabel = 1-mTopmostLabel; // toggles between 0 and 1
  mLabels[mTopmostLabel].RaiseToTop();
  return false;
}


void TextOverlapController::OnKeyEvent( const KeyEvent& keyEvent )
{
  if( keyEvent.state == KeyEvent::Down &&
      ( IsKey( keyEvent, DALI_KEY_BACK ) ||
        IsKey( keyEvent, DALI_KEY_ESCAPE ) ) )
  {
    mApplication.Quit();
  }
  else
  {
    Dali::Layer l = mApplication.GetWindow().GetRootLayer();
    int so = l.GetProperty<int>(Dali::DevelActor::Property::SIBLING_ORDER);
    l.SetProperty(Dali::DevelActor::Property::SIBLING_ORDER, so+1);
  }
}


} // namespace Demo

int DALI_EXPORT_API main( int argc, char** argv )
{
  {
    Application app = Application::New( &argc, &argv );
    Demo::TextOverlapController controller( app );
    app.MainLoop();
  }
  exit( 0 );
}
