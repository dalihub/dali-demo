#pragma once

namespace Demo
{

class TextOverlapController : public Dali::ConnectionTracker
{
public:
  TextOverlapController( Dali::Application& app );

private:
  void Create( Dali::Application& app );
  void Create2();
  void Destroy( Dali::Application& app );
  void OnPan( Dali::Actor actor, const Dali::PanGesture& gesture );
  void OnKeyEvent( const Dali::KeyEvent& keyEvent );
  bool OnClicked( Dali::Toolkit::Button button );

private:
  Dali::Application& mApplication;
  Dali::Toolkit::TextLabel mLabels[2];
  Dali::PanGestureDetector mPanDetector;
  Dali::Actor mGrabbedActor;
  Dali::Toolkit::Button mSwapButton;
  int mTopmostLabel;
};

}
