del "C:\temp\LOG\Misc\flash.log"

"C:\Program Files (x86)\Renesas Electronics\Programming Tools\Renesas Flash Programmer V3.06\rfpv3.exe" /silent "%1\vendors\renesas\boards\rx72n-envision-kit\aws_tests\flash_project\flash_project.rpj" /file "%1\projects\renesas\rx72n-envision-kit\e2studio\aws_tests\HardwareDebug\aws_tests.mot" /log "C:\temp\LOG\Misc\flash.log"
ECHO Flashing done!
ECHO Result Code: %ErrorLevel%