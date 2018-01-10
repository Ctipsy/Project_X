::dot脚本自动生成图片文件

cd /d %~dp0
set curPath=D:\Program Files (x86)\X_Ship
set dotPath=graphviz-2.38\release
set sourcePath=image
set var=%1%

::debug
echo cd=%cd%
echo dp0=%~dp0
::pause

::%dotPath%\bin\dot.exe -Tjpg qingya.dot -o %sourcePath%\chuzhongtao.jpg

::%cd%
::%curPath%\graphviz-2.38\release\bin\dot.exe -Tjpg %curPath%\qingya.dot -o %curPath%\image\chuzhongtao.jpg


::~dp0
::%dotPath%\bin\dot.exe -Tjpg dot\internetDot.dot -o %sourcePath%\internetDot.jpg
%dotPath%\bin\dot.exe -Tsvg dot\%var%.dot -o %sourcePath%\%var%.svg

::pause