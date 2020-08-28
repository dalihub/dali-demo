/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

//------------------------------------------------------------------------------
//
//
//------------------------------------------------------------------------------

#include <dali/dali.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/builder/builder.h>
#include <dali-toolkit/devel-api/builder/tree-node.h>
#include <dali-toolkit/devel-api/builder/json-parser.h>
#include <dali-toolkit/devel-api/controls/popup/popup.h>
#include <dali-toolkit/devel-api/controls/navigation-view/navigation-view.h>

#include <map>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <dirent.h>
#include <stdio.h>
#include <iostream>

#include "sys/stat.h"
#include <ctime>
#include <cstring>

#include <dali/integration-api/debug.h>
#include <dali/devel-api/adaptor-framework/file-loader.h>
#include "shared/view.h"

#define TOKEN_STRING(x) #x

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const char* BACKGROUND_IMAGE( "" );
const char* TOOLBAR_IMAGE( DEMO_IMAGE_DIR "top-bar.png" );
const char* EDIT_IMAGE( DEMO_IMAGE_DIR "icon-change.png" );
const char* EDIT_IMAGE_SELECTED( DEMO_IMAGE_DIR "icon-change-selected.png" );

std::string USER_DIRECTORY;

std::string JSON_BROKEN("                                      \
{                                                              \
  'stage':                                                     \
  [                                                            \
    {                                                          \
      'type':'TextLabel',                                      \
      'size': [50,50,1],                                       \
      'parentOrigin': 'CENTER',                                \
      'text':'COULD NOT LOAD JSON FILE'                        \
    }                                                          \
  ]                                                            \
}                                                              \
");

std::string ReplaceQuotes(const std::string &single_quoted)
{
  std::string s(single_quoted);

  // wrong as no embedded quote but had regex link problems
  std::replace(s.begin(), s.end(), '\'', '"');

  return s;
}

std::string GetFileContents(const std::string &fn)
{
  std::streampos bufferSize = 0;
  Dali::Vector<char> fileBuffer;
  if( !Dali::FileLoader::ReadFile( fn, bufferSize, fileBuffer, FileLoader::FileType::BINARY ) )
  {
    return std::string();
  }

  return std::string( &fileBuffer[0], bufferSize );
};

typedef std::vector<std::string> FileList;

void DirectoryFileList(const std::string& directory, FileList& files)
{
  DIR           *d;
  struct dirent *dir;
  d = opendir(directory.c_str());
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
      if (dir->d_type == DT_REG)
      {
        files.push_back( directory + std::string(dir->d_name) );
      }
    }

    closedir(d);
  }
}

void DirectoryFilesByType(const std::string& dir, const std::string& fileType /* ie "json" */, FileList& files)
{
  typedef FileList Collection;
  typedef FileList::iterator Iter;

  Collection allFiles;
  DirectoryFileList(dir, allFiles);

  for(Iter iter = allFiles.begin(); iter != allFiles.end(); ++iter)
  {
    size_t pos = (*iter).rfind( '.' );
    if( pos != std::string::npos )
    {
      if( (*iter).substr( pos+1 ) == fileType )
      {
        files.push_back( (*iter) );
      }
    }
  }
}

const std::string ShortName( const std::string& name )
{
  size_t pos = name.rfind( '/' );

  if( pos != std::string::npos )
  {
    pos++;
    return name.substr( pos );
  }
  else
  {
    return name;
  }
}

//------------------------------------------------------------------------------
//
//
//
//------------------------------------------------------------------------------
class FileWatcher
{
public:
  FileWatcher(void);
  ~FileWatcher(void);
  explicit FileWatcher(const std::string &fn) { SetFilename(fn) ; };

  void SetFilename(const std::string &fn);
  std::string GetFilename() const;

  bool FileHasChanged(void);
  std::string GetFileContents(void) const { return ::GetFileContents(mstringPath) ; };

private:
  // compiler does
  // FileWatcher(const FileWatcher&);
  // FileWatcher &operator=(const FileWatcher &);

  std::time_t mLastTime;
  std::string mstringPath;

};

