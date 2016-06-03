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

#include <iostream>

using namespace Dali;
using Dali::Toolkit::TextLabel;

namespace
{
const char* IMAGE_PATH[] = {
  DEMO_IMAGE_DIR "application-icon-0.png",
  DEMO_IMAGE_DIR "application-icon-100.png",
  DEMO_IMAGE_DIR "application-icon-101.png",
  DEMO_IMAGE_DIR "application-icon-102.png",
  DEMO_IMAGE_DIR "application-icon-103.png",
  DEMO_IMAGE_DIR "application-icon-104.png",
  DEMO_IMAGE_DIR "application-icon-105.png",
  DEMO_IMAGE_DIR "application-icon-106.png",
  DEMO_IMAGE_DIR "application-icon-107.png",
  DEMO_IMAGE_DIR "application-icon-108.png",
  DEMO_IMAGE_DIR "application-icon-109.png",
  DEMO_IMAGE_DIR "application-icon-10.png",
  DEMO_IMAGE_DIR "application-icon-110.png",
  DEMO_IMAGE_DIR "application-icon-111.png",
  DEMO_IMAGE_DIR "application-icon-112.png",
  DEMO_IMAGE_DIR "application-icon-113.png",
  DEMO_IMAGE_DIR "application-icon-114.png",
  DEMO_IMAGE_DIR "application-icon-115.png",
  DEMO_IMAGE_DIR "application-icon-116.png",
  DEMO_IMAGE_DIR "application-icon-117.png",
  DEMO_IMAGE_DIR "application-icon-118.png",
  DEMO_IMAGE_DIR "application-icon-119.png",
  DEMO_IMAGE_DIR "application-icon-11.png",
  DEMO_IMAGE_DIR "application-icon-120.png",
  DEMO_IMAGE_DIR "application-icon-121.png",
  DEMO_IMAGE_DIR "application-icon-122.png",
  DEMO_IMAGE_DIR "application-icon-123.png",
  DEMO_IMAGE_DIR "application-icon-124.png",
  DEMO_IMAGE_DIR "application-icon-125.png",
  DEMO_IMAGE_DIR "application-icon-126.png",
  DEMO_IMAGE_DIR "application-icon-127.png",
  DEMO_IMAGE_DIR "application-icon-128.png",
  DEMO_IMAGE_DIR "application-icon-129.png",
  DEMO_IMAGE_DIR "application-icon-12.png",
  DEMO_IMAGE_DIR "application-icon-130.png",
  DEMO_IMAGE_DIR "application-icon-131.png",
  DEMO_IMAGE_DIR "application-icon-132.png",
  DEMO_IMAGE_DIR "application-icon-133.png",
  DEMO_IMAGE_DIR "application-icon-134.png",
  DEMO_IMAGE_DIR "application-icon-135.png",
  DEMO_IMAGE_DIR "application-icon-136.png",
  DEMO_IMAGE_DIR "application-icon-137.png",
  DEMO_IMAGE_DIR "application-icon-138.png",
  DEMO_IMAGE_DIR "application-icon-139.png",
  DEMO_IMAGE_DIR "application-icon-13.png",
  DEMO_IMAGE_DIR "application-icon-140.png",
  DEMO_IMAGE_DIR "application-icon-141.png",
  DEMO_IMAGE_DIR "application-icon-142.png",
  DEMO_IMAGE_DIR "application-icon-143.png",
  DEMO_IMAGE_DIR "application-icon-144.png",
  DEMO_IMAGE_DIR "application-icon-145.png",
  DEMO_IMAGE_DIR "application-icon-146.png",
  DEMO_IMAGE_DIR "application-icon-147.png",
  DEMO_IMAGE_DIR "application-icon-14.png",
  DEMO_IMAGE_DIR "application-icon-15.png",
  DEMO_IMAGE_DIR "application-icon-16.png",
  DEMO_IMAGE_DIR "application-icon-17.png",
  DEMO_IMAGE_DIR "application-icon-18.png",
  DEMO_IMAGE_DIR "application-icon-19.png",
  DEMO_IMAGE_DIR "application-icon-1.png",
  DEMO_IMAGE_DIR "application-icon-20.png",
  DEMO_IMAGE_DIR "application-icon-21.png",
  DEMO_IMAGE_DIR "application-icon-22.png",
  DEMO_IMAGE_DIR "application-icon-23.png",
  DEMO_IMAGE_DIR "application-icon-24.png",
  DEMO_IMAGE_DIR "application-icon-25.png",
  DEMO_IMAGE_DIR "application-icon-26.png",
  DEMO_IMAGE_DIR "application-icon-27.png",
  DEMO_IMAGE_DIR "application-icon-28.png",
  DEMO_IMAGE_DIR "application-icon-29.png",
  DEMO_IMAGE_DIR "application-icon-2.png",
  DEMO_IMAGE_DIR "application-icon-30.png",
  DEMO_IMAGE_DIR "application-icon-31.png",
  DEMO_IMAGE_DIR "application-icon-32.png",
  DEMO_IMAGE_DIR "application-icon-33.png",
  DEMO_IMAGE_DIR "application-icon-34.png",
  DEMO_IMAGE_DIR "application-icon-35.png",
  DEMO_IMAGE_DIR "application-icon-36.png",
  DEMO_IMAGE_DIR "application-icon-37.png",
  DEMO_IMAGE_DIR "application-icon-38.png",
  DEMO_IMAGE_DIR "application-icon-39.png",
  DEMO_IMAGE_DIR "application-icon-3.png",
  DEMO_IMAGE_DIR "application-icon-40.png",
  DEMO_IMAGE_DIR "application-icon-41.png",
  DEMO_IMAGE_DIR "application-icon-42.png",
  DEMO_IMAGE_DIR "application-icon-43.png",
  DEMO_IMAGE_DIR "application-icon-44.png",
  DEMO_IMAGE_DIR "application-icon-45.png",
  DEMO_IMAGE_DIR "application-icon-46.png",
  DEMO_IMAGE_DIR "application-icon-47.png",
  DEMO_IMAGE_DIR "application-icon-48.png",
  DEMO_IMAGE_DIR "application-icon-49.png",
  DEMO_IMAGE_DIR "application-icon-4.png",
  DEMO_IMAGE_DIR "application-icon-50.png",
  DEMO_IMAGE_DIR "application-icon-51.png",
  DEMO_IMAGE_DIR "application-icon-52.png",
  DEMO_IMAGE_DIR "application-icon-53.png",
  DEMO_IMAGE_DIR "application-icon-54.png",
  DEMO_IMAGE_DIR "application-icon-55.png",
  DEMO_IMAGE_DIR "application-icon-56.png",
  DEMO_IMAGE_DIR "application-icon-57.png",
  DEMO_IMAGE_DIR "application-icon-58.png",
  DEMO_IMAGE_DIR "application-icon-59.png",
  DEMO_IMAGE_DIR "application-icon-5.png",
  DEMO_IMAGE_DIR "application-icon-60.png",
  DEMO_IMAGE_DIR "application-icon-61.png",
  DEMO_IMAGE_DIR "application-icon-62.png",
  DEMO_IMAGE_DIR "application-icon-63.png",
  DEMO_IMAGE_DIR "application-icon-64.png",
  DEMO_IMAGE_DIR "application-icon-65.png",
  DEMO_IMAGE_DIR "application-icon-66.png",
  DEMO_IMAGE_DIR "application-icon-67.png",
  DEMO_IMAGE_DIR "application-icon-68.png",
  DEMO_IMAGE_DIR "application-icon-69.png",
  DEMO_IMAGE_DIR "application-icon-6.png",
  DEMO_IMAGE_DIR "application-icon-70.png",
  DEMO_IMAGE_DIR "application-icon-71.png",
  DEMO_IMAGE_DIR "application-icon-72.png",
  DEMO_IMAGE_DIR "application-icon-73.png",
  DEMO_IMAGE_DIR "application-icon-74.png",
  DEMO_IMAGE_DIR "application-icon-75.png",
  DEMO_IMAGE_DIR "application-icon-76.png",
  DEMO_IMAGE_DIR "application-icon-77.png",
  DEMO_IMAGE_DIR "application-icon-78.png",
  DEMO_IMAGE_DIR "application-icon-79.png",
  DEMO_IMAGE_DIR "application-icon-7.png",
  DEMO_IMAGE_DIR "application-icon-80.png",
  DEMO_IMAGE_DIR "application-icon-81.png",
  DEMO_IMAGE_DIR "application-icon-82.png",
  DEMO_IMAGE_DIR "application-icon-83.png",
  DEMO_IMAGE_DIR "application-icon-84.png",
  DEMO_IMAGE_DIR "application-icon-85.png",
  DEMO_IMAGE_DIR "application-icon-86.png",
  DEMO_IMAGE_DIR "application-icon-87.png",
  DEMO_IMAGE_DIR "application-icon-88.png",
  DEMO_IMAGE_DIR "application-icon-89.png",
  DEMO_IMAGE_DIR "application-icon-8.png",
  DEMO_IMAGE_DIR "application-icon-90.png",
  DEMO_IMAGE_DIR "application-icon-91.png",
  DEMO_IMAGE_DIR "application-icon-92.png",
  DEMO_IMAGE_DIR "application-icon-93.png",
  DEMO_IMAGE_DIR "application-icon-94.png",
  DEMO_IMAGE_DIR "application-icon-95.png",
  DEMO_IMAGE_DIR "application-icon-96.png",
  DEMO_IMAGE_DIR "application-icon-97.png",
  DEMO_IMAGE_DIR "application-icon-98.png",
  DEMO_IMAGE_DIR "application-icon-99.png",
  DEMO_IMAGE_DIR "application-icon-9.png",
  NULL
};

/**
 * Random words used as unique application names
 */
const char* DEMO_APPS_NAMES[] =
{
  "Achdyer",
  "Achtortor",
  "Ackirlor",
  "Ackptin",
  "Aighte",
  "Akala",
  "Alealdny",
  "Angash",
  "Anglor",
  "Anveraugh",
  "Ardangas",
  "Ardug",
  "Ardworu",
  "Ascerald",
  "Ash'ach",
  "Athiund",
  "Aughm",
  "Aughtheryer",
  "Awitad",
  "Banengon",
  "Banhinat",
  "Belrisash",
  "Bilorm",
  "Bleustcer",
  "Bliagelor",
  "Blorynton",
  "Booten",
  "Bripolqua",
  "Bryray",
  "Burust",
  "Cataikel",
  "Cerilwar",
  "Cerl",
  "Certin",
  "Checerper",
  "Chegit",
  "Cheirat",
  "Che'rak",
  "Cheves",
  "Chiperath",
  "Chralerack",
  "Chram",
  "Clyimen",
  "Coqueang",
  "Craennther",
  "Cykage",
  "Dalek",
  "Darhkel",
  "Daril",
  "Darpban",
  "Dasrad",
  "Deeqskel",
  "Delurnther",
  "Denalda",
  "Derynkel",
  "Deurnos",
  "Doyaryke",
  "Draithon",
  "Drantess",
  "Druardny",
  "Dynsaytor",
  "Dytinris",
  "Eeni",
  "Elmryn",
  "Emgha",
  "Emiton",
  "Emworeng",
  "Endnys",
  "Enessray",
  "Engyer",
  "En'rady",
  "Enthount",
  "Enundem",
  "Essina",
  "Faughald",
  "Fiummos",
  "Garash",
  "Garight",
  "Garrynath",
  "Ghalora",
  "Ghatan",
  "Gibanis",
  "Hatdyn",
  "Heesban",
  "Hesub",
  "Hinkelenth",
  "Hirryer",
  "Ideinta",
  "Im'eld",
  "Ina'ir",
  "Ing'moro",
  "Ingormess",
  "Ingshy",
  "Issath",
  "Issendris",
  "Issey",
  "Issum",
  "Itenthbel",
  "K'ackves",
  "Kagdra",
  "Kalbankim",
  "Kal'enda",
  "Kimest",
  "Kimundeng",
  "Koachlor",
  "Kuren",
  "Kygver",
  "Kyning",
  "Laiyach",
  "Lasuzu",
  "Lekew",
  "Lerengom",
  "Lertan",
  "Liadem",
  "Liathar",
  "Liephden",
  "Likellor",
  "Loightmos",
  "Loromum",
  "Lorr",
  "Lortas",
  "Lyerr",
  "Maustbur",
  "Menvor",
  "Meusten",
  "Mirodskel",
  "Morhatrod",
  "Moserbel",
  "Mosorrad",
  "Mosraye",
  "Mosth",
  "Neabar",
  "Neerdem",
  "Nichqua",
  "Nudraough",
  "Nuyim",
  "Nycha",
  "Nyia",
  "Nyjac",
  "Nystondar",
  "Okine",
  "Oldit",
  "Om'mose",
  "Onye",
  "Ososrak",
  "Pecertin",
  "Perrd",
  "Phutorny",
  "Puizlye",
  "Quirantai",
  NULL
};

// this code comes from command-line-options.cpp. the reason it's here is to
// keep consistent the extra-help formatting when '--help' used

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

const float PAGE_SCALE_FACTOR_X             ( 0.95f );
const float PAGE_SCALE_FACTOR_Y             ( 0.95f );
const float PAGE_DURATION_SCALE_FACTOR      ( 2.0f ); // time-scale factor, larger = animation is slower

const float DEFAULT_OPT_ROW_COUNT           ( 5 );
const float DEFAULT_OPT_COL_COUNT           ( 4 );
const float DEFAULT_OPT_PAGE_COUNT          ( 10 );
const bool  DEFAULT_OPT_USETABLEVIEW        ( false );

}

