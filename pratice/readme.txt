chipmunk prebuild

if not exist "$(OutDir)" mkdir "$(OutDir)"
xcopy /Y /Q "$(ProjectDir)..\..\glew\lib\Release\Win32\*.*" "$(OutDir)"
xcopy /Y /Q "$(ProjectDir)..\..\glew\bin\Release\Win32\*.*" "$(OutDir)"
xcopy /Y /Q "$(ProjectDir)..\..\glfw\lib-msvc100\*.*" "$(OutDir)"