FileWatcher::FileWatcher(void) : mLastTime(0)
{
}

bool FileWatcher::FileHasChanged(void)
{
  struct stat buf;

  if(0 != stat(mstringPath.c_str(), &buf))
  {
    return false;
  }
  else
  {
    if(buf.st_mtime > mLastTime)
    {
      mLastTime = buf.st_mtime;
      return true;
    }
    else
    {
      mLastTime = buf.st_mtime;
      return false;
    }
  }

  return false;
}

FileWatcher::~FileWatcher()
{
}

void FileWatcher::SetFilename(const std::string &fn)
{
  mstringPath = fn;
  FileHasChanged(); // update last time
}

std::string FileWatcher::GetFilename(void) const
{
  return mstringPath;
}


} // anon namespace


//------------------------------------------------------------------------------
//
//
//
//------------------------------------------------------------------------------
class ExampleApp : public ConnectionTracker, public Toolkit::ItemFactory
{
public:
  ExampleApp(Application &app) : mApp(app)
  {
    app.InitSignal().Connect(this, &ExampleApp::Create);
  }

  ~ExampleApp() {}

public:

  void SetTitle(const std::string& title)
  {
    if(!mTitleActor)
    {
      mTitleActor = DemoHelper::CreateToolBarLabel( "" );
      // Add title to the tool bar.
      mToolBar.AddControl( mTitleActor, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Alignment::HORIZONTAL_CENTER );
    }

    mTitleActor.SetProperty( TextLabel::Property::TEXT, title );
  }

  bool OnBackButtonPressed( Toolkit::Button button )
  {
    OnQuitOrBack();
    return true;
  }

