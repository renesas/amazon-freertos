@echo off
rem ################################################################################
rem # This batch file executes some preprocess for build and then executes the make
rem ################################################################################

if not exist "%~dp0..\..\..\..\..\..\..\..\lib\third_party\mcu_vendor\renesas\rx_mcu_boards\tools\aws_demos_build_path_check_and_make_for_ota.bat" (
    echo ERROR: Unable to find "%~dp0..\..\..\..\..\..\..\..\lib\third_party\mcu_vendor\renesas\rx_mcu_boards\tools\aws_demos_build_path_check_and_make_for_ota.bat"
    exit 2
)

"%~dp0..\..\..\..\..\..\..\..\lib\third_party\mcu_vendor\renesas\rx_mcu_boards\tools\aws_demos_build_path_check_and_make_for_ota.bat" %*
