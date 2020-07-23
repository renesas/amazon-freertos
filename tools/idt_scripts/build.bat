del /Q "%1\projects\renesas\rx72n-envision-kit\e2studio\aws_tests\HardwareDebug\*.*"

"C:\Renesas\e2_studio_v7_8\eclipse\eclipsec.exe" -nosplash --launcher.suppressErrors -application org.eclipse.cdt.managedbuilder.core.headlessbuild -data %1\temp -import %1\projects\renesas\rx72n-envision-kit\e2studio\aws_tests\ -cleanBuild all
ECHO Result Code: %ErrorLevel%