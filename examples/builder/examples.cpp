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
#include "shared/view.h"

#define TOKEN_STRING(x) #x

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

const char* BACKGROUND_IMAGE( "" );
const char* TOOLBAR_IMAGE( DALI_IMAGE_DIR "top-bar.png" );
const char* EDIT_IMAGE( DALI_IMAGE_DIR "icon-change.png" );
const char* EDIT_IMAGE_SELECTED( DALI_IMAGE_DIR "icon-change-selected.png" );

std::string USER_DIRECTORY;

std::string JSON_BROKEN("                                      \
{                                                              \
  'stage':                                                     \
  [                                                            \
    {                                                          \
      'type':'TextLabel',                                      \
      'size': [50,50,1],                                       \
      'parent-origin': 'CENTER',                               \
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
  std::ifstream t(fn.c_str());
  return std::string((std::istreambuf_iterator<char>(t)),
                     std::istreambuf_iterator<char>());
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
      mToolBar.AddControl( mTitleActor, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarTitlePercentage, Alignment::HorizontalCenter );
    }

    mTitleActor.SetProperty( TextLabel::Property::TEXT, title );
  }

  bool OnToolSelectLayout( Toolkit::Button button )
  {
    bool on = mItemView.IsVisible();

    if( on )
    {
      LeaveSelection();
    }
    else
    {
      EnterSelection();
    }

    return true;
  }

  void LeaveSelection()
  {

  }

  void EnterSelection()
  {
    Stage stage = Stage::GetCurrent();
    stage.SetBackgroundColor( Color::WHITE );

    mTapDetector = TapGestureDetector::New();
    mTapDetector.DetectedSignal().Connect( this, &ExampleApp::OnTap );

    if( mItemView )
    {
      stage.Remove( mItemView );
    }

    mFiles.clear();

    mItemView = ItemView::New(*this);
    stage.Add( mItemView );
    mItemView.SetParentOrigin(ParentOrigin::CENTER);
    mItemView.SetAnchorPoint(AnchorPoint::CENTER);
    mLayout = DefaultItemLayout::New( DefaultItemLayout::LIST );

    mLayout->SetItemSize( Vector3( stage.GetSize().width, 50, 1 ) );

    mItemView.AddLayout( *mLayout );

    mItemView.SetKeyboardFocusable( true );

    mFiles.clear();
    FileList files;

    if( USER_DIRECTORY.size() )
    {
      DirectoryFilesByType( USER_DIRECTORY, "json", files );
    }
    else
    {
      DirectoryFilesByType( DALI_SCRIPT_DIR, "json", files );
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

    // Display item view on the stage
    stage.Add( mItemView );

    mItemView.SetVisible( true );
    mBuilderLayer.SetVisible( false );

    SetTitle("Select");

    // Activate the layout
    Vector3 size(stage.GetSize());
    mItemView.ActivateLayout(0, size, 0.0f/*immediate*/);
  }

  void ExitSelection()
  {
    mTapDetector.Reset();

    mItemView.SetVisible( false );
    mBuilderLayer.SetVisible( true );

    SetTitle("View");
  }

  void OnTap( Actor actor, const TapGesture& tap )
  {
    ItemId id = mItemView.GetItemId( actor );

    LoadFromFileList( id );
  }

  Actor MenuItem(const std::string& text)
  {
    TextLabel label = TextLabel::New( ShortName( text ) );
    label.SetProperty( Dali::Toolkit::Control::Property::STYLE_NAME, "builderlabel" );
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
    Stage stage = Stage::GetCurrent();

    builder = Builder::New();
    builder.QuitSignal().Connect( this, &ExampleApp::OnBuilderQuit );

    Property::Map defaultDirs;
    defaultDirs[ TOKEN_STRING(DALI_IMAGE_DIR) ]  = DALI_IMAGE_DIR;
    defaultDirs[ TOKEN_STRING(DALI_MODEL_DIR) ]  = DALI_MODEL_DIR;
    defaultDirs[ TOKEN_STRING(DALI_SCRIPT_DIR) ] = DALI_SCRIPT_DIR;

    builder.AddConstants( defaultDirs );

    // render tasks may have been setup last load so remove them
    RenderTaskList taskList = stage.GetRenderTaskList();
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
      defaultTask.SetSourceActor( stage.GetRootLayer() );
      defaultTask.SetTargetFrameBuffer( FrameBufferImage() );
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

    // do this here as GetCurrentSize()
    mBuilderLayer.SetParentOrigin(ParentOrigin::CENTER);
    mBuilderLayer.SetAnchorPoint(AnchorPoint::CENTER);
    Dali::Vector3 size = Stage::GetCurrent().GetRootLayer().GetCurrentSize();
    size.y -= DemoHelper::DEFAULT_VIEW_STYLE.mToolBarHeight;
    mBuilderLayer.SetSize( size );

    mBuilderLayer.LowerToBottom();
    Stage::GetCurrent().GetRootLayer().RaiseToTop();

    ExitSelection();
  }

  void Create(Application& app)
  {
    Stage stage = Stage::GetCurrent();

    Stage::GetCurrent().KeyEventSignal().Connect(this, &ExampleApp::OnKeyEvent);

    Layer contents = DemoHelper::CreateView( app,
                                             mView,
                                             mToolBar,
                                             BACKGROUND_IMAGE,
                                             TOOLBAR_IMAGE,
                                             "" );

    SetTitle("Builder");

    mBuilderLayer = Layer::New();
    stage.GetRootLayer().Add(mBuilderLayer);


    // Create an edit mode button. (left of toolbar)
    Toolkit::PushButton editButton = Toolkit::PushButton::New();
    editButton.SetUnselectedImage( EDIT_IMAGE );
    editButton.SetSelectedImage( EDIT_IMAGE_SELECTED );
    editButton.ClickedSignal().Connect( this, &ExampleApp::OnToolSelectLayout);
    editButton.SetLeaveRequired( true );
    mToolBar.AddControl( editButton, DemoHelper::DEFAULT_VIEW_STYLE.mToolBarButtonPercentage, Toolkit::Alignment::HorizontalLeft, DemoHelper::DEFAULT_MODE_SWITCH_PADDING  );

    EnterSelection();

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
    if(event.state == KeyEvent::Down)
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE) || IsKey( event, Dali::DALI_KEY_BACK) )
      {
        if ( mItemView.IsVisible() )
        {
          mApp.Quit();
        }
        else
        {
          EnterSelection();
        }
      }
    }
  }

  /**
   * Event handler when Builder wants to quit (we only want to close the shown json unless we're at the top-level)
   */
  void OnBuilderQuit()
  {
    if ( mItemView.IsVisible() )
    {
      mApp.Quit();
    }
    else
    {
      EnterSelection();
    }
  }

private:
  Application& mApp;

  ItemLayoutPtr mLayout;
  ItemView mItemView;

  Toolkit::Control mView;
  unsigned int mOrientation;

  Toolkit::ToolBar mToolBar;
  TextLabel mTitleActor;             ///< The Toolbar's Title.

  Layer mBuilderLayer;

  Toolkit::Popup mMenu;

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
int main(int argc, char **argv)
{
  if(argc > 2)
  {
    if(strcmp(argv[1], "-f") == 0)
    {
      USER_DIRECTORY = argv[2];
    }
  }

  Application app = Application::New(&argc, &argv, DALI_DEMO_THEME_PATH);

  ExampleApp dali_app(app);

  app.MainLoop();

  return 0;
}
