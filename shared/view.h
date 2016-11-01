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
#include <dali-toolkit/devel-api/controls/tool-bar/tool-bar.h>


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
const float                   DEFAULT_TEXT_STYLE_POINT_SIZE( 8.0f );

const Dali::Toolkit::Alignment::Padding DEFAULT_PLAY_PADDING(12.0f, 12.0f, 12.0f, 12.0f);
const Dali::Toolkit::Alignment::Padding DEFAULT_MODE_SWITCH_PADDING(8.0f, 8.0f, 8.0f, 8.0f);

float ScalePointSize(int pointSize)
{
  Dali::Vector2 dpi = Dali::Stage::GetCurrent().GetDpi();
  float meanDpi = (dpi.height + dpi.width) * 0.5f;
  return pointSize * 220.0f / meanDpi;        // 220 is the default horizontal DPI defined in adaptor Application
}

Dali::Layer CreateToolbar( Dali::Toolkit::ToolBar& toolBar,
                           const std::string& toolbarImagePath,
                           const std::string& title,
                           const ViewStyle& style )
{
  Dali::Stage stage = Dali::Stage::GetCurrent();

  Dali::Layer toolBarLayer = Dali::Layer::New();
  toolBarLayer.SetName( "TOOLBAR_LAYER" );
  toolBarLayer.SetAnchorPoint( Dali::AnchorPoint::TOP_CENTER );
  toolBarLayer.SetParentOrigin( Dali::ParentOrigin::TOP_CENTER );
  toolBarLayer.SetResizePolicy( Dali::ResizePolicy::FILL_TO_PARENT, Dali::Dimension::WIDTH );
  toolBarLayer.SetSize( 0.0f, style.mToolBarHeight );

  // Raise tool bar layer to the top.
  toolBarLayer.RaiseToTop();

  // Tool bar
  toolBar = Dali::Toolkit::ToolBar::New();
  toolBar.SetName( "TOOLBAR" );
  Dali::Property::Map background;
  background["url"] = toolbarImagePath;
  toolBar.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, background );
  toolBar.SetParentOrigin( Dali::ParentOrigin::TOP_CENTER );
  toolBar.SetAnchorPoint( Dali::AnchorPoint::TOP_CENTER );
  toolBar.SetResizePolicy( Dali::ResizePolicy::FILL_TO_PARENT, Dali::Dimension::ALL_DIMENSIONS );

  // Add the tool bar to the tool bar layer.
  toolBarLayer.Add( toolBar );

  // Tool bar text.
  if( !title.empty() )
  {
    Dali::Toolkit::TextLabel label = Dali::Toolkit::TextLabel::New();
    label.SetAnchorPoint( Dali::AnchorPoint::TOP_LEFT );
    label.SetStyleName( "ToolbarLabel" );
    label.SetProperty( Dali::Toolkit::TextLabel::Property::TEXT, title );
    label.SetProperty( Dali::Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
    label.SetProperty( Dali::Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
    label.SetResizePolicy( Dali::ResizePolicy::FILL_TO_PARENT, Dali::Dimension::HEIGHT );

    // Add title to the tool bar.
    const float padding( style.mToolBarPadding );
    toolBar.AddControl( label, style.mToolBarTitlePercentage, Dali::Toolkit::Alignment::HorizontalCenter, Dali::Toolkit::Alignment::Padding( padding, padding, padding, padding ) );
  }

  return toolBarLayer;
}

Dali::Layer CreateView( Dali::Application& application,
                        Dali::Toolkit::Control& view,
                        Dali::Toolkit::ToolBar& toolBar,
                        const std::string& backgroundImagePath,
                        const std::string& toolbarImagePath,
                        const std::string& title,
                        const ViewStyle& style = DEFAULT_VIEW_STYLE )
{
  Dali::Stage stage = Dali::Stage::GetCurrent();

  // Hide the indicator bar
  application.GetWindow().ShowIndicator( Dali::Window::INVISIBLE );

  // Create default View.
  view = Dali::Toolkit::Control::New();
  view.SetAnchorPoint( Dali::AnchorPoint::CENTER );
  view.SetParentOrigin( Dali::ParentOrigin::CENTER );
  view.SetResizePolicy( Dali::ResizePolicy::FILL_TO_PARENT, Dali::Dimension::ALL_DIMENSIONS );

  // Add the view to the stage before setting the background.
  stage.Add( view );

  // Set background image, loading it at screen resolution:
  if ( !backgroundImagePath.empty() )
  {
    Dali::Property::Map map;
    map[Dali::Toolkit::Visual::Property::TYPE] = Dali::Toolkit::Visual::IMAGE;
    map[Dali::Toolkit::ImageVisual::Property::URL] = backgroundImagePath;
    map[Dali::Toolkit::ImageVisual::Property::DESIRED_WIDTH] = stage.GetSize().x;
    map[Dali::Toolkit::ImageVisual::Property::DESIRED_HEIGHT] = stage.GetSize().y;
    map[Dali::Toolkit::ImageVisual::Property::FITTING_MODE] = Dali::FittingMode::SCALE_TO_FILL;
    map[Dali::Toolkit::ImageVisual::Property::SAMPLING_MODE] = Dali::SamplingMode::BOX_THEN_LINEAR;
    map[Dali::Toolkit::ImageVisual::Property::SYNCHRONOUS_LOADING] = true;
    view.SetProperty( Dali::Toolkit::Control::Property::BACKGROUND, map );
  }

  // FIXME
  // Connects the orientation signal with the View::OrientationChanged method.
  //application.GetOrientation().ChangedSignal().Connect( &view, &Dali::Toolkit::View::OrientationChanged );

  // Create default ToolBar
  Dali::Layer toolBarLayer = CreateToolbar( toolBar, toolbarImagePath, title, style );

  // Add tool bar layer to the view.
  view.Add( toolBarLayer );

  // Create a content layer.
  Dali::Layer contentLayer = Dali::Layer::New();
  contentLayer.SetAnchorPoint( Dali::AnchorPoint::CENTER );
  contentLayer.SetParentOrigin( Dali::ParentOrigin::CENTER );
  contentLayer.SetResizePolicy( Dali::ResizePolicy::FILL_TO_PARENT, Dali::Dimension::ALL_DIMENSIONS );
  view.Add( contentLayer );
  contentLayer.LowerBelow( toolBarLayer );

  return contentLayer;
}

Dali::Toolkit::TextLabel CreateToolBarLabel( const std::string& text )
{
  Dali::Toolkit::TextLabel label = Dali::Toolkit::TextLabel::New( text );
  label.SetStyleName( "ToolbarLabel" );
  label.SetProperty( Dali::Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
  label.SetProperty( Dali::Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
  label.SetResizePolicy( Dali::ResizePolicy::FILL_TO_PARENT, Dali::Dimension::HEIGHT );

  return label;
}

} // DemoHelper

#endif // __DALI_DEMO_HELPER_VIEW_H__