// This example is a benchmark that mimics the paged applications list of the homescreen app
//
class HomescreenBenchmark : public ConnectionTracker
{
public:

  // Config structure passed to the constructor. It makes easier to increase number
  // of setup parameters if needed
  struct Config
  {
    Config() :
      mRows( DEFAULT_OPT_ROW_COUNT ),
      mCols( DEFAULT_OPT_COL_COUNT ),
      mPageCount( DEFAULT_OPT_PAGE_COUNT ),
      mUseTableView( DEFAULT_OPT_USETABLEVIEW )
    {
    }

    int   mRows;
    int   mCols;
    int   mPageCount;
    bool  mUseTableView;
  };

  // animation script data
  struct ScriptData
  {
    ScriptData( int pages, float duration, bool flick )
    {
      mPages = pages;
      mDuration = duration;
      mFlick = flick;
    }

    int     mPages; // number of pages to scroll
    float   mDuration; // duration
    bool    mFlick; // use flick or 'one-by-one' scroll
  };

  HomescreenBenchmark( Application& application, const Config& config )
  : mApplication( application ),
    mConfig( config ),
    mCurrentPage( 0 )
  {
    // Connect to the Application's Init signal
    mApplication.InitSignal().Connect( this, &HomescreenBenchmark::Create );
  }

