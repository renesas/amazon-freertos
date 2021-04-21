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
<gridItem id="BSP_CFG_USER_STACK_ENABLE" selectedIndex="0"/>
<gridItem id="BSP_CFG_ISTACK_BYTES" selectedIndex="0x1000"/>
<gridItem id="BSP_CFG_HEAP_BYTES" selectedIndex="0x1000"/>
<gridItem id="BSP_CFG_CODE_FLASH_BANK_MODE" selectedIndex="0"/>
<gridItem id="BSP_CFG_SWINT_UNIT2_ENABLE" selectedIndex="0"/>
<gridItem id="BSP_CFG_ROM_CACHE_ENABLE" selectedIndex="0"/>
</component>
<source id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration inuse="true" name="r_s12ad_rx">
<component display="r_s12ad_rx" id="r_s12ad_rx4.50" version="4.50"></component>
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
<gridItem id="RXD2/SMISO2" selectedIndex="1"/>
<gridItem id="TXD2/SMOSI2" selectedIndex="1"/>
<gridItem id="SCI2" selectedIndex="1"/>
<gridItem id="RXD7/SMISO7" selectedIndex="1"/>
<gridItem id="TXD7/SMOSI7" selectedIndex="1"/>
<gridItem id="SCI7" selectedIndex="1"/>
<gridItem id="SCI_CFG_TEI_INCLUDED" selectedIndex="1"/>
<gridItem id="SCI_CFG_CH2_INCLUDED" selectedIndex="1"/>
<gridItem id="SCI_CFG_CH7_INCLUDED" selectedIndex="1"/>
<gridItem id="SCI_CFG_CH1_INCLUDED" selectedIndex="0"/>
</component>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration inuse="true" name="r_byteq">
<component display="r_byteq" id="r_byteq1.80" version="1.80"></component>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration inuse="true" name="r_ether_rx">
<component display="r_ether_rx" id="r_ether_rx1.20" version="1.20">
<gridItem id="CLKOUT25M" selectedIndex="1"/>
<gridItem id="ET0_TX_CLK" selectedIndex="1"/>
<gridItem id="ET0_RX_CLK" selectedIndex="1"/>
<gridItem id="ET0_TX_EN" selectedIndex="1"/>
<gridItem id="ET0_ETXD3" selectedIndex="1"/>
<gridItem id="ET0_ETXD2" selectedIndex="1"/>
<gridItem id="ET0_ETXD1" selectedIndex="1"/>
<gridItem id="ET0_ETXD0" selectedIndex="1"/>
<gridItem id="ET0_TX_ER" selectedIndex="1"/>
<gridItem id="ET0_RX_DV" selectedIndex="1"/>
<gridItem id="ET0_ERXD3" selectedIndex="1"/>
<gridItem id="ET0_ERXD2" selectedIndex="1"/>
<gridItem id="ET0_ERXD1" selectedIndex="1"/>
<gridItem id="ET0_ERXD0" selectedIndex="1"/>
<gridItem id="ET0_RX_ER" selectedIndex="1"/>
<gridItem id="ET0_CRS" selectedIndex="1"/>
<gridItem id="ET0_COL" selectedIndex="1"/>
<gridItem id="ET0_MDC" selectedIndex="1"/>
<gridItem id="ET0_MDIO" selectedIndex="1"/>
<gridItem id="ET0_MDC" selectedIndex="1"/>
<gridItem id="ET0_MDIO" selectedIndex="1"/>
<gridItem id="CLKOUT25M" selectedIndex="1"/>
<gridItem id="ETHERC0_MII" selectedIndex="1"/>
<gridItem id="ETHER_CFG_CH0_PHY_ACCESS" selectedIndex="0"/>
<gridItem id="ETHER_CFG_CH1_PHY_ACCESS" selectedIndex="0"/>
<gridItem id="ETHER_CFG_USE_LINKSTA" selectedIndex="0"/>
<gridItem id="ETHER_CFG_CH0_PHY_ADDRESS" selectedIndex="0"/>
<gridItem id="ETHER_CFG_CH1_PHY_ADDRESS" selectedIndex="1"/>
<gridItem id="ETHER_CFG_EMAC_RX_DESCRIPTORS" selectedIndex="4"/>
<gridItem id="ETHER_CFG_EMAC_TX_DESCRIPTORS" selectedIndex="4"/>
<gridItem id="ETHER_CFG_BUFSIZE" selectedIndex="1536"/>
<gridItem id="ETHER_CFG_AL1_INT_PRIORTY" selectedIndex="13"/>
<gridItem id="ETHER_CFG_CH1_PHY_ACCESS" selectedIndex="1"/>
<gridItem id="ETHER_CFG_USE_PHY_KSZ8041NL" selectedIndex="1"/>
</component>
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
<Item current="16 MHz" id="mainclock.OutputValue"/>
<Item current="16" error="false" id="mainclock.frequencyTxt"/>
<Item current="16 MHz" id="pllSourceSelect.InputValue"/>
<Item current="16 MHz" id="pllSourceSelect.OutputValue"/>
<Item current="16 MHz" id="pll.InputValue"/>
<Item current="mul15-1" error="false" id="pll.multiplyBox"/>
<Item current="16.0 MHz" error="false" id="pll.MiddleValue"/>
<Item current="16 MHz" id="ppll.InputValue"/>
<Item current="mul12_5-1" error="false" id="ppll.multiplyBox"/>
<Item current="div1-1" error="false" id="ppll.dividerBox"/>
<Item current="16.0 MHz" error="false" id="ppll.MiddleValue"/>
<Item current="16 MHz" id="ckoselector.InputValue"/>
<Item current="16 MHz" id="ckoselector.OutputValue"/>
<Item current="16 MHz" id="ckodivider.InputValue"/>
<Item current="16.0 MHz" id="ckodivider.OutputValue"/>
<Item current="16 MHz" error="false" id="ckodivider.PreOutput"/>
<Item current="120.0 MHz" id="bck.InputValue"/>
<Item current="120.0 MHz" id="bck.OutputValue"/>
<Item current="120.0" id="bck.frequencyTxt"/>
<Item current="120.0" id="bck.CurrentValue"/>
<Item current="120.0 MHz" id="sdclk.InputValue"/>
<Item current="true" id="sdclk.SelectBoxDisabled"/>
<Item current="16.0 MHz" id="clkout.InputValue"/>
<Item current="16.0" id="clkout.CurrentValue"/>
<Item current="16 MHz" id="cacmclk.InputValue"/>
<Item current="16 MHz" id="cacmclk.OutputValue"/>
<Item current="16" id="cacmclk.frequencyTxt"/>
<Item current="16" id="cacmclk.CurrentValue"/>
<Item current="true" id="bckdivider.isSelected"/>
<Item current="240.0 MHz" id="bckdivider.InputValue"/>
<Item current="120.0 MHz" id="bckdivider.OutputValue"/>
<Item current="sckselector.radiobutton.swtpll" id="bckdivider.Condition"/>
<Item current="comboBox1-2" error="false" id="bckdivider.comboBox"/>
<Item current="240.0 MHz" error="false" id="bckdivider.PreOutput"/>
<Item current="iclkdivider" error="false" id="bckdivider.PreOutputController"/>
<Item current="false" id="bckselector.selectBox"/>
<Item current="120.0 MHz" id="bckselector.InputValue"/>
<Item current="" id="bckselector.OutputValue"/>
<Item current="comboBox1-2" error="false" id="bckselector.comboBox"/>
<Item current="true" id="uckdivider.selectBox"/>
<Item current="240.0 MHz" id="uckdivider.InputValue"/>
<Item current="48.0 MHz" id="uckdivider.OutputValue"/>
<Item current="sckselector.radiobutton.swtpll" id="uckdivider.Condition"/>
<Item current="comboBox1-5" error="false" id="uckdivider.comboBox"/>
</tool>
<tool id="Pins" version="1.0.1.0">
<pinItem allocation="128" comments="" direction="None" id="RXD7" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="131" comments="" direction="None" id="TXD7" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="45" comments="" direction="None" id="RXD2" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="44" comments="" direction="None" id="TXD2" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="68" comments="" direction="None" id="ET0_RX_ER" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="61" comments="" direction="None" id="ET0_ETXD3" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="62" comments="" direction="None" id="ET0_ETXD2" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="63" comments="" direction="None" id="ET0_ETXD1" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="64" comments="" direction="None" id="ET0_ETXD0" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="69" comments="" direction="None" id="ET0_RX_CLK" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="75" comments="" direction="None" id="ET0_ERXD3" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="73" comments="" direction="None" id="ET0_ERXD2" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="72" comments="" direction="None" id="ET0_ERXD1" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="71" comments="" direction="None" id="ET0_ERXD0" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="66" comments="" direction="None" id="ET0_TX_CLK" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="86" comments="" direction="None" id="ET0_MDIO" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="50" comments="" direction="None" id="CLKOUT25M" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="20" comments="" direction="None" id="XTAL" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="65" comments="" direction="None" id="ET0_TX_EN" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="67" comments="" direction="None" id="ET0_TX_ER" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="85" comments="" direction="None" id="ET0_MDC" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="60" comments="" direction="None" id="ET0_COL" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="58" comments="" direction="None" id="ET0_CRS" isUsedBySoftware="false" locked="false" status="0"/>
<pinItem allocation="22" comments="" direction="None" id="EXTAL" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="70" comments="" direction="None" id="ET0_RX_DV" isUsedBySoftware="false" locked="false" status="0"/>
</tool>
</smc>
