>  PROGRAM: C_SDK_example.cpp          
>                                   
>  SOLE PROPERTY OF APPDYNAMICS, INC.  ALL RIGHTS RESERVED.
>       
>  This program is soley for instructional use.  Its purpose is only to indicate how one "might" mplement the C SDK Agent calls.   This software will not be supported by AppDynnamics nor isnthere any guarantee, implied or otherwise, that it will work.   Use at your own risk.


# README - How to use

## Build

1. The build files (gradle, VC++) expect the appdynamics sdk to unpacked into extlibs, i.e., `extlibs/appdynamics-sdk-native`
2. Edit `src/main/cpp/C_SDK_Example.c` and modify cfg structure to reflect your controller connection parameters. Starts around line 94.
3. Build Executable
    1. **gradle**:
    	1. run `gradle wrapper`
    	2. run `gradlew build`
    2. **VC++**:
      1. open `C-SDK-Example.sln` in Visual Studio
      2. build solution

## SDK Setup

**Linux**:
1. `cd extlibs/appdynamics-sdk-native`
2. run `chmod 777 logs`
3. run `./install.sh`

**Windows**:
1. open extlibs\appdynamics-sdk-native in finder
2. double-click `install.bat`

## Run

### Linux
1. cd extlibs/appdynamics-sdk-native
2. run `nohup ./runSDKProxy.sh 1>logs/proxy.out 2>&1 &` or, better yet, run ./runSDKProxy.sh in the foreground in it's own terminal window.
3. run ``LD_LIBRARY_PATH=`pwd`/extlibs/appdynamics-sdk-native/sdk-lib/lib ./build/exe/main/debug/C_SDK_Example``

### Windows
1. Run in VC++

## Extras

Included in the code are two variables that can change the behavior of the example:

1. **loop_cnt_total**: This controls the number of BTs produced in a single run.
2. **error_rate_pct**: This controller the percentage of the BTs that are registered as an error.

The gradle build should also work on windows with VC++ < 2015. Gradle has not yet added support for Visual Studio 2015
