
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
  Stage stage = Stage::GetCurrent();
  stage.KeyEventSignal().Connect( this, &TextOverlapController::OnKeyEvent );

  Vector2 stageSize = stage.GetSize();

  mLabels[0] = TextLabel::New("Text Label 1");
  mLabels[1] = TextLabel::New("Text Label 2");

  mLabels[0].SetName("Label1");
  mLabels[1].SetName("Label2");

  mLabels[0].SetProperty( DevelActor::Property::SIBLING_ORDER, 1 );
  mLabels[1].SetProperty( DevelActor::Property::SIBLING_ORDER, 2 );

  mLabels[0].SetProperty( Control::Property::BACKGROUND, Color::RED );
  mLabels[1].SetProperty( Control::Property::BACKGROUND, Color::YELLOW );

  for(int i=0; i<NUMBER_OF_LABELS; ++i )
  {
    mLabels[i].SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    mLabels[i].SetAnchorPoint( AnchorPoint::TOP_LEFT );
    mLabels[i].SetParentOrigin( ParentOrigin::TOP_LEFT );
    mLabels[i].SetPosition( 0, (i*2+1) * stageSize.height * 0.25f );
  }

  stage.Add( mLabels[0] );
  stage.Add( mLabels[1] );

  mSwapButton = PushButton::New();
  mSwapButton.SetProperty( Button::Property::LABEL, "Swap depth order");
  mSwapButton.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
  mSwapButton.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
  mSwapButton.ClickedSignal().Connect( this, &TextOverlapController::OnClicked );
  mSwapButton.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
  mSwapButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
  stage.Add( mSwapButton );


  Layer rootLayer = stage.GetRootLayer();
  rootLayer.SetName("RootLayer");

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
      Vector3 position = mLabels[i].GetCurrentPosition();
      Vector3 size = mLabels[i].GetCurrentSize();
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
    Vector2 stageSize = Stage::GetCurrent().GetSize();
    Vector3 size = mGrabbedActor.GetCurrentSize();
    float y = Clamp( gesture.position.y, size.y * 0.5f, stageSize.y - size.y*0.5f );
    mGrabbedActor.SetPosition( 0, y );
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
    Dali::Layer l = Dali::Stage::GetCurrent().GetRootLayer();
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
