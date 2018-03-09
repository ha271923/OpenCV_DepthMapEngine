@ECHO OFF
cls
REM.-- Prepare the Command Processor
SETLOCAL ENABLEEXTENSIONS
SETLOCAL ENABLEDELAYEDEXPANSION

REM.-- Version History --
REM         XX.XXX           YYYYMMDD Author Description
SET version=01.000-beta &rem 20051201 p.h.  initial version, providing the framework
REM !! For a new version entry, copy the last entry down and modify Date, Author and Description
SET version=%version: =%

REM.-- Set the window title
SET title=%~n0
TITLE %title%

REM.--initialize the variables by DEFAULT
set FilePersist=variables.set

set             max_disparity=160
set             vis_mult=1.0
set             window_size=-1
set             wls_lambda=8000.0
set             wls_sigma=1.5

set multi_param= --max_disparity=160 --vis_mult=1.0 --window_size=-1  --wls_lambda=8000.0 --wls_sigma=1.5 

set             output_choice=,display,no-display,
call:setPersist output=display

set             algorithm_choice=,bm,sgbm,
call:setPersist algorithm=bm

set             filter_choice=,wls_conf,wls_no_conf,
call:setPersist filter=wls_no_conf

set             downscale_choice=,downscale,no-downscale,
call:setPersist downscale=downscale

set             outputfiles_choice=,null,current,
call:setPersist outputfiles=null

set             input_choice=,IMAG0032,VR9_FOV1,IMAG0032resize,IMAG0032ppt,IMAG0032L,grayman,ambush_5,ambush_5a,ambush_5b,tsukuba,tsukuba_gray,Unity,
call:setPersist input=IMAG0032


rem.--read the persistent variables from the storage
call:restorePersistentVars "%FilePersist%"



:menuLOOP
echo.
echo.= Menu =========================================================================================================
echo.
for /f "tokens=1,2,* delims=_ " %%A in ('"findstr /b /c:":menu_" "%~f0""') do echo.  %%B  %%C
set choice=
echo.&set /p choice=Make a choice or hit ENTER to quit: ||(
    call:savePersistentVars "%FilePersist%"&   rem --save the persistent variables to the storage
    GOTO:EOF
)
echo.&call:menu_%choice%
GOTO:menuLOOP

::-----------------------------------------------------------
:: menu functions follow below here
::-----------------------------------------------------------

:menu_Options:

:menu_0   Output                       : '!output!' [!output_choice:~1,-1!]
call:getNextInList output "!output_choice!"
cls
GOTO:EOF


:menu_1   Algorithm                    : '!algorithm!' [!algorithm_choice:~1,-1!]
call:getNextInList algorithm "!algorithm_choice!"
cls
GOTO:EOF

:menu_3   Filter                       : '!filter!' [!filter_choice:~1,-1!]
call:getNextInList filter "!filter_choice!"
cls
GOTO:EOF

:menu_4   DownScale                    : '!downscale!' [!downscale_choice:~1,-1!]
call:getNextInList downscale "!downscale_choice!"
cls
GOTO:EOF

:menu_8   Save output files(CRASH)     : '!outputfiles!' [!outputfiles_choice:~1,-1!]
call:getNextInList outputfiles "!outputfiles_choice!"
cls
GOTO:EOF

:menu_9   Input image(left/right)      : '!input!' [!input_choice:~1,-1!]
call:getNextInList input "!input_choice!"
cls
GOTO:EOF

:menu_
:menu_Execute:

:menu_S   Start Run command
IF "%outputfiles%" == "current" call :getPersistentVars retVariables
echo RETURN variables = %retVariables% from !FilePersist!
echo RUN ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
call :setOutputPath
echo ximgproc-example-disparity_filtering  --!output! --!downscale! --!algorithm! --!filter! !input!_left.jpg !input!_right.jpg
echo multi_param = !multi_param!
IF "%outputfiles%" == "null"    C:\opencv\output\bin\Release\ximgproc-example-disparity_filtering  --!output! --!downscale! --!algorithm! --!filter! !multi_param! !input!_left.jpg !input!_right.jpg
IF "%outputfiles%" == "current" C:\opencv\output\bin\Release\ximgproc-example-disparity_filtering  --!output! --!downscale! --!algorithm! --!filter! !multi_param! --!dst_path! --!dst_raw_path! --!dst_conf_path! !input!_left.jpg !input!_right.jpg
echo RUN ------------------------------------------------------------------

