<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<smc>
<general version="2.0.0.0">
<configuration active="true" id="${configurationTypeReleaseID}">
<property id="com.renesas.smc.service.project.buildArtefactType" values="com.renesas.smc.service.project.buildArtefactType.exe"/>
<toolchain id="${toolChainID}">
<option id="com.renesas.smc.toolchain.option.rtos" key="com.renesas.smc.toolchain.option.rtos.freertos">
<item id="com.renesas.smc.toolchain.option.rtos.freertos" value="AmazonFreeRTOS_RX_${packageVersion}"/>
</option>
</toolchain>
</configuration>
<platform id="${targetDevice}"/>
</general>
<tool id="Clock">
<option enabled="true" id="ckioen2" selection="unstable">
</option>
<option enabled="true" id="hypeclk" selection="textoutputitem">
</option>
<option enabled="true" id="mdclk" selection="uncheck">
</option>
<option enabled="true" id="clockfrequency" selection="textinputitem">
</option>
<option enabled="true" id="divider1" selection="select2">
</option>
<option enabled="true" id="octclk" selection="textoutputitem">
</option>
<option enabled="true" id="spiclkswitcher" selection="Bj">
</option>
<option enabled="true" id="mdclks" selection="check">
</option>
<option enabled="true" id="ckio" selection="check">
</option>
<option enabled="true" id="clockmode" selection="mode1">
</option>
<option enabled="true" id="imageclk" selection="textoutputitem">
</option>
<option enabled="true" id="octclk2" selection="textoutputitem">
</option>
<option enabled="true" id="octclkswitcher" selection="Gj">
</option>
<option enabled="true" id="clkdivider2" selection="select0">
</option>
<option enabled="true" id="pclk0" selection="textoutputitem">
</option>
<option enabled="true" id="pclk1" selection="textoutputitem">
</option>
<option enabled="true" id="hypeswitcher" selection="Gj">
</option>
<option enabled="true" id="cpuclk" selection="textoutputitem">
</option>
<option enabled="true" id="bclk" selection="textoutputitem">
</option>
<option enabled="true" id="spiclk" selection="textoutputitem">
</option>
<option enabled="true" id="eclk" selection="textoutputitem">
</option>
<option enabled="true" id="ckioen" selection="hiz">
</option>
<option enabled="true" id="clockfrequencyerror" selection="visiable">
</option>
<option enabled="true" id="eclkswitcher" selection="Bj">
</option>
<option enabled="true" id="spiclk2" selection="textoutputitem">
</option>
<option enabled="true" id="hypeclk2" selection="textoutputitem">
</option>
</tool>
<tool id="Pins" version="1.0.1.0">
</tool>
<tool enable="true" id="Mmu" import="resources\importtemplate\bootmode.xml" version="1.0.0.0">
</tool>
<tool id="Summary" version="1.0.0.0">
<option id="com.renesas.smc.code.type" value="Normal Folder"/>
</tool>
<tool id="SWComponent" version="1.0.0.0">
<configuration channel="r_adc" inuse="true" name="adc">
<component description="Dependency : r_cbuffer version(s) 1.03&#10;Analogue to digital converter driver" detailDescription="" display="r_adc" id="r_adc1.02" version="1.02">
</component>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration channel="RIIC3" inuse="true" name="riic3">
<component description="Dependency : r_cpg version(s) 1.03&#10;RIIC Driver" detailDescription="" display="r_riic" id="r_riic1.03" version="1.03">
</component>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration channel="SCIFA4" inuse="true" name="scifa4">
<component description="Dependency : r_cbuffer version(s) 1.03&#10;Dependency : r_cpg version(s) 1.03&#10;SCIFA Driver IMPORTANT - Prior to using this module please ensure os_abstraction &#10;&#9;  driver version 3.03 (or later) is included in your project (any variant)" detailDescription="" display="r_scifa" id="r_scifa2.00" version="2.00">
</component>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration channel="r_cbuffer" inuse="true" name="cbuffer0">
<component description="Dependencies : None&#10;Common software for buffer processing" detailDescription="" display="r_cbuffer" id="r_cbuffer1.03" version="1.03"/>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration channel="r_ether" inuse="true" name="ether1">
<component description="Dependencies : None&#10;Ethernet driver" detailDescription="" display="r_ether" id="r_ether1.11" version="1.11">
</component>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration channel="r_cache" inuse="true" name="cache0">
<component description="Dependencies : None&#10;Cache Module (Configure Memory using MMU TAB)" detailDescription="" display="r_cache" id="r_cache1.03" version="1.03"/>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration channel="r_iodefine" inuse="true" name="iodefine0">
<component detailDescription="" display="r_iodefine" id="r_iodefine2.02" version="2.02"/>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration channel="r_cpg" inuse="true" name="cpg0">
<component description="Dependencies : None&#10;Clock Pulse Generator Module (Configure using Clocks TAB)" detailDescription="" display="r_cpg" id="r_cpg1.03" version="1.03"/>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration channel="r_gpio" inuse="true" name="gpio0">
<component description="Dependencies : None&#10;General Purpose Input/Output Pin Module (Configure using Pins TAB)" detailDescription="" display="r_gpio" id="r_gpio1.04" version="1.04"/>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration channel="r_intc" inuse="true" name="intc0">
<component description="Dependencies : None&#10;Interrupt Controller Module" detailDescription="" display="r_intc" id="r_intc1.04" version="1.04"/>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration channel="r_mmu" inuse="true" name="mmu0">
<component description="Dependencies : None&#10;Memory Management Module (Configure Memory using MMU TAB)" detailDescription="" display="r_mmu" id="r_mmu1.03" version="1.03"/>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration channel="r_stb" inuse="true" name="stb0">
<component description="Dependencies : None&#10;Standby control Management Module" detailDescription="" display="r_stb" id="r_stb1.03" version="1.03"/>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration channel="OSTM0" inuse="true" name="ostm_reserved" required="true">
<component description="Dependency : r_cpg version(s) 1.03&#10;OS Timer Driver" detailDescription="" display="r_ostm" id="r_ostm1.05" version="1.05">
</component>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration inuse="true" name="FreeRTOS_Object">
<component display="FreeRTOS_Object" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.freertos.object"/>
<source id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
</configuration>
<configuration inuse="true" name="FreeRTOS_Kernel">
<component display="FreeRTOS_Kernel" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.freertos.amazon.kernel"/>
<source id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
</configuration>
<configuration inuse="true" name="AWS_device_shadow">
<component display="AWS_device_shadow" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.freertos.amazon.device_shadow"/>
<source id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
</configuration>
<configuration inuse="true" name="AWS_tcp_ip">
<component display="AWS_tcp_ip" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.freertos.amazon.tcp_ip"/>
<source id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
</configuration>
<configuration inuse="true" name="AWS_mqtt">
<component display="AWS_mqtt" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.freertos.amazon.mqtt"/>
<source id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
</configuration>
<configuration inuse="true" name="AWS_secure_socket">
<component display="AWS_secure_socket" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.freertos.amazon.secure_socket"/>
<source id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
</configuration>
<configuration inuse="true" name="AWS_ggd">
<component display="AWS_ggd" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.freertos.amazon.ggd"/>
<source id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
</configuration>
<configuration channel="r_os_abstraction_amazon" inuse="true" name="os_abstraction_amazon">
<component display="r_os_abstraction_amazon" id="r_os_abstraction_amazon3.03" version="3.03"/>
<source display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
</tool>
</smc>