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

#include "shared/view.h"
#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/controls/popup/popup.h>
#include <dali-toolkit/devel-api/focus-manager/keyinput-focus-manager.h>

using namespace Dali;

using Dali::Toolkit::TextLabel;

struct ButtonItem
{
  const char* name;
  const char* text;
};

namespace
{

const char* const BACKGROUND_IMAGE = DEMO_IMAGE_DIR "background-gradient.jpg";
const char* const TOOLBAR_IMAGE = DEMO_IMAGE_DIR "top-bar.png";
const char* const IMAGE = DEMO_IMAGE_DIR "background-magnifier.jpg";

const char* const TOOLBAR_TITLE = "Negotiate Size";

// This example contains size negotiation tests for TableView and Popup.
const char* const TABLEVIEW_BUTTON_1CELL_ID = "TABLEVIEW_BUTTON_1CELL";
const char* const TABLEVIEW_BUTTON_3CELL_ID = "TABLEVIEW_BUTTON_3CELL";
const char* const TABLEVIEW_BUTTON_3X3CELL_ID = "TABLEVIEW_BUTTON_3X3CELL";
const char* const TABLEVIEW_BUTTON_FIXED1_ID = "TABLEVIEW_BUTTON_FIXED1";
const char* const TABLEVIEW_BUTTON_FIXED2_ID = "TABLEVIEW_BUTTON_FIXED2";
const char* const TABLEVIEW_BUTTON_FIT1_ID = "TABLEVIEW_BUTTON_FIT1";
const char* const TABLEVIEW_BUTTON_FIT2_ID = "TABLEVIEW_BUTTON_FIT2";
const char* const TABLEVIEW_BUTTON_NATURAL1_ID = "TABLEVIEW_BUTTON_NATURAL1";
const char* const TABLEVIEW_BUTTON_NATURAL2_ID = "TABLEVIEW_BUTTON_NATURAL2";
const char* const TABLEVIEW_BUTTON_NATURAL3_ID = "TABLEVIEW_BUTTON_NATURAL3";
const char* const POPUP_BUTTON_CONTENT_IMAGE_SCALE_ID = "POPUP_BUTTON_CONTENT_IMAGE_SCALE";
const char* const POPUP_BUTTON_CONTENT_IMAGE_FIT_ID = "POPUP_BUTTON_CONTENT_IMAGE_FIT";
const char* const POPUP_BUTTON_CONTENT_IMAGE_FILL_ID = "POPUP_BUTTON_CONTENT_IMAGE_FILL";

const ButtonItem TABLEVIEW_BUTTON_ITEMS[] = {
    { TABLEVIEW_BUTTON_1CELL_ID,                    "1 Cell" },
    { TABLEVIEW_BUTTON_3CELL_ID,                    "3 Cell" },
    { TABLEVIEW_BUTTON_3X3CELL_ID,                  "3x3 Cells" },
    { TABLEVIEW_BUTTON_FIXED1_ID,                   "Fixed 1" },
    { TABLEVIEW_BUTTON_FIXED2_ID,                   "Fixed 2" },
    { TABLEVIEW_BUTTON_FIT1_ID,                     "Fit Top Bottom" },
    { TABLEVIEW_BUTTON_FIT2_ID,                     "Fit Middle" },
    { TABLEVIEW_BUTTON_NATURAL1_ID,                 "Natural 1" },
    { TABLEVIEW_BUTTON_NATURAL2_ID,                 "Natural 2" },
    { TABLEVIEW_BUTTON_NATURAL3_ID,                 "Natural 3" },
    { POPUP_BUTTON_CONTENT_IMAGE_SCALE_ID,          "Image Scale" },
    { POPUP_BUTTON_CONTENT_IMAGE_FIT_ID,            "Image Fit" },
    { POPUP_BUTTON_CONTENT_IMAGE_FILL_ID,           "Image Fill" },
};

const unsigned int TABLEVIEW_BUTTON_ITEMS_COUNT = sizeof( TABLEVIEW_BUTTON_ITEMS ) / sizeof( TABLEVIEW_BUTTON_ITEMS[0] );


Actor CreateSolidColor( Vector4 color )
{
  Toolkit::Control control = Toolkit::Control::New();

  Property::Map map;
  map[ Toolkit::Visual::Property::TYPE ] = Toolkit::Visual::COLOR;
  map[ Toolkit::ColorVisual::Property::MIX_COLOR ] = color;
  control.SetProperty( Toolkit::Control::Property::BACKGROUND, map );

  return control;
}

}  // anonymous namespace


