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

// EXTERNAL INCLUDES
#include <dali-toolkit/dali-toolkit.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <sstream>
#include <iostream>

#include <dali-toolkit/devel-api/visual-factory/visual-factory.h>

using namespace Dali;
using Dali::Toolkit::TextLabel;

namespace
{
enum IconType
{
  IMAGEVIEW,
  CHECKBOX
};

const char* IMAGE_PATH_PREFIX               ( DEMO_IMAGE_DIR "application-icon-" );
const char* IMAGE_PATH_POSTFIX              ( ".png" );
const int   TOTAL_ICON_DEFINITIONS          ( 147 );

const char* BACKGROUND_IMAGE                ( DEMO_IMAGE_DIR "background-3.jpg" );
const float PAGE_SCALE_FACTOR_X             ( 0.95f );
const float PAGE_SCALE_FACTOR_Y             ( 0.95f );
const float PAGE_DURATION_SCALE_FACTOR      ( 10.0f ); ///< Time-scale factor, larger = animation is slower

const float DEFAULT_OPT_ROW_COUNT           ( 5 );
const float DEFAULT_OPT_COL_COUNT           ( 4 );
const float DEFAULT_OPT_PAGE_COUNT          ( 10 );
const bool  DEFAULT_OPT_USE_TABLEVIEW       ( true );
const bool  DEFAULT_OPT_ICON_LABELS         ( true );
const IconType  DEFAULT_OPT_ICON_TYPE       ( IMAGEVIEW );
const bool  DEFAULT_OPT_USE_TEXT_LABEL      ( false );

// The image/label area tries to make sure the positioning will be relative to previous sibling
const float IMAGE_AREA                      ( 0.60f );
const float LABEL_AREA                      ( 0.50f );



/**
 * Random words used as unique application names.
 * The number matches the value of TOTAL_ICON_DEFINITIONS.
 */
const char* DEMO_APPS_NAMES[] =
{
  "Achdyer",   "Aughm",       "Cerl",       "Daril",      "Emgha",     "Ghatan",     "Issum",     "Lertan",    "Mosorrad",
  "Achtortor", "Aughtheryer", "Certin",     "Darpban",    "Emiton",    "Gibanis",    "Itenthbel", "Liadem",    "Mosraye",
  "Ackirlor",  "Awitad",      "Checerper",  "Dasrad",     "Emworeng",  "Hatdyn",     "K'ackves",  "Liathar",   "Mosth",
  "Ackptin",   "Banengon",    "Chegit",     "Deeqskel",   "Endnys",    "Heesban",    "Kagdra",    "Liephden",  "Neabar",
  "Aighte",    "Banhinat",    "Cheirat",    "Delurnther", "Enessray",  "Hesub",      "Kalbankim", "Likellor",  "Neerdem",
  "Akala",     "Belrisash",   "Che'rak",    "Denalda",    "Engyer",    "Hinkelenth", "Kal'enda",  "Loightmos", "Nichqua",
  "Alealdny",  "Bilorm",      "Cheves",     "Derynkel",   "En'rady",   "Hirryer",    "Kimest",    "Loromum",   "Nudraough",
  "Angash",    "Bleustcer",   "Chiperath",  "Deurnos",    "Enthount",  "Ideinta",    "Kimundeng", "Lorr",      "Nuyim",
  "Anglor",    "Bliagelor",   "Chralerack", "Doyaryke",   "Enundem",   "Im'eld",     "Koachlor",  "Lortas",    "Nycha",
  "Anveraugh", "Blorynton",   "Chram",      "Draithon",   "Essina",    "Ina'ir",     "Kuren",     "Lyerr",     "Nyia",
  "Ardangas",  "Booten",      "Clyimen",    "Drantess",   "Faughald",  "Ing'moro",   "Kygver",    "Maustbur",  "Nyjac",
  "Ardug",     "Bripolqua",   "Coqueang",   "Druardny",   "Fiummos",   "Ingormess",  "Kyning",    "Menvor",    "Nystondar",
  "Ardworu",   "Bryray",      "Craennther", "Dynsaytor",  "Garash",    "Ingshy",     "Laiyach",   "Meusten",   "Okine",
  "Ascerald",  "Burust",      "Cykage",     "Dytinris",   "Garight",   "Issath",     "Lasuzu",    "Mirodskel", "Oldit",
  "Ash'ach",   "Cataikel",    "Dalek",      "Eeni",       "Garrynath", "Issendris",  "Lekew",     "Morhatrod", "Om'mose",
  "Athiund",   "Cerilwar",    "Darhkel",    "Elmryn",     "Ghalora",   "Issey",      "Lerengom",  "Moserbel",  "Onye",
  "Ososrak",   "Pecertin",    "Perrd"
};

// This code comes from command-line-options.cpp. the reason it's here is to
// keep consistent the extra-help formatting when '--help' used.
void PrintHelp( const char * const opt, const char * const optDescription)
{
  const std::ios_base::fmtflags flags = std::cout.flags();
  std::cout << std::left << "  -";
  std::cout.width( 18 );
  std::cout << opt;
  std::cout << optDescription;
  std::cout << std::endl;
  std::cout.flags( flags );
}

}

