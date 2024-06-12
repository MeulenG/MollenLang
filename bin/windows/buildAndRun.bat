@ECHO OFF
ECHO BUILDING MOLLENLANG AND RUNNING TESTS
REM Check if .git directory exists in the current directory
if exist ".git" (
    echo You are in the root of a Git project.
) else (
    echo You are NOT in the root of a Git project.
    exit
)
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
cmake --build . --config RelWithDebInfo
PAUSE