#ifndef __DALI_DEMO_HELPER_VIEW_H__
#define __DALI_DEMO_HELPER_VIEW_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

namespace DemoHelper
{

/**
 * Provide a style for the view and its tool bar.
 */
struct ViewStyle
{
  ViewStyle( float toolBarButtonPercentage, float toolBarTitlePercentage, float toolBarHeight, float toolBarPadding )
  : mToolBarButtonPercentage( toolBarButtonPercentage ),
    mToolBarTitlePercentage( toolBarTitlePercentage ),
    mToolBarHeight( toolBarHeight ),
    mToolBarPadding( toolBarPadding )
  {}

  float mToolBarButtonPercentage; ///< The tool bar button width is a percentage of the tool bar width.
  float mToolBarTitlePercentage;  ///< The tool bar title width is a percentage of the tool bar width.
  float mToolBarHeight;           ///< The tool bar height (in pixels).
  float mToolBarPadding;          ///< The tool bar padding (in pixels)..
};

const ViewStyle DEFAULT_VIEW_STYLE( 0.1f, 0.7f, 80.f, 4.f );

const char*                   DEFAULT_TEXT_STYLE_FONT_FAMILY("HelveticaNue");
const char*                   DEFAULT_TEXT_STYLE_FONT_STYLE("Regular");
const Dali::PointSize         DEFAULT_TEXT_STYLE_POINT_SIZE( 8.0f );
const Dali::TextStyle::Weight DEFAULT_TEXT_STYLE_WEIGHT(Dali::TextStyle::EXTRALIGHT);
const Dali::Vector4           DEFAULT_TEXT_STYLE_COLOR(0.0f, 0.0f, 0.0f, 1.0f);

const Dali::Toolkit::Alignment::Padding DEFAULT_PLAY_PADDING(12.0f, 12.0f, 12.0f, 12.0f);
const Dali::Toolkit::Alignment::Padding DEFAULT_MODE_SWITCH_PADDING(8.0f, 8.0f, 8.0f, 8.0f);

static Dali::TextStyle defaultTextStyle;
static bool textStyleSet=false;

float ScalePointSize(int pointSize)
{
  Dali::Vector2 dpi = Dali::Stage::GetCurrent().GetDpi();
  float meanDpi = (dpi.height + dpi.width) * 0.5f;
  return pointSize * 220.0f / meanDpi;        // 220 is the default horizontal DPI defined in adaptor Application
}

Dali::TextStyle& GetDefaultTextStyle()
{
  if(!textStyleSet)
  {
    defaultTextStyle.SetFontName(DEFAULT_TEXT_STYLE_FONT_FAMILY);
    defaultTextStyle.SetFontStyle(DEFAULT_TEXT_STYLE_FONT_STYLE);
    defaultTextStyle.SetFontPointSize(Dali::PointSize(ScalePointSize(DEFAULT_TEXT_STYLE_POINT_SIZE)));
    defaultTextStyle.SetWeight(DEFAULT_TEXT_STYLE_WEIGHT);
    defaultTextStyle.SetTextColor(DEFAULT_TEXT_STYLE_COLOR);
    textStyleSet = true;
  }

  return defaultTextStyle;
}

Dali::Layer CreateToolbar( Dali::Toolkit::ToolBar& toolBar,
                           const std::string& toolbarImagePath,
                           const std::string& title,
                           const ViewStyle& style,
                           const Dali::TextStyle& textStyle )
{
  Dali::Layer toolBarLayer = Dali::Layer::New();
  toolBarLayer.SetName( "TOOLBAR_LAYER" );
  toolBarLayer.SetAnchorPoint( Dali::AnchorPoint::TOP_CENTER );
  toolBarLayer.SetParentOrigin( Dali::ParentOrigin::TOP_CENTER );
  toolBarLayer.SetSize( 0.0f, style.mToolBarHeight );
  toolBarLayer.SetResizePolicy( Dali::FILL_TO_PARENT, Dali::WIDTH );

  // Raise tool bar layer to the top.
  toolBarLayer.RaiseToTop();

  // Tool bar
  Dali::Image image = Dali::ResourceImage::New( toolbarImagePath );
  Dali::ImageActor toolBarBackground = Dali::ImageActor::New( image );
  toolBarBackground.SetName( "TOOLBAR_BACKGROUND" );
  toolBarBackground.SetResizePolicy( Dali::FILL_TO_PARENT, Dali::ALL_DIMENSIONS );
  toolBar = Dali::Toolkit::ToolBar::New();
  toolBar.SetName( "TOOLBAR" );
  toolBar.SetBackground( toolBarBackground );
  toolBar.SetParentOrigin( Dali::ParentOrigin::TOP_CENTER );
  toolBar.SetAnchorPoint( Dali::AnchorPoint::TOP_CENTER );
  toolBar.SetResizePolicy( Dali::FILL_TO_PARENT, Dali::ALL_DIMENSIONS );
  toolBarBackground.SetSortModifier(1.0f);

  // Add the tool bar to the too bar layer.
  toolBarLayer.Add( toolBar );

  Dali::Font font = Dali::Font::New();

  // Tool bar text.
  if( !title.empty() )
  {
    Dali::Toolkit::TextView titleActor = Dali::Toolkit::TextView::New();
    titleActor.SetName( "TOOLBAR_TITLE" );
    titleActor.SetText( title );
    titleActor.SetSize( font.MeasureText( title ) );
    titleActor.SetStyleToCurrentText(textStyle);

    // Add title to the tool bar.
    const float padding( style.mToolBarPadding );
    toolBar.AddControl( titleActor, style.mToolBarTitlePercentage, Dali::Toolkit::Alignment::HorizontalCenter, Dali::Toolkit::Alignment::Padding( padding, padding, padding, padding ) );
  }

  return toolBarLayer;
}

Dali::Layer CreateView( Dali::Application& application,
                        Dali::Toolkit::View& view,
                        Dali::Toolkit::ToolBar& toolBar,
                        const std::string& backgroundImagePath,
                        const std::string& toolbarImagePath,
                        const std::string& title,
                        const ViewStyle& style,
                        const Dali::TextStyle& textStyle )
{
  Dali::Stage stage = Dali::Stage::GetCurrent();

  // Create default View.
  view = Dali::Toolkit::View::New();
  view.SetResizePolicy( Dali::FILL_TO_PARENT, Dali::ALL_DIMENSIONS );

  // Add the view to the stage before setting the background.
  stage.Add( view );

  // Set background image, loading it at screen resolution:
  if ( !backgroundImagePath.empty() )
  {
    Dali::ImageAttributes backgroundAttributes;
    backgroundAttributes.SetSize( stage.GetSize() );
    backgroundAttributes.SetFilterMode( Dali::ImageAttributes::BoxThenLinear );
    backgroundAttributes.SetScalingMode( Dali::ImageAttributes::ScaleToFill );
    Dali::Image backgroundImage = Dali::ResourceImage::New( backgroundImagePath, backgroundAttributes );
    Dali::ImageActor backgroundImageActor = Dali::ImageActor::New( backgroundImage );
    view.SetBackground( backgroundImageActor );
  }

  // FIXME
  // Connects the orientation signal with the View::OrientationChanged method.
  //application.GetOrientation().ChangedSignal().Connect( &view, &Dali::Toolkit::View::OrientationChanged );

  // Create default ToolBar
  Dali::Layer toolBarLayer = CreateToolbar( toolBar, toolbarImagePath, title, style, textStyle );

  // Add tool bar layer to the view.
  view.AddContentLayer( toolBarLayer );

  // Create a content layer.
  Dali::Layer contentLayer = Dali::Layer::New();
  contentLayer.SetAnchorPoint( Dali::AnchorPoint::CENTER );
  contentLayer.SetParentOrigin( Dali::ParentOrigin::CENTER );
  contentLayer.SetResizePolicy( Dali::FILL_TO_PARENT, Dali::ALL_DIMENSIONS );
  view.AddContentLayer( contentLayer );
  contentLayer.LowerBelow( toolBarLayer );

  return contentLayer;
}

Dali::Layer CreateView( Dali::Application& application,
                        Dali::Toolkit::View& view,
                        Dali::Toolkit::ToolBar& toolBar,
                        const std::string& backgroundImagePath,
                        const std::string& toolbarImagePath,
                        const std::string& title,
                        const ViewStyle& style  = DEFAULT_VIEW_STYLE )
{
  return CreateView( application, view, toolBar, backgroundImagePath, toolbarImagePath, title, style,
                     GetDefaultTextStyle() );
}


} // DemoHelper

#endif // __DALI_DEMO_HELPER_VIEW_H__