/**
 * @brief This example is a benchmark that mimics the paged applications list of the homescreen application.
 */
class HomescreenBenchmark : public ConnectionTracker
{
public:

  // Config structure passed to the constructor. It makes easier to increase number
  // of setup parameters if needed.
  struct Config
  {
    Config() :
      mRows( DEFAULT_OPT_ROW_COUNT ),
      mCols( DEFAULT_OPT_COL_COUNT ),
      mPageCount( DEFAULT_OPT_PAGE_COUNT ),
      mTableViewEnabled( DEFAULT_OPT_USE_TABLEVIEW ),
      mIconLabelsEnabled( DEFAULT_OPT_ICON_LABELS ),
      mIconType( DEFAULT_OPT_ICON_TYPE ),
      mUseTextLabel( DEFAULT_OPT_USE_TEXT_LABEL )
    {
    }

    int  mRows;
    int  mCols;
    int  mPageCount;
    bool mTableViewEnabled;
    bool mIconLabelsEnabled;
    IconType mIconType;
    bool mUseTextLabel;
  };

  // animation script data
  struct ScriptData
  {
    ScriptData( int pages, float duration, bool flick )
    : mPages( pages ),
      mDuration( duration ),
      mFlick( flick )
    {
    }

    int   mPages;    ///< Number of pages to scroll
    float mDuration; ///< Duration
    bool  mFlick;    ///< Use flick or 'one-by-one' scroll
  };

  HomescreenBenchmark( Application& application, const Config& config )
  : mApplication( application ),
    mConfig( config ),
    mScriptFrame( 0 ),
    mCurrentPage( 0 )
  {
    // Connect to the Application's Init signal.
    mApplication.InitSignal().Connect( this, &HomescreenBenchmark::Create );
  }

  ~HomescreenBenchmark()
  {
  }

