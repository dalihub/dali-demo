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

#include <dali-toolkit/dali-toolkit.h>

using namespace Dali;
using namespace Dali::Toolkit;

const char* images[] = {
    DEMO_IMAGE_DIR "gallery-small-1.jpg",
    DEMO_IMAGE_DIR "gallery-small-2.jpg",
    DEMO_IMAGE_DIR "gallery-small-3.jpg",
    DEMO_IMAGE_DIR "gallery-small-4.jpg",
    DEMO_IMAGE_DIR "gallery-small-5.jpg"
};

// This verification example confirms drawing order is the same, with or without clipping enabled.
class ClippingDrawOrderVerification : public ConnectionTracker
{
public:

  ClippingDrawOrderVerification( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal.
    mApplication.InitSignal().Connect( this, &ClippingDrawOrderVerification::Create );
  }

  ~ClippingDrawOrderVerification()
  {
    // Nothing to do here.
  }

  // The Init signal is received once (only) during the Application lifetime.
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    // Connect to the stage's key signal to allow Back and Escape to exit.
    stage.KeyEventSignal().Connect( this, &ClippingDrawOrderVerification::OnKeyEvent );

    // Create the title label.
    TextLabel title = TextLabel::New( "Clipping draw order verification" );
    title.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    title.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    title.SetAnchorPoint( AnchorPoint::CENTER );
    title.SetParentOrigin( ParentOrigin::CENTER );

    // Create the description label.
    TextLabel description = TextLabel::New( "The bottom tree should have the same draw order as the top tree.\nThey should look identical except \"C\" is clipped on the bottom tree." );
    description.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    description.SetProperty( TextLabel::Property::MULTI_LINE, true );
    description.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
    description.SetParentOrigin( Vector3( 0.5f, 1.0f, 0.5f ) );
    stage.Add( description );

    /*
     * Create a 4-row TableView.
     * It will be segmented as follows:
     *
     *    +---------------+
     *    |     Title     |
     *    +---------------+
     *    |     Tree      |
     *    |    Without    |
     *    |   Clipping    |
     *    +---------------+
     *    |     Tree      |
     *    |     With      |
     *    |   Clipping    |
     *    +---------------+
     *    |  Explanation  |
     *    +---------------+
     */
    TableView view = TableView::New( 4, 1 );
    view.SetAnchorPoint( AnchorPoint::CENTER );
    view.SetParentOrigin( ParentOrigin::CENTER );
    view.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

    view.SetCellAlignment( Toolkit::TableView::CellPosition( 0, 0 ), HorizontalAlignment::CENTER, VerticalAlignment::CENTER );
    view.SetCellAlignment( Toolkit::TableView::CellPosition( 1, 0 ), HorizontalAlignment::CENTER, VerticalAlignment::CENTER );
    view.SetCellAlignment( Toolkit::TableView::CellPosition( 2, 0 ), HorizontalAlignment::CENTER, VerticalAlignment::CENTER );
    view.SetCellAlignment( Toolkit::TableView::CellPosition( 3, 0 ), HorizontalAlignment::CENTER, VerticalAlignment::CENTER );

    view.SetCellPadding( Vector2( 14.0f, 7.0f ) );

    view.SetRelativeWidth( 0u, 1.0f );

    view.SetFitHeight( 0u );
    view.SetRelativeHeight( 1u, 0.5f );
    view.SetRelativeHeight( 2u, 0.5f );
    view.SetFitHeight( 3u );

    // Add the title and description to the TableView.
    view.AddChild( title, TableView::CellPosition( 0u, 0u ) );
    view.AddChild( description, TableView::CellPosition( 3u, 0u ) );

    /*
       For each of the 2 tree views, we create a small tree of actors as follows:
       ( Note: Clipping is only enabled for B on the bottom tree ).

                             A
                            / \
       Clipping enabled -> B   D
                           |   |
                           C   E

       The correct draw order is "ABCDE" (the same as if clipping was not enabled).
    */
    const float treeYStart = 0.12f;
    const float depthGap = 0.35f;

    for( int tree = 0; tree < 2; ++tree )
    {
      Control container = Control::New();
      container.SetAnchorPoint( AnchorPoint::TOP_CENTER );
      container.SetParentOrigin( ParentOrigin::TOP_CENTER );
      container.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      Vector4 backgroundColor = tree == 0 ? Vector4( 0.77f, 1.0f, 0.77f, 1.0f ) : Vector4( 0.8f, 0.8f, 1.0f, 1.0f );
      container.SetProperty( Control::Property::BACKGROUND, backgroundColor );
      ImageView image[5];

      // Loop for each of the 5 images & labels.
      for( int i = 0; i < 5; ++i )
      {
        std::stringstream labelStream;
        labelStream << static_cast<char>( static_cast<char>( i ) + 'A' );
        TextLabel textLabel = TextLabel::New( labelStream.str() );
        textLabel.SetAnchorPoint( AnchorPoint::TOP_CENTER );

        image[i] = ImageView::New( images[i] );
        image[i].SetAnchorPoint( AnchorPoint::TOP_CENTER );

        // Calculate the relative positioning for the images and labels.
        float depth = static_cast<float>( i == 0 ? 0 : ( ( i - 1 ) % 2 ) + 1 );

        if( i == 0 )
        {
          image[i].SetParentOrigin( Vector3( 0.5f, treeYStart, 0.5f ) );
          textLabel.SetParentOrigin( Vector3( 1.0f, 0.05f * depth, 0.5f ) );
        }
        else
        {
          float b = i > 2 ? 1.0f : -1.0f;
          image[i].SetParentOrigin( Vector3( 0.5f + ( 0.2f * b ), depthGap, 0.5f ) );
          textLabel.SetParentOrigin( Vector3( 0.98f + 0.215f * b + ( 0.04f * b * depth ), treeYStart + 0.02f + ( 0.16f * depth ), 0.5f ) );
        }

        container.Add( textLabel );
      }

      // Create the title label.
      std::string treeText = tree == 0 ? "Without Clipping" : "With Clipping";
      TextLabel treeLabel = TextLabel::New( treeText );
      treeLabel.SetProperty( TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
      treeLabel.SetProperty( TextLabel::Property::VERTICAL_ALIGNMENT, "BOTTOM" );
      treeLabel.SetAnchorPoint( AnchorPoint::BOTTOM_CENTER );
      treeLabel.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
      container.Add( treeLabel );

      // Enable clipping for the 2nd tree.
      if( tree == 1 )
      {
        image[1].SetProperty( Actor::Property::CLIPPING_MODE, ClippingMode::CLIP_CHILDREN );
      }

      // Build the tree structure.
      container.Add( image[0] );

      image[0].Add( image[1] );
      image[1].Add( image[2] );

      image[0].Add( image[3] );
      image[3].Add( image[4] );

      // Add the finished tree to the TableView.
      view.AddChild( container, TableView::CellPosition( 1u + tree, 0u ) );
    }

    // Add the finished TableView to the stage.
    stage.Add( view );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchSignal().Connect( this, &ClippingDrawOrderVerification::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchData& touch )
  {
    // Quit the application.
    mApplication.Quit();
    return true;
  }

  /**
   * @brief Called when any key event is received
   *
   * Will use this to quit the application if Back or the Escape key is received
   * @param[in] event The key event information
   */
  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, DALI_KEY_ESCAPE) || IsKey( event, DALI_KEY_BACK ) )
      {
        mApplication.Quit();
      }
    }
  }

private:
  Application&  mApplication;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );
  ClippingDrawOrderVerification verification( application );
  application.MainLoop();
  return 0;
}
