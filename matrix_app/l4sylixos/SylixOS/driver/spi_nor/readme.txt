
/*********************************************************************************************************
** 函数名称:
** 功能描述: 功能测试
** 输  入  :
** 输  出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  Flash_QueryJedec ()
{
    u8 au8Tmp[3] = { 0 };
    u8 cmd = 0x9F;
    LW_SPI_MESSAGE   spiCmdMessage  = {
            .SPIMSG_uiLen = 1,
            .SPIMSG_pucWrBuffer = &cmd,
            .SPIMSG_pucRdBuffer = NULL,
    };
    LW_SPI_MESSAGE   spiRdMessage  = {
            .SPIMSG_uiLen = 3,
            .SPIMSG_pucWrBuffer = NULL,
            .SPIMSG_pucRdBuffer = au8Tmp,
    };
    SPI_ENABLE_SS();
    API_SpiDeviceTransfer(_G_spiNorObj.Spi_NorDev, &spiCmdMessage, 1);
    API_SpiDeviceTransfer(_G_spiNorObj.Spi_NorDev, &spiRdMessage, 1);
    SPI_DISABLE_SS();

    printk( "adapter spi flash IP hx hahaha : %x, %x, %x\n", *au8Tmp, au8Tmp[1], au8Tmp[2]);

    return (ERROR_NONE);
}

INT  Flash_Query ()
{
    u8 au8Tmp[4] = { 0 };
    u8 cmd[4] = {0x90};
    LW_SPI_MESSAGE   spiCmdMessage  = {
            .SPIMSG_uiLen = 4,
            .SPIMSG_pucWrBuffer = cmd,
            .SPIMSG_pucRdBuffer = NULL,
    };
    LW_SPI_MESSAGE   spiRdMessage  = {
            .SPIMSG_uiLen = 4,
            .SPIMSG_pucWrBuffer = NULL,
            .SPIMSG_pucRdBuffer = au8Tmp,
    };
    SPI_ENABLE_SS();
    API_SpiDeviceTransfer(_G_spiNorObj.Spi_NorDev, &spiCmdMessage, 1);
    API_SpiDeviceTransfer(_G_spiNorObj.Spi_NorDev, &spiRdMessage, 1);
    SPI_DISABLE_SS();

    printk( "adapter spi flash IP hx hahaha : %x, %x, %x, %x\n", *au8Tmp, au8Tmp[1], au8Tmp[2], au8Tmp[3]);

    return (ERROR_NONE);
}

