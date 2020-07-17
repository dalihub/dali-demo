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
// Run a json script layout file
//
//  - watches an named file and reloads actor tree if the file changes
//    ie run
//       builder-run layout.json
//
//       and edit layout.json in a text editor saving to trigger the reload
//
//------------------------------------------------------------------------------

#include <dali/dali.h>
#include <dali/devel-api/adaptor-framework/file-loader.h>
#include <dali-toolkit/dali-toolkit.h>
#include <dali-toolkit/devel-api/builder/builder.h>
#include <dali-toolkit/devel-api/builder/tree-node.h>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <streambuf>

#include "sys/stat.h"
#include <ctime>

#include <dali/integration-api/debug.h>

#define TOKEN_STRING(x) #x

using namespace Dali;
using namespace Dali::Toolkit;

namespace
{

std::string JSON_BROKEN("                                      \
{                                                              \
  'stage':                                                     \
  [                                                            \
    {                                                          \
      'type':'TextActor',                                      \
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

} // anon namespace


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
  explicit FileWatcher(const std::string &fn): mLastTime(0) { SetFilename(fn) ; };

  void SetFilename(const std::string &fn);
  std::string GetFilename();

  bool FileHasChanged(void);
  std::string GetFileContents(void) { return GetFileContents(mstringPath) ; };

private:
  // compiler does
  // FileWatcher(const FileWatcher&);
  // FileWatcher &operator=(const FileWatcher &);

  std::time_t mLastTime;
  std::string mstringPath;

  std::string GetFileContents(const std::string &filename)
  {
    std::streampos bufferSize = 0;
    Dali::Vector<char> fileBuffer;
    if( !Dali::FileLoader::ReadFile( filename, bufferSize, fileBuffer, FileLoader::FileType::BINARY ) )
    {
      return std::string();
    }

    return std::string( &fileBuffer[0], bufferSize );
  };
};

FileWatcher::FileWatcher(void) : mLastTime(0)
{
}

bool FileWatcher::FileHasChanged(void)
{
  struct stat buf;

  if(0 != stat(mstringPath.c_str(), &buf))
  {
    DALI_LOG_WARNING("File does not exist '%s'\n", mstringPath.c_str());
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
}

std::string FileWatcher::GetFilename(void)
{
  return mstringPath;
}


//------------------------------------------------------------------------------
//
//
//
//------------------------------------------------------------------------------
class ExampleApp : public ConnectionTracker
{
public:
  ExampleApp(Application &app) : mApp(app)
  {
    app.InitSignal().Connect(this, &ExampleApp::Create);

  }

  ~ExampleApp() {}

public:
  void SetJSONFilename(std::string const &fn) { fw.SetFilename(fn) ; };

  void Create(Application& app)
  {
    mTimer = Timer::New( 500 ); // ms
    mTimer.TickSignal().Connect( this, &ExampleApp::OnTimer);
    mTimer.Start();

    // Connect to key events in order to exit
    app.GetWindow().KeyEventSignal().Connect(this, &ExampleApp::OnKeyEvent);
  }

private:
  Application& mApp;
  Layer mRootLayer;

  FileWatcher fw;
  Timer mTimer;

  void ReloadJsonFile(Builder& builder, Layer& layer)
  {
    Window window = mApp.GetWindow();
    window.SetBackgroundColor( Color::WHITE );

    builder = Builder::New();
    builder.QuitSignal().Connect( this, &ExampleApp::OnBuilderQuit );

    Property::Map defaultDirs;
    defaultDirs[ TOKEN_STRING(DEMO_IMAGE_DIR) ]  = DEMO_IMAGE_DIR;
    defaultDirs[ TOKEN_STRING(DEMO_MODEL_DIR) ]  = DEMO_MODEL_DIR;
    defaultDirs[ TOKEN_STRING(DEMO_SCRIPT_DIR) ] = DEMO_SCRIPT_DIR;

    builder.AddConstants( defaultDirs );

    if(!layer)
    {
      layer = Layer::New();
      layer.SetProperty( Actor::Property::PARENT_ORIGIN,ParentOrigin::CENTER);
      layer.SetProperty( Actor::Property::ANCHOR_POINT,AnchorPoint::CENTER);
      layer.SetProperty( Actor::Property::SIZE, window.GetRootLayer().GetCurrentProperty< Vector3 >( Actor::Property::SIZE ) );
      window.GetRootLayer().Add(layer);

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
    }

    unsigned int numChildren = layer.GetChildCount();

    for(unsigned int i=0; i<numChildren; ++i)
    {
      layer.Remove( layer.GetChildAt(0) );
    }

    std::string data(fw.GetFileContents());

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


  bool OnTimer(void)
  {
    if(fw.FileHasChanged())
    {
      ReloadJsonFile( mBuilder, mRootLayer );
    }

    return true;
  }

  // Process Key events to Quit on back-key
  void OnKeyEvent( const KeyEvent& event )
  {
    if( event.state == KeyEvent::Down )
    {
      if( IsKey( event, Dali::DALI_KEY_ESCAPE ) || IsKey( event, Dali::DALI_KEY_BACK ) )
      {
        mApp.Quit();
      }
    }
  }

  void OnBuilderQuit()
  {
    mApp.Quit();
  }

  Builder mBuilder;
};

//------------------------------------------------------------------------------
//
//
//
//------------------------------------------------------------------------------
int DALI_EXPORT_API main(int argc, char **argv)
{

  Application dali_app = Application::New(&argc, &argv, DEMO_THEME_PATH);
  ExampleApp app(dali_app);

  if(argc > 1)
  {
    std::cout << "Loading file:" << argc << " " << argv[1] << std::endl;
    app.SetJSONFilename(argv[1]);
  }
  else
  {
    DALI_ASSERT_ALWAYS(!"Specify JSON file on command line\n");
  }

  dali_app.MainLoop();

  return 0;
}