  void SetUpItemView()
  {
    Window window = mApp.GetWindow();

    mTapDetector = TapGestureDetector::New();
    mTapDetector.DetectedSignal().Connect( this, &ExampleApp::OnTap );

    mFiles.clear();

    mItemView = ItemView::New(*this);

    mItemView.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::CENTER);
    mItemView.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::CENTER);
    mLayout = DefaultItemLayout::New( DefaultItemLayout::LIST );

    mLayout->SetItemSize( Vector3( window.GetSize().GetWidth(), 50, 1 ) );

    mItemView.AddLayout( *mLayout );

    mItemView.SetProperty( Actor::Property::KEYBOARD_FOCUSABLE, true );

    mFiles.clear();
    FileList files;

    if( USER_DIRECTORY.size() )
    {
      DirectoryFilesByType( USER_DIRECTORY, "json", files );
    }
    else
    {
      DirectoryFilesByType( DEMO_SCRIPT_DIR, "json", files );
    }

    std::sort(files.begin(), files.end());

    ItemId itemId = 0;
    for(FileList::iterator iter = files.begin(); iter != files.end(); ++iter)
    {
      JsonParser parser = JsonParser::New();

      std::string data( GetFileContents( *iter ) );

      parser.Parse( data );

      if( parser.ParseError() )
      {
        std::cout << "Parser Error:" << *iter << std::endl;
        std::cout << parser.GetErrorLineNumber() << "(" << parser.GetErrorColumn() << "):" << parser.GetErrorDescription() << std::endl;
        exit(1);
      }

      if( parser.GetRoot() )
      {
        if( const TreeNode* node = parser.GetRoot()->Find("stage") )
        {
          // only those with a stage section
          if( node->Size() )
          {
            mFiles.push_back( *iter );

            mItemView.InsertItem( Item(itemId,
                                       MenuItem( ShortName( *iter ) ) ),
                                  0.5f );

            itemId++;
          }
          else
          {
            std::cout << "Ignored file (stage has no nodes?):" << *iter << std::endl;
          }
        }
        else
        {
          std::cout << "Ignored file (no stage section):" << *iter << std::endl;
        }
      }
    }

    // Activate the layout
    Vector3 size(window.GetSize());
    mItemView.ActivateLayout(0, size, 0.0f/*immediate*/);
  }


  void OnTap( Actor actor, const TapGesture& tap )
  {
    ItemId id = mItemView.GetItemId( actor );

    LoadFromFileList( id );
  }

  Actor MenuItem(const std::string& text)
  {
    TextLabel label = TextLabel::New( ShortName( text ) );
    label.SetStyleName( "BuilderLabel" );
    label.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::WIDTH );

    // Hook up tap detector
    mTapDetector.Attach( label );

    return label;
  }

  bool OnTimer()
  {
    if( mFileWatcher.FileHasChanged() )
    {
      LoadFromFile( mFileWatcher.GetFilename() );
    }

    return true;
  }

  void ReloadJsonFile(const std::string& filename, Builder& builder, Layer& layer)
  {
    Window window = mApp.GetWindow();

    builder = Builder::New();
    builder.QuitSignal().Connect( this, &ExampleApp::OnQuitOrBack );

    Property::Map defaultDirs;
    defaultDirs[ TOKEN_STRING(DEMO_IMAGE_DIR) ]  = DEMO_IMAGE_DIR;
    defaultDirs[ TOKEN_STRING(DEMO_MODEL_DIR) ]  = DEMO_MODEL_DIR;
    defaultDirs[ TOKEN_STRING(DEMO_SCRIPT_DIR) ] = DEMO_SCRIPT_DIR;

    builder.AddConstants( defaultDirs );

    // render tasks may have been setup last load so remove them
    RenderTaskList taskList = window.GetRenderTaskList();
    if( taskList.GetTaskCount() > 1 )
    {
      typedef std::vector<RenderTask> Collection;
      typedef Collection::iterator ColIter;
      Collection tasks;

      for(unsigned int i = 1; i < taskList.GetTaskCount(); ++i)
      {
        tasks.push_back( taskList.GetTask(i) );
      }

      for(ColIter iter = tasks.begin(); iter != tasks.end(); ++iter)
      {
        taskList.RemoveTask(*iter);
      }

      RenderTask defaultTask = taskList.GetTask(0);
      defaultTask.SetSourceActor( window.GetRootLayer() );
      defaultTask.SetFrameBuffer( FrameBuffer() );
    }

    unsigned int numChildren = layer.GetChildCount();

    for(unsigned int i=0; i<numChildren; ++i)
    {
      layer.Remove( layer.GetChildAt(0) );
    }

    std::string data(GetFileContents(filename));

    try
    {
      builder.LoadFromString(data);
    }
    catch(...)
    {
      builder.LoadFromString(ReplaceQuotes(JSON_BROKEN));
    }

    builder.AddActors( layer );
  }


  void LoadFromFileList( size_t index )
  {
    if( index < mFiles.size())
    {
      const std::string& name = mFiles[index];
      mFileWatcher.SetFilename( name );
      LoadFromFile( name );
    }
  }

  void LoadFromFile( const std::string& name )
  {
    ReloadJsonFile( name, mBuilder, mBuilderLayer );

    mBuilderLayer.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::BOTTOM_CENTER);
    mBuilderLayer.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::BOTTOM_CENTER);
    Dali::Vector3 size = mApp.GetWindow().GetRootLayer().GetCurrentProperty< Vector3 >( Actor::Property::SIZE );
    size.y -= DemoHelper::DEFAULT_VIEW_STYLE.mToolBarHeight;
    mBuilderLayer.SetProperty( Actor::Property::SIZE, size );

    mNavigationView.Push( mBuilderLayer );
  }

  void Create(Application& app)
  {
    Window window = app.GetWindow();

    window.KeyEventSignal().Connect(this, &ExampleApp::OnKeyEvent);

    Layer contents = DemoHelper::CreateView( app,
                                             mView,
                                             mToolBar,
                                             BACKGROUND_IMAGE,
                                             TOOLBAR_IMAGE,
                                             "" );

    SetTitle("Select Example");

    mBuilderLayer = Layer::New();

    // Create an edit mode button. (left of toolbar)
    Toolkit::PushButton backButton = Toolkit::PushButton::New();
    backButton.SetProperty( Toolkit::Button::Property::UNSELECTED_BACKGROUND_VISUAL, EDIT_IMAGE );
    backButton.SetProperty( Toolkit::Button::Property::SELECTED_BACKGROUND_VISUAL, EDIT_IMAGE_SELECTED );
    backButton.ClickedSignal().Connect( this, &ExampleApp::OnBackButtonPressed);
    backButton.SetProperty( Actor::Property::LEAVE_REQUIRED, true );
    mToolBar.AddControl( backButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HORIZONTAL_LEFT, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

    mNavigationView = Toolkit::NavigationView::New();
    mNavigationView.SetResizePolicy( ResizePolicy::FILL_TO_PARENT, Dimension::ALL_DIMENSIONS );
    mNavigationView.SetProperty( Actor::Property::ANCHOR_POINT, AnchorPoint::TOP_LEFT);

    window.Add( mNavigationView );

    // Set up the background gradient.
    Property::Array stopOffsets;
    stopOffsets.PushBack( 0.0f );
    stopOffsets.PushBack( 1.0f );
    Property::Array stopColors;
    stopColors.PushBack( Color::WHITE );
    stopColors.PushBack( Vector4( 0.45f, 0.70f, 0.80f, 1.0f ) ); // Medium bright, pastel blue
    const float percentageWindowHeight = window.GetSize().GetHeight() * 0.6f;

    mNavigationView.SetProperty( Toolkit::Control::Property::BACKGROUND, Dali::Property::Map()
      .Add( Toolkit::Visual::Property::TYPE, Dali::Toolkit::Visual::GRADIENT )
      .Add( Toolkit::GradientVisual::Property::STOP_OFFSET, stopOffsets )
      .Add( Toolkit::GradientVisual::Property::STOP_COLOR, stopColors )
      .Add( Toolkit::GradientVisual::Property::START_POSITION, Vector2( 0.0f, -percentageWindowHeight ) )
      .Add( Toolkit::GradientVisual::Property::END_POSITION, Vector2( 0.0f, percentageWindowHeight ) )
      .Add( Toolkit::GradientVisual::Property::UNITS, Toolkit::GradientVisual::Units::USER_SPACE ) );

    SetUpItemView();
    mNavigationView.Push( mItemView );

    mTimer = Timer::New( 500 ); // ms
    mTimer.TickSignal().Connect( this, &ExampleApp::OnTimer);
    mTimer.Start();

  } // Create(app)

  virtual unsigned int GetNumberOfItems()
  {
    return mFiles.size();
  }

  virtual Actor NewItem(unsigned int itemId)
  {
    DALI_ASSERT_DEBUG( itemId < mFiles.size() );
    return MenuItem( ShortName( mFiles[itemId] ) );
  }

  /**
   * Main key event handler
   */
  void OnKeyEvent(const KeyEvent& event)
  {
    if(event.GetState() == KeyEvent::DOWN)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        OnQuitOrBack();
      }
    }
  }

  /**
   * Event handler when Builder wants to quit (we only want to close the shown json unless we're at the top-level)
   */
  void OnQuitOrBack()
  {
    if ( mItemView.GetProperty< bool >( Actor::Property::CONNECTED_TO_SCENE ) )
    {
      mApp.Quit();
    }
    else
    {
      mNavigationView.Pop();
    }
  }

private:
  Application& mApp;

  ItemLayoutPtr mLayout;
  ItemView mItemView;
  Toolkit::NavigationView mNavigationView;

  Toolkit::Control mView;

  Toolkit::ToolBar mToolBar;
  TextLabel mTitleActor;

  Layer mBuilderLayer;

  TapGestureDetector mTapDetector;

  // builder
  Builder mBuilder;

  FileList mFiles;

  FileWatcher mFileWatcher;
  Timer mTimer;
};

//------------------------------------------------------------------------------
//
//
//
//------------------------------------------------------------------------------
int DALI_EXPORT_API main(int argc, char **argv)
{
  if(argc > 2)
  {
    if(strcmp(argv[1], "-f") == 0)
    {
      USER_DIRECTORY = argv[2];
    }
  }

  Application app = Application::New(&argc, &argv, DEMO_THEME_PATH);

  ExampleApp dali_app(app);

  app.MainLoop();

  return 0;
}