GOTO:EOF

:menu_C   Clear Screen
cls
GOTO:EOF



::-----------------------------------------------------------
:: helper functions follow below here
::-----------------------------------------------------------

:setOutputPath -- set output file path
::                  -- %~1: file path
REM echo !outputpath!
IF "%outputfiles%" == "current" echo outputpath is Current Path
IF "%outputfiles%" == "current" set outputpath=%~dp0
IF "%outputfiles%" == "current" set dst_path=dst_path=!outputpath!
IF "%outputfiles%" == "current" set dst_raw_path=dst_raw_path=!outputpath!
IF "%outputfiles%" == "current" set dst_conf_path=dst_conf_path=!outputpath!
GOTO:EOF



:setPersist -- to be called to initialize persistent variables
::          -- %*: set command arguments
set %*
GOTO:EOF


:getPersistentVars -- returns a comma separated list of persistent variables
::                 -- %~1: reference to return variable
SETLOCAL
set retlist=
set parse=findstr /i /c:"call:setPersist" "%~f0%"^|find /v "ButNotThisLine"
for /f "tokens=2 delims== " %%a in ('"%parse%"') do (set retlist= !retlist!%%a, )
( ENDLOCAL & REM RETURN VALUES
    IF "%~1" NEQ "" SET %~1=%retlist%
)
GOTO:EOF


:savePersistentVars -- Save values of persistent variables into a file
::                  -- %~1: file name
SETLOCAL
echo.>"%~1"
call :getPersistentVars persvars
for %%a in (%persvars%) do (echo.SET %%a=!%%a!>>"%~1")
GOTO:EOF


:restorePersistentVars -- Restore the values of the persistent variables
::                     -- %~1: batch file name to restore from
if exist "%FilePersist%" call "%FilePersist%"
GOTO:EOF


:getNextInList -- return next value in list
::             -- %~1 - in/out ref to current value, returns new value
::             -- %~2 - in     choice list, must start with delimiter which must not be '@'
SETLOCAL
set lst=%~2&             rem.-- get the choice list
if "%lst:~0,1%" NEQ "%lst:~-1%" echo.ERROR Choice list must start and end with the delimiter&GOTO:EOF
set dlm=%lst:~-1%&       rem.-- extract the delimiter used
set old=!%~1!&           rem.-- get the current value
set fst=&for /f "delims=%dlm%" %%a in ("%lst%") do set fst=%%a&rem.--get the first entry
                         rem.-- replace the current value with a @, append the first value
set lll=!lst:%dlm%%old%%dlm%=%dlm%@%dlm%!%fst%%dlm%
                         rem.-- get the string after the @
for /f "tokens=2 delims=@" %%a in ("%lll%") do set lll=%%a
                         rem.-- extract the next value
for /f "delims=%dlm%" %%a in ("%lll%") do set new=%%a
( ENDLOCAL & REM RETURN VALUES
    IF "%~1" NEQ "" (SET %~1=%new%) ELSE (echo.%new%)
)
GOTO:EOF


:initProgress -- initialize an internal progress counter and display the progress in percent
::            -- %~1: in  - progress counter maximum, equal to 100 percent
::            -- %~2: in  - title string formatter, default is '[P] completed.'
set /a ProgressCnt=-1
set /a ProgressMax=%~1
set ProgressFormat=%~2
if "%ProgressFormat%"=="" set ProgressFormat=[PPPP]
set ProgressFormat=!ProgressFormat:[PPPP]=[P] completed.!
call :doProgress
GOTO:EOF


:doProgress -- display the next progress tick
set /a ProgressCnt+=1
SETLOCAL
set /a per=100*ProgressCnt/ProgressMax
set per=!per!%%
title %ProgressFormat:[P]=!per!%
GOTO:EOF




FOR /l %%a in (%~1,-1,1) do (ping -n 2 -w 1 127.0.0.1>NUL)
goto :eof