  // The Init signal is received once (only) during the Application lifetime.
  void Create( Application& application )
  {
    // Create benchmark script
    CreateScript();

    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();

    mScrollParent = Actor::New();
    mScrollParent.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mScrollParent.SetAnchorPoint( AnchorPoint::CENTER );
    mScrollParent.SetParentOrigin( ParentOrigin::CENTER );

    // create background
    Toolkit::ImageView background = Toolkit::ImageView::New( BACKGROUND_IMAGE );
    Stage::GetCurrent().Add( background );
    background.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    background.SetAnchorPoint( AnchorPoint::CENTER );
    background.SetParentOrigin( ParentOrigin::CENTER );

    PopulatePages();

    stage.Add( mScrollParent );

    // Respond to a click anywhere on the stage.
    stage.GetRootLayer().TouchSignal().Connect( this, &HomescreenBenchmark::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchData& touch )
  {
    // Quit the application.
    mApplication.Quit();
    return true;
  }

  Actor AddPage()
  {
    // Create root page actor.
    Actor pageActor;

    if( mConfig.mTableViewEnabled )
    {
      Toolkit::TableView tableView = Toolkit::TableView::New( mConfig.mRows, mConfig.mCols );

      // Create geometry batcher for table view.
      tableView.SetBackgroundColor( Vector4( 0.0f, 0.0f, 0.0f, 0.5f ) );
      pageActor = tableView;
    }
    else
    {
      pageActor = Toolkit::Control::New();
      pageActor.SetProperty( Toolkit::Control::Property::BACKGROUND_COLOR, Vector4( 0.0f, 0.0f, 0.0f, 0.5f ) );
    }

    pageActor.SetParentOrigin( ParentOrigin::CENTER );
    pageActor.SetAnchorPoint( AnchorPoint::CENTER );
    pageActor.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    pageActor.SetSizeModeFactor( Vector3( PAGE_SCALE_FACTOR_X, PAGE_SCALE_FACTOR_Y, 1.0f ) );
    return pageActor;
  }

  Toolkit::ImageView CreateImageView( const unsigned int currentIconIndex )
  {
    // Create empty image to avoid early renderer creation
    Toolkit::ImageView imageView = Toolkit::ImageView::New();

    // Auto-generate the Icons image URL.
    Property::Map map;
    std::stringstream imagePath;
    imagePath << IMAGE_PATH_PREFIX << currentIconIndex << IMAGE_PATH_POSTFIX;
    map[ Dali::Toolkit::ImageVisual::Property::URL ] = imagePath.str();

    imageView.SetProperty( Toolkit::ImageView::Property::IMAGE, map );
    imageView.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    imageView.SetSizeScalePolicy( SizeScalePolicy::FIT_WITH_ASPECT_RATIO );
    imageView.SetAnchorPoint( AnchorPoint::CENTER );
    imageView.SetParentOrigin( ParentOrigin::CENTER );
    imageView.SetSizeModeFactor( Vector3( IMAGE_AREA, IMAGE_AREA, 1.0f ) );

    return imageView;
  }

  Toolkit::Button CreateButton( const unsigned int currentIconIndex )
  {
    Toolkit::CheckBoxButton button = Toolkit::CheckBoxButton::New();
    button.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
    button.SetSizeScalePolicy( SizeScalePolicy::FIT_WITH_ASPECT_RATIO );
    button.SetAnchorPoint( AnchorPoint::CENTER );
    button.SetParentOrigin( ParentOrigin::CENTER );
    button.SetProperty( Toolkit::Button::Property::SELECTED, ( currentIconIndex % 2 == 0 ) ); // Select half the button

    return button;
  }

  void AddIconsToPage( Actor page, bool useTextLabel )
  {
    Size stageSize( Stage::GetCurrent().GetSize() );
    const float scaledHeight = stageSize.y * PAGE_SCALE_FACTOR_Y;
    const float scaledWidth = stageSize.x * PAGE_SCALE_FACTOR_X;
    const float PADDING = stageSize.y / 64.0f;
    const float ROW_HEIGHT = ( scaledHeight - (PADDING*2.0f) ) / static_cast<float>( mConfig.mRows );
    const float COL_WIDTH = ( scaledWidth - (PADDING*2.0f) ) / static_cast<float>( mConfig.mCols );

    Vector2 dpi = Stage::GetCurrent().GetDpi();

    static int currentIconIndex = 0;

    for( int y = 0; y < mConfig.mRows; ++y )
    {
      for( int x = 0; x < mConfig.mCols; ++x )
      {
        // Create parent icon view
        Toolkit::Control iconView = Toolkit::Control::New();
        iconView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
        iconView.SetParentOrigin( ParentOrigin::TOP_LEFT );

        if( !mConfig.mTableViewEnabled )
        {
          float rowX = x * COL_WIDTH + PADDING;
          float rowY = y * ROW_HEIGHT + PADDING;
          iconView.SetSize( Vector3( COL_WIDTH, ROW_HEIGHT, 1.0f ) );
          iconView.SetPosition( Vector3( rowX, rowY, 0.0f ) );
        }
        else
        {
          iconView.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
          iconView.SetSizeScalePolicy( SizeScalePolicy::FIT_WITH_ASPECT_RATIO );
        }

        Actor icon;

        switch( mConfig.mIconType )
        {
          case CHECKBOX:
          {
            icon = CreateButton( currentIconIndex );
            break;
          }
          case IMAGEVIEW:
          {
            icon = CreateImageView( currentIconIndex );
            break;
          }
        }

        if( mConfig.mIconLabelsEnabled )
        {
          // create label
          if( useTextLabel )
          {
            Toolkit::TextLabel textLabel = Toolkit::TextLabel::New( DEMO_APPS_NAMES[currentIconIndex] );
            textLabel.SetAnchorPoint( AnchorPoint::TOP_CENTER );
            textLabel.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
            textLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
            textLabel.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) ); // White.
            textLabel.SetProperty( Toolkit::TextLabel::Property::POINT_SIZE, ( ( static_cast<float>( ROW_HEIGHT * LABEL_AREA ) * 72.0f )  / dpi.y ) * 0.25f );
            textLabel.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
            textLabel.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "TOP" );
            icon.Add( textLabel );
          }
          else
          {
            Property::Map map;
            map.Add( Toolkit::Visual::Property::TYPE, Toolkit::Visual::TEXT ).
              Add( Toolkit::TextVisual::Property::TEXT, DEMO_APPS_NAMES[currentIconIndex] ).
              Add( Toolkit::TextVisual::Property::TEXT_COLOR, Color::WHITE ).
              Add( Toolkit::TextVisual::Property::POINT_SIZE, ( ( static_cast<float>( ROW_HEIGHT * LABEL_AREA ) * 72.0f )  / dpi.y ) * 0.25f ).
              Add( Toolkit::TextVisual::Property::HORIZONTAL_ALIGNMENT, "CENTER" ).
              Add( Toolkit::TextVisual::Property::VERTICAL_ALIGNMENT, "TOP" );

            Toolkit::Control control = Toolkit::Control::New();
            control.SetProperty( Toolkit::Control::Property::BACKGROUND, map );
            control.SetAnchorPoint( AnchorPoint::TOP_CENTER );
            control.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );
            icon.Add( control );
          }
        }