  ~HomescreenBenchmark()
  {
    // Nothing to do here;
  }

  // The Init signal is received once (only) during the Application lifetime
  void Create( Application& application )
  {
    // create benchmark script
    CreateScript();

    // Get a handle to the stage
    Stage stage = Stage::GetCurrent();

    mScrollParent = Actor::New();
    mScrollParent.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mScrollParent.SetAnchorPoint( AnchorPoint::CENTER );
    mScrollParent.SetParentOrigin( ParentOrigin::CENTER );

    // create background
    Toolkit::ImageView background = Toolkit::ImageView::New( DEMO_IMAGE_DIR "background-3.jpg");
    Stage::GetCurrent().Add(background);
    background.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    background.SetAnchorPoint( AnchorPoint::CENTER );
    background.SetParentOrigin( ParentOrigin::CENTER );

    PopulatePages();

    stage.Add(mScrollParent);

    // Respond to a click anywhere on the stage
    stage.GetRootLayer().TouchSignal().Connect( this, &HomescreenBenchmark::OnTouch );
  }

  bool OnTouch( Actor actor, const TouchData& touch )
  {
    // quit the application
    mApplication.Quit();
    return true;
  }

  Actor AddPage()
  {
    // create root page actor
    Actor pageActor;

    if( mConfig.mUseTableView )
    {
      Toolkit::TableView tableView = Toolkit::TableView::New( mConfig.mRows, mConfig.mCols );
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
    pageActor.SetSizeModeFactor(Vector3( PAGE_SCALE_FACTOR_X, PAGE_SCALE_FACTOR_Y, 1.0f ) );
    return pageActor;
  }

  void AddIconsToPage(Actor page)
  {
    Size stageSize( Stage::GetCurrent().GetSize() );
    const float scaledHeight = stageSize.y * PAGE_SCALE_FACTOR_Y;
    const float scaledWidth = stageSize.x * PAGE_SCALE_FACTOR_X;

    const float PADDING = stageSize.y / 64.0f;
    const float ROW_HEIGHT = ( scaledHeight - (PADDING*2.0f) ) / (float)mConfig.mRows;
    const float COL_WIDTH = ( scaledWidth - (PADDING*2.0f) ) / (float)mConfig.mCols;

    // the image/label area tries to make sure the positioning will be relative to previous sibling
    const float IMAGE_AREA = 0.60f;
    const float LABEL_AREA = 0.50f;

    static int currentIconIndex = 0;

    Vector2 dpi = Stage::GetCurrent().GetDpi();

    for( int y = 0; y < mConfig.mRows; ++y )
    {
      for( int x = 0; x < mConfig.mCols; ++x )
      {
        // create parent icon view
        Toolkit::Control iconView = Toolkit::Control::New();
        iconView.SetAnchorPoint( AnchorPoint::TOP_LEFT );
        iconView.SetParentOrigin( ParentOrigin::TOP_LEFT );

        if( !mConfig.mUseTableView )
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

        // create image view
        Toolkit::ImageView imageView = Toolkit::ImageView::New( IMAGE_PATH[currentIconIndex] );
        imageView.SetResizePolicy( ResizePolicy::SIZE_RELATIVE_TO_PARENT, Dimension::ALL_DIMENSIONS );
        imageView.SetSizeScalePolicy( SizeScalePolicy::FIT_WITH_ASPECT_RATIO );
        imageView.SetAnchorPoint( AnchorPoint::CENTER );
        imageView.SetParentOrigin( ParentOrigin::CENTER );
        imageView.SetSizeModeFactor( Vector3(IMAGE_AREA, IMAGE_AREA, 1.0f) );

        // create label
        Toolkit::TextLabel textLabel = Toolkit::TextLabel::New( DEMO_APPS_NAMES[currentIconIndex] );
        textLabel.SetAnchorPoint( AnchorPoint::TOP_CENTER );
        textLabel.SetParentOrigin( ParentOrigin::BOTTOM_CENTER );

        textLabel.SetResizePolicy( ResizePolicy::USE_NATURAL_SIZE, Dimension::ALL_DIMENSIONS );
        textLabel.SetProperty( Toolkit::TextLabel::Property::TEXT_COLOR, Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
        textLabel.SetProperty( Toolkit::TextLabel::Property::POINT_SIZE, (((float)( ROW_HEIGHT * LABEL_AREA ) * 72.0f)  / (dpi.y))*0.25f );
        textLabel.SetProperty( Toolkit::TextLabel::Property::HORIZONTAL_ALIGNMENT, "CENTER" );
        textLabel.SetProperty( Toolkit::TextLabel::Property::VERTICAL_ALIGNMENT, "TOP" );

        iconView.Add( imageView );
        imageView.Add( textLabel );

        page.Add(iconView);

        if( !IMAGE_PATH[ ++currentIconIndex ] )
        {
          currentIconIndex = 0;
        }
      }
    }
  }

  void CreateScript()
  {
    const int lastPage = mConfig.mPageCount-1;
    const int halfA = lastPage/2;
    const int halfB = lastPage/2 + lastPage%2;
    mScriptFrameData.push_back( ScriptData( lastPage,                1.5f,     true));
    mScriptFrameData.push_back( ScriptData( -lastPage,               1.5f,     true ));
    mScriptFrameData.push_back( ScriptData( halfA,                   1.0f,     true ));
    mScriptFrameData.push_back( ScriptData( halfB,                   1.0f,     true ));
    mScriptFrameData.push_back( ScriptData( -lastPage,               0.5f,     false ));
    mScriptFrameData.push_back( ScriptData( halfA,                   0.5f,     false ));
    mScriptFrameData.push_back( ScriptData( halfB,                   1.0f,     true ));
    mScriptFrameData.push_back( ScriptData( -halfA,                  1.0f,     true ));
    mScriptFrameData.push_back( ScriptData( 1,                       0.1f,     true ));
    mScriptFrameData.push_back( ScriptData( -1,                      0.1f,     true ));
    mScriptFrameData.push_back( ScriptData( 1,                       0.1f,     true ));
    mScriptFrameData.push_back( ScriptData( -1,                      0.1f,     true ));
    mScriptFrameData.push_back( ScriptData( 1,                       0.1f,     true ));
    mScriptFrameData.push_back( ScriptData( -1,                      0.1f,     true ));
    mScriptFrameData.push_back( ScriptData( halfA,                   1.0f,     true ));
  }

  void PopulatePages()
  {
    Vector3 stageSize( Stage::GetCurrent().GetSize() );
    const float SCALED_HEIGHT = stageSize.y * PAGE_SCALE_FACTOR_Y;

    for( int i = 0; i < mConfig.mPageCount; ++i )
    {
      // create page
      Actor page = AddPage();

      // populate icons
      AddIconsToPage(page);

      // move page 'a little bit up'
      page.SetParentOrigin( ParentOrigin::CENTER );
      page.SetAnchorPoint( AnchorPoint::CENTER );
      page.SetPosition( Vector3(stageSize.x*i, -0.30f*( stageSize.y-SCALED_HEIGHT ), 0.0f) );
      mScrollParent.Add( page );
    }

    mScrollParent.SetOpacity( 0.0f );
    mScrollParent.SetScale( Vector3(0.0f, 0.0f, 0.0f) );

    // fade in
    ShowAnimation();
  }

  void ShowAnimation()
  {
    mShowAnimation = Animation::New(1.0f);
    mShowAnimation.AnimateTo( Property(mScrollParent, Actor::Property::COLOR_ALPHA), 1.0f, AlphaFunction::EASE_IN_OUT );
    mShowAnimation.AnimateTo( Property(mScrollParent, Actor::Property::SCALE), Vector3(1.0f, 1.0f, 1.0f), AlphaFunction::EASE_IN_OUT );
    mShowAnimation.FinishedSignal().Connect( this, &HomescreenBenchmark::OnAnimationEnd );
    mShowAnimation.Play();
  }

  void ScrollPages(int pages, float duration, bool flick)
  {
    duration *= PAGE_DURATION_SCALE_FACTOR;
    Vector3 stageSize( Stage::GetCurrent().GetSize() );
    mScrollAnimation = Animation::New(duration);
    if( flick )
    {
      mScrollAnimation.AnimateBy( Property(mScrollParent, Actor::Property::POSITION), Vector3(-stageSize.x*pages, 0.0f, 0.0f), AlphaFunction::EASE_IN_OUT );
    }
    else
    {
      for(  int i = 0; i < abs(pages); ++i )
      {
        mScrollAnimation.AnimateBy( Property(mScrollParent, Actor::Property::POSITION), Vector3(pages < 0 ? stageSize.x : -stageSize.x, 0.0f, 0.0f), AlphaFunction::EASE_IN_OUT, TimePeriod(duration*i, duration) );
      }
    }
    mScrollAnimation.FinishedSignal().Connect( this, &HomescreenBenchmark::OnAnimationEnd );
    mScrollAnimation.Play();
    mCurrentPage += pages;
  }

  void OnAnimationEnd( Animation& source )
  {
    if( source == mShowAnimation )
    {
      ScriptData& frame = mScriptFrameData[0];
      ScrollPages( frame.mPages, frame.mDuration, frame.mFlick );
      mScriptFrame = 1;
    }
    else if( mScriptFrame < mScriptFrameData.size() && source == mScrollAnimation )
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

void RunTest( Application& application, const HomescreenBenchmark::Config& config, bool printHelpAndExit )
{
  HomescreenBenchmark test( application, config );

  if( printHelpAndExit )
  {
    PrintHelp( "c<num>", " Number of columns");
    PrintHelp( "r<num>", " Number of rows");
    PrintHelp( "p<num>", " Number of pages ( must be greater than 1 )");
    PrintHelp( "-use-tableview", " Uses TableView for layouting");
    return;
  }

  application.MainLoop();
}

// Entry point for Linux & Tizen applications
//
int DALI_EXPORT_API main( int argc, char **argv )
{
  // default settings
  HomescreenBenchmark::Config config;

  bool printHelpAndExit = false;

  for( int i(1) ; i < argc; ++i )
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
    else if( arg.compare( "--use-tableview" ) == 0 )
    {
      config.mUseTableView = true;
    }
    else if( arg.compare( "--help" ) == 0 )
    {
      printHelpAndExit = true;
    }

  }

  Application application = Application::New( &argc, &argv );

  RunTest( application, config, printHelpAndExit );

  return 0;
}
