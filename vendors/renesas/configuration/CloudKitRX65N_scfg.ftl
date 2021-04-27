<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<smc>
<general version="2.0.0.0">
<configuration active="true" id="${configurationTypeReleaseID}">
<property id="com.renesas.smc.service.project.buildArtefactType" values="com.renesas.smc.service.project.buildArtefactType.exe"/>
<toolchain id="${toolChainID}">
<option id="com.renesas.smc.toolchain.option.buildArtefactType" key="com.renesas.smc.toolchain.option.buildArtefactType.exe"/>
<option id="com.renesas.smc.toolchain.option.rtos" key="com.renesas.smc.toolchain.option.rtos.freertos">
<item id="com.renesas.smc.toolchain.option.rtos.freertos" value="AmazonFreeRTOS_RX_${packageVersion}"/>
</option>
</toolchain>
</configuration>
<platform id="${targetDevice}"/>
<option id="board" value="Custom User Board"/>
</general>
<tool id="Summary" version="1.0.0.0">
<option id="com.renesas.smc.code.type" value="Normal Folder"/>
</tool>
<tool id="SWComponent" version="1.0.0.0">
<configuration inuse="true" name="r_bsp">
<component display="r_bsp" id="r_bsp5.52" version="5.52">
<gridItem id="BSP_CFG_CODE_FLASH_BANK_MODE" selectedIndex="0"/>
<gridItem id="BSP_CFG_USER_STACK_ENABLE" selectedIndex="1"/>
<gridItem id="BSP_CFG_SWINT_UNIT2_ENABLE" selectedIndex="1"/>
</component>
<source id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration inuse="true" name="r_wifi_sx_ulpgn">
<component description="Dependency : r_bsp version(s) 5.20&#10;Dependency : r_byteq version(s) 1.80&#10;Dependency : r_sci_rx version(s) 3.20&#10;Use UART to control SX-ULPGN and connect to the Internet." detailDescription="Wi-Fi Module control functions for Renesas MCUs." display="r_wifi_sx_ulpgn" id="r_wifi_sx_ulpgn1.10" version="1.10">
<gridItem id="WIFI_CFG_SCI_CHANNEL" selectedIndex="0"/>
<gridItem id="WIFI_CFG_SCI_SECOND_CHANNEL" selectedIndex="1"/>
<gridItem id="WIFI_CFG_SCI_INTERRUPT_LEVEL" selectedIndex="4"/>
<gridItem id="WIFI_CFG_SCI_BAUDRATE" selectedIndex="460800"/>
<gridItem id="WIFI_CFG_RESET_PORT" selectedIndex="13"/>
<gridItem id="WIFI_CFG_RESET_PIN" selectedIndex="0"/>
<gridItem id="WIFI_CFG_RTS_PORT" selectedIndex="2"/>
<gridItem id="WIFI_CFG_RTS_PIN" selectedIndex="2"/>
<gridItem id="WIFI_CFG_CREATABLE_SOCKETS" selectedIndex="4"/>
<gridItem id="WIFI_CFG_SOCKETS_RECEIVE_BUFFER_SIZE" selectedIndex="8192"/>
<gridItem id="WIFI_CFG_USE_CALLBACK_FUNCTION" selectedIndex="0"/>
<gridItem id="WIFI_CFG_CALLBACK_FUNCTION_NAME" selectedIndex="NULL"/>
</component>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration inuse="true" name="r_s12ad_rx">
<component display="r_s12ad_rx" id="r_s12ad_rx4.50" version="4.50">
<gridItem id="S12AD0" selectedIndex="1"/>
<gridItem id="S12AD0" selectedIndex="0"/>
</component>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration inuse="true" name="r_flash_rx">
<component display="r_flash_rx" id="r_flash_rx4.50" version="4.50">
<gridItem id="FLASH_CFG_PARAM_CHECKING_ENABLE" selectedIndex="1"/>
<gridItem id="FLASH_CFG_CODE_FLASH_ENABLE" selectedIndex="1"/>
<gridItem id="FLASH_CFG_DATA_FLASH_BGO" selectedIndex="1"/>
<gridItem id="FLASH_CFG_CODE_FLASH_BGO" selectedIndex="1"/>
<gridItem id="FLASH_CFG_CODE_FLASH_RUN_FROM_ROM" selectedIndex="1"/>
</component>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration inuse="true" name="r_sci_rx">
<component display="r_sci_rx" id="r_sci_rx3.40" version="3.40">
<gridItem id="SCI_CFG_CH1_INCLUDED" selectedIndex="1"/>
<gridItem id="SCI_CFG_CH8_INCLUDED" selectedIndex="0"/>
<gridItem id="RXD0/SMISO0" selectedIndex="1"/>
<gridItem id="TXD0/SMOSI0" selectedIndex="1"/>
<gridItem id="CTS0#/RTS0#/SS0#" selectedIndex="1"/>
<gridItem id="SCI0" selectedIndex="1"/>
<gridItem id="RXD1/SMISO1" selectedIndex="1"/>
<gridItem id="TXD1/SMOSI1" selectedIndex="1"/>
<gridItem id="SCI1" selectedIndex="1"/>
<gridItem id="RXD5/SMISO5" selectedIndex="1"/>
<gridItem id="TXD5/SMOSI5" selectedIndex="1"/>
<gridItem id="SCI5" selectedIndex="1"/>
<gridItem id="RXD8/SMISO8/SSCL8" selectedIndex="0"/>
<gridItem id="TXD8/SMOSI8/SSDA8" selectedIndex="0"/>
<gridItem id="SCI8" selectedIndex="0"/>
<gridItem id="SCI_CFG_CH0_INCLUDED" selectedIndex="1"/>
<gridItem id="SCI_CFG_CH1_INCLUDED" selectedIndex="1"/>
<gridItem id="SCI_CFG_CH5_INCLUDED" selectedIndex="1"/>
<gridItem id="SCI_CFG_CH8_INCLUDED" selectedIndex="0"/>
<gridItem id="SCI_CFG_CH0_TX_BUFSIZ" selectedIndex="2180"/>
<gridItem id="SCI_CFG_CH1_TX_BUFSIZ" selectedIndex="2180"/>
<gridItem id="SCI_CFG_CH1_RX_BUFSIZ" selectedIndex="4096"/>
<gridItem id="SCI_CFG_CH2_RX_BUFSIZ" selectedIndex="4096"/>
</component>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration inuse="true" name="r_byteq">
<component display="r_byteq" id="r_byteq1.80" version="1.80"/>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration inuse="true" name="FreeRTOS_Object">
<component display="FreeRTOS_Object" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.freertos.object"/>
<source id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
</configuration>
<configuration inuse="true" name="FreeRTOS_Kernel">
<component display="FreeRTOS_Kernel" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.freertos.amazon.kernel">
<gridItem id="configTOTAL_HEAP_SIZE" selectedIndex="( size_t ) ( 256U * 1024U )"/>
</component>
<source id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
</configuration>
<configuration inuse="true" name="AWS_device_shadow">
<component display="AWS_device_shadow" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.freertos.amazon.device_shadow"/>
<source id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
</configuration>
<configuration inuse="true" name="AWS_tcp_ip">
<component display="AWS_tcp_ip" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.freertos.amazon.tcp_ip">
<gridItem id="ipconfigINCLUDE_FULL_INET_ADDR" selectedIndex="0"/>
<gridItem id="ipconfigNUM_NETWORK_BUFFER_DESCRIPTORS" selectedIndex="8"/>
<gridItem id="ipconfigUSE_TCP_WIN" selectedIndex="0"/>
</component>
<source id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
</configuration>
<configuration inuse="true" name="AWS_mqtt">
<component display="AWS_mqtt" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.freertos.amazon.mqtt">
<gridItem id="mqttconfigMQTT_TASK_STACK_DEPTH" selectedIndex="6114"/>
</component>
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
</tool>
<tool id="Clock">
<option enabled="true" id="mainclockenable" selection="uncheck">
</option>
<option enabled="false" id="mainsourcebox" selection="srcR">
</option>
<option enabled="false" id="mainfrequency" selection="textinputitem">
</option>
<option enabled="false" id="mainwaittime" selection="textinputitem">
</option>
<option enabled="true" id="hococlockenable" selection="check">
</option>
<option enabled="true" id="hocofrequency" selection="frq20">
</option>
<option enabled="true" id="hocooscenable" selection="uncheck">
</option>
<option enabled="true" id="pllswitcher" selection="pllhoco">
<item enabled="false" id="pllmain" input="" value="24.0"/>
<item enabled="true" id="pllhoco" input="" value="20"/>
</option>
<option enabled="true" id="plldivider" selection="div1-1">
<item enabled="true" id="div1-1"/>
<item enabled="true" id="div1-2"/>
<item enabled="false" id="div1-3"/>
</option>
<option enabled="true" id="pllmul" selection="mul12-1">
<item enabled="true" id="mul10_5-1"/>
<item enabled="true" id="mul11-1"/>
<item enabled="true" id="mul11_5-1"/>
<item enabled="true" id="mul12-1"/>
</option>
<option enabled="true" id="sckswitcher" selection="pll">
<item enabled="false" id="main" input="" value="2.4E7"/>
<item enabled="true" id="hoco" input="" value="20000000"/>
</option>
<option enabled="true" id="uclk" selection="textoutputitem">
<item enabled="false" id="textoutputitem" input="" value="48.0"/>
</option>
<option enabled="true" id="cachclk" selection="textoutputitem">
<item enabled="true" id="textoutputitem" input="" value="20"/>
</option>
<option enabled="false" id="cacmclk" selection="textoutputitem">
<item enabled="true" id="textoutputitem" input="" value="24.0"/>
</option>
<Item current="true" id="uckdivider.selectBox"/>
<Item current="240.0 MHz" id="uckdivider.InputValue"/>
<Item current="48.0 MHz" id="uckdivider.OutputValue"/>
<Item current="sckselector.radiobutton.swtpll" id="uckdivider.Condition"/>
<Item current="comboBox1-5" error="false" id="uckdivider.comboBox"/>
</tool>
<tool id="Pins" version="1.0.1.0">
<pinItem allocation="11" comments="" direction="None" id="XTAL" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="4" comments="" direction="None" id="SS0#" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="20" comments="" direction="None" id="RXD1" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="18" comments="" direction="None" id="SMOSI0" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="49" comments="" direction="None" id="SMOSI5" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="22" comments="" direction="None" id="TXD1" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="13" comments="" direction="None" id="EXTAL" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="17" comments="" direction="None" id="SMISO0" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="50" comments="" direction="None" id="SMISO5" isUsedBySoftware="true" locked="false" status="0"/>
</tool>
</smc>