        iconView.Add( icon );
        page.Add( iconView );

        // We only have images and names for a certain number of icons.
        // Wrap around if we have used them all.
        if( ++currentIconIndex == TOTAL_ICON_DEFINITIONS )
        {
          currentIconIndex = 0;
        }
      }
    }
  }

  void CreateScript()
  {
    const int lastPage = mConfig.mPageCount - 1;
    const int halfA = lastPage / 2;
    const int halfB = lastPage / 2 + lastPage % 2;
    mScriptFrameData.push_back( ScriptData( lastPage,  1.5f, true  ) );
    mScriptFrameData.push_back( ScriptData( -lastPage, 1.5f, true  ) );
    mScriptFrameData.push_back( ScriptData( halfA,     1.0f, true  ) );
    mScriptFrameData.push_back( ScriptData( halfB,     1.0f, true  ) );
    mScriptFrameData.push_back( ScriptData( -lastPage, 0.5f, false ) );
    mScriptFrameData.push_back( ScriptData( halfA,     0.5f, false ) );
    mScriptFrameData.push_back( ScriptData( halfB,     1.0f, true  ) );
    mScriptFrameData.push_back( ScriptData( -halfA,    1.0f, true  ) );
    mScriptFrameData.push_back( ScriptData( 1,         0.1f, true  ) );
    mScriptFrameData.push_back( ScriptData( -1,        0.1f, true  ) );
    mScriptFrameData.push_back( ScriptData( 1,         0.1f, true  ) );
    mScriptFrameData.push_back( ScriptData( -1,        0.1f, true  ) );
    mScriptFrameData.push_back( ScriptData( 1,         0.1f, true  ) );
    mScriptFrameData.push_back( ScriptData( -1,        0.1f, true  ) );
    mScriptFrameData.push_back( ScriptData( halfA,     1.0f, true  ) );
  }

  void PopulatePages()
  {
    Vector3 stageSize( Stage::GetCurrent().GetSize() );

    for( int i = 0; i < mConfig.mPageCount; ++i )
    {
      // Create page.
      Actor page = AddPage();

      // Populate icons.
      AddIconsToPage( page, mConfig.mUseTextLabel );

      // Move page 'a little bit up'.
      page.SetParentOrigin( ParentOrigin::CENTER );
      page.SetAnchorPoint( AnchorPoint::CENTER );
      page.SetPosition( Vector3( stageSize.x * i, 0.0f, 0.0f ) );
      mScrollParent.Add( page );
    }

    mScrollParent.SetOpacity( 1.0f );
    mScrollParent.SetScale( Vector3::ONE );

    // Fade in.
    ShowAnimation();
  }

  void ShowAnimation()
  {
    mShowAnimation = Animation::New( 1.0f );
    mShowAnimation.AnimateTo( Property( mScrollParent, Actor::Property::COLOR_ALPHA ), 1.0f, AlphaFunction::EASE_IN_OUT );
    mShowAnimation.AnimateTo( Property( mScrollParent, Actor::Property::SCALE ), Vector3::ONE, AlphaFunction::EASE_IN_OUT );
    mShowAnimation.FinishedSignal().Connect( this, &HomescreenBenchmark::OnAnimationEnd );
    mShowAnimation.Play();
  }

  void ScrollPages(int pages, float duration, bool flick)
  {
    duration *= PAGE_DURATION_SCALE_FACTOR;
    Vector3 stageSize( Stage::GetCurrent().GetSize() );
    mScrollAnimation = Animation::New( duration );
    if( flick )
    {
      mScrollAnimation.AnimateBy( Property( mScrollParent, Actor::Property::POSITION ), Vector3( -stageSize.x * pages, 0.0f, 0.0f ), AlphaFunction::EASE_IN_OUT );
    }
    else
    {
      int totalPages = abs( pages );
      for( int i = 0; i < totalPages; ++i )
      {
        mScrollAnimation.AnimateBy( Property( mScrollParent, Actor::Property::POSITION ), Vector3( pages < 0 ? stageSize.x : -stageSize.x, 0.0f, 0.0f ), AlphaFunction::EASE_IN_OUT, TimePeriod( duration * i, duration ) );
      }
    }
    mScrollAnimation.FinishedSignal().Connect( this, &HomescreenBenchmark::OnAnimationEnd );
    mScrollAnimation.Play();
    mCurrentPage += pages;
  }

  void OnAnimationEnd( Animation& source )
  {
    if( mScriptFrame < mScriptFrameData.size() )
    {
      ScriptData& frame = mScriptFrameData[mScriptFrame];
      ScrollPages( frame.mPages, frame.mDuration, frame.mFlick );
      ++mScriptFrame;
    }
    else
    {
      mApplication.Quit();
    }
  }

