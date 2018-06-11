cd ..
set envDir=%cd%\dali-env
set appPackage=%envDir%"/opt/share/com.samsung.dali-demo/res"
set csharpDemoDir=%cd%\"dali-windows-backend\csharp-demo"

setx Dali_env %envDir:\=/%
setx dali_csharp-demo %csharpDemoDir:\=/%
setx DALI_APPLICATION_PACKAGE %appPackage:\=/%
setx DALI_WINDOW_WIDTH 1920
setx DALI_WINDOW_HEIGHT 1080
@pause
