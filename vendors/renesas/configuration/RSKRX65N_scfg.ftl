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
<configuration inuse="true" name="r_s12ad_rx">
<component display="r_s12ad_rx" id="r_s12ad_rx4.50" version="4.50">
<gridItem id="S12AD0" selectedIndex="1"/>
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
<gridItem id="SCI_CFG_CH1_INCLUDED" selectedIndex="0"/>
<gridItem id="SCI_CFG_CH8_INCLUDED" selectedIndex="1"/>
<gridItem id="SCK1" selectedIndex="1"/>
<gridItem id="SCI1" selectedIndex="1"/>
<gridItem id="SCK6" selectedIndex="1"/>
<gridItem id="SCI6" selectedIndex="1"/>
<gridItem id="RXD8/SMISO8/SSCL8" selectedIndex="1"/>
<gridItem id="TXD8/SMOSI8/SSDA8" selectedIndex="1"/>
<gridItem id="SCI8" selectedIndex="1"/>
<gridItem id="SCI_CFG_TEI_INCLUDED" selectedIndex="1"/>
</component>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration inuse="true" name="r_byteq">
<component display="r_byteq" id="r_byteq1.80" version="1.80"/>
<source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
</configuration>
<configuration inuse="true" name="r_ether_rx">
<component display="r_ether_rx" id="r_ether_rx1.20" version="1.20">
<gridItem id="ETHER_CFG_CH0_PHY_ACCESS" selectedIndex="0"/>
<gridItem id="ETHER_CFG_CH1_PHY_ACCESS" selectedIndex="1"/>
<gridItem id="ETHER_CFG_CH0_PHY_ADDRESS" selectedIndex="30"/>
<gridItem id="ETHER_CFG_CH1_PHY_ADDRESS" selectedIndex="1"/>
<gridItem id="ETHER_CFG_EMAC_RX_DESCRIPTORS" selectedIndex="6"/>
<gridItem id="ETHER_CFG_EMAC_TX_DESCRIPTORS" selectedIndex="3"/>
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
<gridItem id="ET0_LINKSTA" selectedIndex="1"/>
<gridItem id="ETHERC0_MII" selectedIndex="1"/>
<gridItem id="ET0_MDC" selectedIndex="1"/>
<gridItem id="ET0_MDIO" selectedIndex="1"/>
<gridItem id="ET0_LINKSTA" selectedIndex="1"/>
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
<Item current="true" id="uckdivider.selectBox"/>
<Item current="240.0 MHz" id="uckdivider.InputValue"/>
<Item current="48.0 MHz" id="uckdivider.OutputValue"/>
<Item current="sckselector.radiobutton.swtpll" id="uckdivider.Condition"/>
<Item current="comboBox1-5" error="false" id="uckdivider.comboBox"/>
</tool>
    <tool id="Pins" version="1.0.1.0">
<pinItem allocation="22" comments="" direction="None" id="XTAL" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="59" comments="" direction="None" id="RXD8" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="81" comments="" direction="None" id="ET0_TX_EN" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="83" comments="" direction="None" id="ET0_TX_ER" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="101" comments="" direction="None" id="ET0_MDC" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="76" comments="" direction="None" id="ET0_COL" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="74" comments="" direction="None" id="ET0_CRS" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="58" comments="" direction="None" id="TXD8" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="24" comments="" direction="None" id="EXTAL" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="86" comments="" direction="None" id="ET0_RX_DV" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="27" comments="" direction="None" id="ET0_LINKSTA" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="84" comments="" direction="None" id="ET0_RX_ER" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="77" comments="" direction="None" id="ET0_ETXD3" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="78" comments="" direction="None" id="ET0_ETXD2" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="79" comments="" direction="None" id="ET0_ETXD1" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="80" comments="" direction="None" id="ET0_ETXD0" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="85" comments="" direction="None" id="ET0_RX_CLK" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="91" comments="" direction="None" id="ET0_ERXD3" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="89" comments="" direction="None" id="ET0_ERXD2" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="88" comments="" direction="None" id="ET0_ERXD1" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="87" comments="" direction="None" id="ET0_ERXD0" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="82" comments="" direction="None" id="ET0_TX_CLK" isUsedBySoftware="true" locked="false" status="0"/>
<pinItem allocation="102" comments="" direction="None" id="ET0_MDIO" isUsedBySoftware="true" locked="false" status="0"/>
</tool>
</smc>