private:

  Application&                mApplication;
  Actor                       mScrollParent;
  Animation                   mShowAnimation;
  Animation                   mScrollAnimation;
  Config                      mConfig;
  std::vector<ScriptData>     mScriptFrameData;
  size_t                      mScriptFrame;
  int                         mCurrentPage;
};

int DALI_EXPORT_API main( int argc, char **argv )
{
  // Default settings.
  HomescreenBenchmark::Config config;

  bool printHelpAndExit = false;

  for( int i = 1 ; i < argc; ++i )
  {
    std::string arg( argv[i] );
    if( arg.compare( 0, 2, "-r" ) == 0 )
    {
      config.mRows = atoi( arg.substr( 2 ).c_str() );
    }
    else if( arg.compare( 0, 2, "-c" ) == 0 )
    {
      config.mCols = atoi( arg.substr( 2 ).c_str() );
    }
    else if( arg.compare( 0, 2, "-p" ) == 0 )
    {
      config.mPageCount = atoi( arg.substr( 2 ).c_str() );
    }
    else if( arg.compare( "--disable-tableview" ) == 0 )
    {
      config.mTableViewEnabled = false;
    }
    else if( arg.compare( "--disable-icon-labels" ) == 0 )
    {
      config.mIconLabelsEnabled = false;
    }
    else if( arg.compare( "--use-checkbox" ) == 0 )
    {
      config.mIconType = CHECKBOX;
    }
    else if( arg.compare("--use-text-label" ) == 0)
    {
      config.mUseTextLabel = true;
    }
    else if( arg.compare( "--help" ) == 0 )
    {
      printHelpAndExit = true;
    }
  }

  Application application = Application::New( &argc, &argv );
  HomescreenBenchmark test( application, config );

  if( printHelpAndExit )
  {
    PrintHelp( "c<num>",               " Number of columns" );
    PrintHelp( "r<num>",               " Number of rows" );
    PrintHelp( "p<num>",               " Number of pages ( must be greater than 1 )" );
    PrintHelp( "-disable-tableview",   " Disables the use of TableView for layouting" );
    PrintHelp( "-disable-icon-labels", " Disables labels for each icon" );
    PrintHelp( "-use-checkbox",        " Uses checkboxes for icons" );
    PrintHelp( "-use-text-label",      " Uses TextLabel instead of a TextVisual" );
    return 0;
  }

  application.MainLoop();

  return 0;
}
