xcopy /s/y/i/f/d "..\dali-adaptor\dali\integration-api\*.h" "..\dali-env\opt\include\dali\integration-api\adaptors"
xcopy /s/y/i/f/d "..\dali-env\opt\include\dali\integration-api\adaptors\windows\*.*" "..\dali-env\opt\include\dali\integration-api\adaptors"
copy /d "..\dali-adaptor\dali\public-api\dali.h" "..\dali-env\opt\include\dali\dali.h"

xcopy /s/y/i/f/d "..\dali-toolkit\dali-toolkit\styles\*.json" "..\dali-env\opt\share\dali\toolkit\styles"
xcopy /s/y/i/f/d "..\dali-toolkit\dali-toolkit\styles\480x800\*.json" "..\dali-env\opt\share\dali\toolkit\styles"
xcopy /s/y/i/f/d "..\dali-toolkit\dali-toolkit\styles\480x800\images\*.png" "..\dali-env\opt\share\dali\toolkit\styles\images"
xcopy /s/y/i/f/d "..\dali-toolkit\dali-toolkit\styles\images-common\*.png" "..\dali-env\opt\share\dali\toolkit\images"
xcopy /s/y/i/f/d "..\dali-toolkit\dali-toolkit\sounds\*.ogg" "..\dali-env\opt\share\dali\toolkit\sounds"

xcopy /s/y/i/f/d "resources\game\*.*" "..\dali-env\opt\share\com.samsung.dali-demo\res\game"
xcopy /s/y/i/f/d "resources\images\*.*" "..\dali-env\opt\share\com.samsung.dali-demo\res\images"
xcopy /s/y/i/f/d "resources\models\*.*" "..\dali-env\opt\share\com.samsung.dali-demo\res\models"
xcopy /s/y/i/f/d "resources\scripts\*.json" "..\dali-env\opt\share\com.samsung.dali-demo\res\scripts"
xcopy /s/y/i/f/d "resources\shaders\*.*" "..\dali-env\opt\share\com.samsung.dali-demo\res\shaders"
xcopy /s/y/i/f/d "resources\style\*.json" "..\dali-env\opt\share\com.samsung.dali-demo\res\style"
xcopy /s/y/i/f/d "resources\style\images\*.png" "..\dali-env\opt\share\com.samsung.dali-demo\res\style\images"
xcopy /s/y/i/f/d "resources\videos\*.*" "..\dali-env\opt\share\com.samsung.dali-demo\res\videos"
xcopy /s/y/i/f/d "..\dali-adaptor\dali-windows-backend\ExDll\*.dll" "..\bin\Debug"

copy "CMakeLists.txt" "../CMakeLists.txt"

cd ..
set appPackage=%envDir%"/opt/share/com.samsung.dali-demo/res"
set csharpDemoDir=%cd%\"dali-windows-backend\csharp-demo"

setx dali_csharp-demo %csharpDemoDir:\=/%
setx DALI_APPLICATION_PACKAGE %appPackage:\=/%
setx DALI_WINDOW_WIDTH 1920
setx DALI_WINDOW_HEIGHT 1080
@pause