/**
 * This example shows the usage of size negotiation.
 */
class SizeNegotiationController: public ConnectionTracker, public Toolkit::ItemFactory
{
public:

  SizeNegotiationController( Application& application )
    : mApplication( application )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &SizeNegotiationController::Create );
  }

  ~SizeNegotiationController()
  {
    // Nothing to do here
  }

  void Create( Application& application )
  {
    // The Init signal is received once (only) during the Application lifetime
    Stage stage = Stage::GetCurrent();

    // Respond to key events if not handled
    stage.KeyEventSignal().Connect(this, &SizeNegotiationController::OnKeyEvent);

    // Creates a default view with a default tool bar.
    // The view is added to the stage.
    mContentLayer = DemoHelper::CreateView( application,
                                            mView,
                                            mToolBar,
                                            BACKGROUND_IMAGE,
                                            TOOLBAR_IMAGE,
                                            std::string("") );

    mTitleActor = DemoHelper::CreateToolBarLabel( "CUSTOM_TOOLBAR_TITLE" );
    mTitleActor.SetProperty( Toolkit::TextLabel::Property::TEXT, TOOLBAR_TITLE );

    // Add title to the tool bar.
    const float padding( DemoHelper::DEFAULT_VIEW_STYLE.mToolBarPadding );
    mToolBar.AddControl( mTitleActor, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Toolkit::Alignment::HorizontalCenter, Toolkit::Alignment::Padding( padding, padding, padding, padding ) );

    mItemView = Toolkit::ItemView::New( *this );
    mItemView.SetParentOrigin( ParentOrigin::CENTER );
    mItemView.SetAnchorPoint( AnchorPoint::CENTER );
    mItemView.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

    // Use a grid layout for tests
    Vector2 stageSize = stage.GetSize();
    Toolkit::ItemLayoutPtr gridLayout = Toolkit::DefaultItemLayout::New( Toolkit::DefaultItemLayout::LIST );
    Vector3 itemSize;
    gridLayout->GetItemSize( 0, Vector3( stageSize ), itemSize );
    itemSize.height = stageSize.y / 10;
    gridLayout->SetItemSize( itemSize );
    mItemView.AddLayout( *gridLayout );

    mItemView.ActivateLayout( 0, Vector3(stageSize.x, stageSize.y, stageSize.x), 0.0f );

    mContentLayer.Add( mItemView );
  }

  void StagePopup( Toolkit::Popup popup )
  {
    Stage::GetCurrent().Add( popup );
    popup.SetDisplayState( Toolkit::Popup::SHOWN );
  }

  void OnPopupOutsideTouched()
  {
    if( mPopup )
    {
      mPopup.SetDisplayState( Toolkit::Popup::HIDDEN );
    }
  }

  void PopupHidden()
  {
    if( mPopup )
    {
      mPopup.Unparent();
      mPopup.Reset();
    }
  }

  Toolkit::Popup CreatePopup()
  {
    Stage stage = Stage::GetCurrent();
    const float POPUP_WIDTH_DP = stage.GetSize().width * 0.75f;

    Toolkit::Popup popup = Toolkit::Popup::New();
    popup.SetName( "popup" );
    popup.SetParentOrigin( ParentOrigin::CENTER );
    popup.SetAnchorPoint( AnchorPoint::CENTER );
    popup.SetSize( POPUP_WIDTH_DP, 0.0f );
    popup.SetProperty( Toolkit::Popup::Property::TAIL_VISIBILITY, false );

    popup.OutsideTouchedSignal().Connect( this, &SizeNegotiationController::OnPopupOutsideTouched );
    popup.HiddenSignal().Connect( this, &SizeNegotiationController::PopupHidden );

    return popup;
  }

  bool OnButtonClicked( Toolkit::Button button )
  {
    if( button.GetName() == TABLEVIEW_BUTTON_1CELL_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );

      Toolkit::TableView table = Toolkit::TableView::New( 0, 0 );
      table.SetName( "TABLEVIEW_BUTTON_1CELL_ID" );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

      Actor backing = CreateSolidColor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
      backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      table.Add( backing );

      mPopup.Add( table );

      StagePopup( mPopup );
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_3CELL_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );

      Toolkit::TableView table = Toolkit::TableView::New( 3, 1 );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

      {
        Actor backing = CreateSolidColor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 0, 0 ) );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 1, 0 ) );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 2, 0 ) );
      }

      mPopup.SetContent( table );

      StagePopup( mPopup );
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_3X3CELL_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );

      Toolkit::TableView table = Toolkit::TableView::New( 3, 3 );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

      // Column 0
      {
        Actor backing = CreateSolidColor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 0, 0 ) );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 1, 0 ) );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 2, 0 ) );
      }

      // Column 1
      {
        Actor backing = CreateSolidColor( Vector4( 1.0f, 0.0f, 1.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 0, 1 ) );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 1, 1 ) );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 0.0f, 0.0f, 1.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 2, 1 ) );
      }

      // Column 2
      {
        Actor backing = CreateSolidColor( Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 0, 2 ) );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 0.5f, 0.5f, 0.5f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 1, 2 ) );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 1.0f, 0.5f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        table.AddChild( backing, Toolkit::TableView::CellPosition( 2, 2 ) );
      }

      mPopup.Add( table );

      StagePopup( mPopup );
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_FIXED1_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );

      Toolkit::TableView table = Toolkit::TableView::New( 3, 1 );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      table.SetFixedHeight( 0, 50.0f );

      {
        Actor backing = CreateSolidColor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fixed" );
        text.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
        backing.Add( text );
        table.Add( backing );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fill" );
        text.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
        backing.Add( text );
        table.Add( backing );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fill" );
        text.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );
        backing.Add( text );
        table.Add( backing );
      }

      mPopup.Add( table );

      StagePopup( mPopup );
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_FIXED2_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );

      Toolkit::TableView table = Toolkit::TableView::New( 3, 1 );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      table.SetFixedHeight( 0, 50.0f );
      table.SetFixedHeight( 2, 50.0f );

      {
        Actor backing = CreateSolidColor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fixed" );
        text.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );
        table.Add( backing );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fill" );
        text.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );
        table.Add( backing );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fixed" );
        text.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );
        table.Add( backing );
      }

      mPopup.Add( table );

      StagePopup( mPopup );
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_FIT1_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );

      Toolkit::TableView table = Toolkit::TableView::New( 3, 1 );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      table.SetFitHeight( 0 );
      table.SetFitHeight( 2 );

      {
        Actor backing = CreateSolidColor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        backing.SetSize( 0.0f, 100.0f );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fit" );
        text.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fill" );
        text.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        backing.SetSize( 0.0f, 100.0f );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fit" );
        text.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }

      mPopup.Add( table );

      StagePopup( mPopup );
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_FIT2_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );

      Toolkit::TableView table = Toolkit::TableView::New( 3, 1 );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      table.SetFitHeight( 1 );

      {
        Actor backing = CreateSolidColor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fill" );
        text.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        backing.SetSize( 0.0f, 200.0f );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fit" );
        text.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        backing.Add( text );

        table.Add( backing );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fill" );
        text.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }

      mPopup.Add( table );

      StagePopup( mPopup );
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_NATURAL1_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::WIDTH );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 1.0f, 1.0f ) );
      mPopup.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );

      Toolkit::TableView table = Toolkit::TableView::New( 3, 1 );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      table.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      table.SetFitHeight( 0 );
      table.SetFitHeight( 1 );
      table.SetFitHeight( 2 );

      {
        Actor backing = CreateSolidColor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        backing.SetSize( 0.0f, 100.0f );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fit" );
        text.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 1.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        backing.SetSize( 0.0f, 200.0f );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fit" );
        text.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        backing.SetSize( 0.0f, 300.0f );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fit" );
        text.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }

      mPopup.Add( table );

      StagePopup( mPopup );
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_NATURAL2_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::WIDTH );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 1.0f, 1.0f ) );
      mPopup.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );

      Toolkit::TableView table = Toolkit::TableView::New( 3, 1 );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      table.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      table.SetFitHeight( 0 );
      table.SetFitHeight( 1 );

      {
        Actor backing = CreateSolidColor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        backing.SetResizePolicy( ResizePolicy::FIXED, Dimension::HEIGHT );
        backing.SetSize( 0.0f, 100.0f );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fit" );
        text.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        backing.SetSize( 0.0f, 200.0f );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fit" );
        text.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }

      mPopup.Add( table );

      StagePopup( mPopup );
    }
    else if( button.GetName() == TABLEVIEW_BUTTON_NATURAL3_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::WIDTH );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 1.0f, 1.0f ) );
      mPopup.SetResizePolicy( ResizePolicy::FIT_TO_CHILDREN, Dimension::HEIGHT );

      Toolkit::TableView table = Toolkit::TableView::New( 3, 1 );
      table.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
      table.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::HEIGHT );
      table.SetFixedHeight( 0, 20.0f );
      table.SetFitHeight( 1 );

      {
        Actor backing = CreateSolidColor( Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fixed" );
        text.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }
      {
        Actor backing = CreateSolidColor( Vector4( 0.0f, 1.0f, 0.0f, 1.0f ) );
        backing.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );
        backing.SetSize( 0.0f, 200.0f );

        Toolkit::TextLabel text = Toolkit::TextLabel::New( "Fit" );
        text.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Color::WHITE );
        text.SetAnchorPoint( AnchorPoint::CENTER );
        text.SetParentOrigin( ParentOrigin::CENTER );
        text.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
        text.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        text.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "CENTER" );

        backing.Add( text );

        table.Add( backing );
      }
      mPopup.Add( table );

      StagePopup( mPopup );
    }
    else if( button.GetName() == POPUP_BUTTON_CONTENT_IMAGE_SCALE_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );

      Toolkit::ImageView image = Toolkit::ImageView::New( IMAGE );
      image.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );

      mPopup.Add( image );

      StagePopup( mPopup );
    }
    else if( button.GetName() == POPUP_BUTTON_CONTENT_IMAGE_FIT_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );

      Toolkit::ImageView image = Toolkit::ImageView::New( IMAGE );
      image.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      image.SetSizeScalePolicy( SizeScalePolicy::FIT_WITH_ASPECT_RATIO );

      mPopup.Add( image );

      StagePopup( mPopup );
    }
    else if( button.GetName() == POPUP_BUTTON_CONTENT_IMAGE_FILL_ID )
    {
      mPopup = CreatePopup();
      mPopup.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
      mPopup.SetSizeModeFactor( Vector3( 0.75f, 0.5f, 1.0f ) );

      Toolkit::ImageView image = Toolkit::ImageView::New( IMAGE );
      image.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
      image.SetSizeScalePolicy( SizeScalePolicy::FILL_WITH_ASPECT_RATIO );

      mPopup.Add( image );

      StagePopup( mPopup );
    }

    return true;
  }

  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        // Exit application when click back or escape.
        mApplication.Quit();
      }
    }
  }

