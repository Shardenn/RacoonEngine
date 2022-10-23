:: Place the following line:
:: call "$(SolutionDir)..\scripts\post_build.bat"
:: into VisualStudio RacoonEngine project properties -> Build events -> Post build events
copy /Y "Debug\RacoonEngined.exe" "..\bin\RacoonEngined.exe"
copy /Y "..\assets\shaders" "..\bin\ShaderLibDX"