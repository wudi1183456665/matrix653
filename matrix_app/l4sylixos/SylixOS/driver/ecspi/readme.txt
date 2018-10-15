/*********************************************************************************************************
** 函数名称: spi_test
** 功能描述: 测试专用
*********************************************************************************************************/
INT spi_nor_flash_query ()
{
    u8 au8Tmp[4] = { 0 };
    UINT32 uiValue;
    UINT32 tmp = __SPI_RETRY_TIMES;
    __PIMX6_SPI_CHANNEL pSpiChannel = &_G_ecSpiChannels[0];

    writel(BM_ECSPI_STATREG_TC, ECSPI_STATREG);

    au8Tmp[3] = 0x9F;

    writel(*(unsigned int *)au8Tmp, ECSPI_TXDATA);
    uiValue = readl(ECSPI_CONREG) ;
    uiValue &= ~ECSPI_CONREG_BURSTLENGTH;
    uiValue |= 31 << 20;
    writel(uiValue, ECSPI_CONREG);

    uiValue = readl(ECSPI_CONREG) ;
    uiValue |= ECSPI_CONREG_XCH;
    writel(uiValue, ECSPI_CONREG);


    while (!(readl(ECSPI_STATREG) & BM_ECSPI_STATREG_TC) && tmp--) {
        udelay(__SPI_RETRY_TIMES);
    }

    uiValue = readl(ECSPI_RXDATA);
    printk( "spi flash IP : %x\n", uiValue);

    return 0;
}

INT adapter_transfer ()
{
    __PIMX6_SPI_CHANNEL  pSpiChannel = &_G_ecSpiChannels[0];
    u8 cmd[4] = {0, 0x0, 0x00, 0x90};
    UINT uivalue;

    _ecSpiCs0Enable();
    IN_TXFIFO(*(unsigned int*)cmd);

    writel(BM_ECSPI_STATREG_TC, ECSPI_STATREG);
    uivalue = readl(ECSPI_CONREG) ;
    uivalue &= ~ECSPI_CONREG_BURSTLENGTH;
    uivalue |= ((4 << 3) - 1) << 20;
    writel(uivalue, ECSPI_CONREG);

    uivalue = readl(ECSPI_CONREG) ;
    uivalue |= ECSPI_CONREG_XCH;
    writel(uivalue, ECSPI_CONREG);

    uivalue = __SPI_RETRY_TIMES;
    while (!(readl(ECSPI_STATREG) & BM_ECSPI_STATREG_TC)) {
        udelay(__SPI_RETRY_TIMES);
    }
    writel(BM_ECSPI_STATREG_TC, ECSPI_STATREG);
    OUT_RXFIFO();

    IN_TXFIFO(0);
    writel(BM_ECSPI_STATREG_TC, ECSPI_STATREG);
    uivalue = readl(ECSPI_CONREG) ;
    uivalue &= ~ECSPI_CONREG_BURSTLENGTH;
    uivalue |= ((4 << 3) - 1) << 20;
    writel(uivalue, ECSPI_CONREG);

    uivalue = readl(ECSPI_CONREG) ;
    uivalue |= ECSPI_CONREG_XCH;
    writel(uivalue, ECSPI_CONREG);

    uivalue = __SPI_RETRY_TIMES;
    while (!(readl(ECSPI_STATREG) & BM_ECSPI_STATREG_TC)) {
        udelay(__SPI_RETRY_TIMES);
    }
    writel(BM_ECSPI_STATREG_TC, ECSPI_STATREG);


    uivalue = OUT_RXFIFO();
    _ecSpiCs0Disable();

    printf("value is %x\n", uivalue);
    return 0;
}

INT adapter_test ()
{
    __PIMX6_SPI_CHANNEL  pSpiChannel = &_G_ecSpiChannels[0];
    u8 au8Tmp[4] = { 0 };
    u8 cmd[4] = {0x90, 0, 0, 0};
    UINT uivalue;

    LW_SPI_MESSAGE   cmd_message  = {
            .SPIMSG_uiLen = 4,
            .SPIMSG_pucWrBuffer = cmd,
            .SPIMSG_pucRdBuffer = NULL,
    };
    LW_SPI_MESSAGE   rd_message  = {
            .SPIMSG_uiLen = 4,
            .SPIMSG_pucWrBuffer = NULL,
            .SPIMSG_pucRdBuffer = au8Tmp,
    };
    _ecSpiCs0Enable();
    __imx6Spi0Transfer(NULL, &cmd_message, 1);
    __imx6Spi0Transfer(NULL, &rd_message, 1);
    _ecSpiCs0Disable();

    printk( "adapter spi flash IP : %x, %x, %x, %x\n", *au8Tmp, au8Tmp[1], au8Tmp[2], au8Tmp[3]);
    return 0;
}