public: // From ItemFactory

  /**
   * @brief Return the number of items to display in the item view
   *
   * @return Return the number of items to display
   */
  virtual unsigned int GetNumberOfItems()
  {
    return TABLEVIEW_BUTTON_ITEMS_COUNT;
  }

  /**
   * @brief Create a new item to populate the item view with
   *
   * @param[in] itemId The index of the item to create
   * @return Return the created actor for the given ID
   */
  virtual Actor NewItem(unsigned int itemId)
  {
    Toolkit::PushButton popupButton = Toolkit::PushButton::New();
    popupButton.SetName( TABLEVIEW_BUTTON_ITEMS[ itemId ].name );
    popupButton.SetProperty( Toolkit::Button::Property::LABEL, TABLEVIEW_BUTTON_ITEMS[ itemId ].text );
    popupButton.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );

    popupButton.ClickedSignal().Connect( this, &SizeNegotiationController::OnButtonClicked );

    return popupButton;
  }

private:

  Application&       mApplication;
  Toolkit::Control   mView;                  ///< The View instance.
  Toolkit::ToolBar   mToolBar;               ///< The View's Toolbar.
  Layer              mContentLayer;          ///< Content layer.

  Toolkit::TextLabel mTitleActor;            ///< Title text.
  Toolkit::Popup     mMenu;                  ///< The navigation menu.
  Toolkit::Popup     mPopup;                 ///< The current example popup.

  Toolkit::ItemView  mItemView;              ///< ItemView to hold test images.

};

int DALI_EXPORT_API main( int argc, char **argv )
{
  Application application = Application::New( &argc, &argv, DEMO_THEME_PATH );
  SizeNegotiationController test( application );
  application.MainLoop();
  return 0;
}
