/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
using Dali::Toolkit::TextLabel;

const char* IMAGE_FILENAME_ETC         = DALI_IMAGE_DIR "tx-etc1.ktx";
const char* IMAGE_FILENAME_ASTC_LINEAR = DALI_IMAGE_DIR "tx-astc-4x4-linear.ktx";
const char* IMAGE_FILENAME_ASTC_SRGB   = DALI_IMAGE_DIR "tx-astc-4x4-srgb.ktx";

/**
 * @brief This example shows 3 images, each of a different compressed texture type.
 * If built and run on a OpenGL ES 3.1 compatable target, then all 3 images will display.
 * Otherwise, the top image will display and the other 2 will appear as black squares.
 */
class CompressedTextureFormatsController : public ConnectionTracker
{
public:

  CompressedTextureFormatsController( Application& application )
  : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &CompressedTextureFormatsController::Create );
  }

  ~CompressedTextureFormatsController()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    // Setup a TableView to hold a grid of images and labels.
    Toolkit::TableView table = Toolkit::TableView::New( 3u, 2u );
    table.SetAnchorPoint( AnchorPoint::CENTER );
    table.SetParentOrigin( ParentOrigin::CENTER );
    table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    table.SetFitHeight( 0u );
    table.SetFitHeight( 1u );
    table.SetFitHeight( 2u );
    table.SetRelativeWidth( 0u, 0.5f );
    table.SetRelativeWidth( 1u, 0.5f );

    // Add text labels.
    TextLabel textLabel = TextLabel::New( "ETC1:" );
    textLabel.SetAnchorPoint( AnchorPoint::CENTER );
    textLabel.SetParentOrigin( ParentOrigin::CENTER );
    table.AddChild( textLabel, Toolkit::TableView::CellPosition( 0u, 0u ) );
    table.SetCellAlignment( Toolkit::TableView::CellPosition( 0u, 0u ), HorizontalAlignment::LEFT, VerticalAlignment::CENTER );

    textLabel = TextLabel::New( "ASTC 4x4 linear:" );
    textLabel.SetAnchorPoint( AnchorPoint::CENTER );
    textLabel.SetParentOrigin( ParentOrigin::CENTER );
    table.AddChild( textLabel, Toolkit::TableView::CellPosition( 1u, 0u ) );
    table.SetCellAlignment( Toolkit::TableView::CellPosition( 1u, 0u ), HorizontalAlignment::LEFT, VerticalAlignment::CENTER );

    textLabel = TextLabel::New( "ASTC 4x4 sRGB:" );
    textLabel.SetAnchorPoint( AnchorPoint::CENTER );
    textLabel.SetParentOrigin( ParentOrigin::CENTER );
    table.AddChild( textLabel, Toolkit::TableView::CellPosition( 2u, 0u ) );
    table.SetCellAlignment( Toolkit::TableView::CellPosition( 2u, 0u ), HorizontalAlignment::LEFT, VerticalAlignment::CENTER );

    // Add images.
    Toolkit::ImageView imageView = Toolkit::ImageView::New( ResourceImage::New( IMAGE_FILENAME_ETC ) );
    imageView.SetAnchorPoint( AnchorPoint::CENTER );
    imageView.SetParentOrigin( ParentOrigin::CENTER );
    table.AddChild( imageView, Toolkit::TableView::CellPosition( 0u, 1u ) );

    imageView = Toolkit::ImageView::New( ResourceImage::New( IMAGE_FILENAME_ASTC_LINEAR ) );
    imageView.SetAnchorPoint( AnchorPoint::CENTER );
    imageView.SetParentOrigin( ParentOrigin::CENTER );
    table.AddChild( imageView, Toolkit::TableView::CellPosition( 1u, 1u ) );

    imageView = Toolkit::ImageView::New( ResourceImage::New( IMAGE_FILENAME_ASTC_SRGB ) );
    imageView.SetAnchorPoint( AnchorPoint::CENTER );
    imageView.SetParentOrigin( ParentOrigin::CENTER );
    table.AddChild( imageView, Toolkit::TableView::CellPosition( 2u, 1u ) );

    stage.Add( table );

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchedSignal().Connect( this, &CompressedTextureFormatsController::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchEvent& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

private:
  Application&  mApplication;
};

void RunTest( Application& application )
{
  CompressedTextureFormatsController test( application );

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv );

  RunTest( application );

  return 0;
}
