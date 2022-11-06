:: Place the following line:
:: call "$(SolutionDir)..\scripts\post_build.bat"
:: into VisualStudio RacoonEngine project properties -> Custom Build events -> Command line
:: and put some non-existent filename into "Outputs", then the script will be triggered always
copy /Y "Debug\RacoonEngined.exe" "..\bin\RacoonEngined.exe"
copy /Y "Release\RacoonEngine.exe" "..\bin\RacoonEngine.exe"
copy /Y "..\assets\shaders" "..\bin\ShaderLibDX"
