/**
 * @copyright   Copyright (c) 2024 Pchom & licensed under Mulan PSL v2
 * @file        drv_flash.c
 * @brief       stm32f1xx flash driver for mds device
 * @date        2024-05-30
 */
/* Include ----------------------------------------------------------------- */
#include "drv_flash.h"
#include "drv_chip.h"

/* Function ---------------------------------------------------------------- */
static uint32_t FLASH_GetAlignPadData(const uint8_t *data, size_t len)
{
    if (len >= sizeof(uint32_t)) {
        return (*(uint32_t *)data);
    }

    union {
        uint8_t dat8[sizeof(uint32_t)];
        uint32_t dat32;
    } val = {.dat32 = 0xFFFFFFFFUL};

    for (size_t i = 0; i < len; i++) {
        val.dat8[i] = data[i];
    }

    return (val.dat32);
}

size_t DRV_FLASH_Sector(uintptr_t addr, size_t *align)
{
    if (align != NULL) {
        *align = addr - (addr % FLASH_PAGE_SIZE);
    }

    return (FLASH_PAGE_SIZE);
}

MDS_Err_t DRV_FLASH_Read(uintptr_t addr, uint8_t *data, size_t len, size_t *read)
{
    size_t size = MDS_MemBuffCopy(data, len, (uint8_t *)addr, len);
    if (read != NULL) {
        *read = size;
    }

    return ((size == len) ? (MDS_EOK) : (MDS_EIO));
}

MDS_Err_t DRV_FLASH_Program(uintptr_t addr, const uint8_t *data, size_t len, size_t *write)
{
    size_t cnt = 0;
    HAL_StatusTypeDef status = HAL_OK;

    HAL_FLASH_Unlock();
    while (cnt < len) {
        uint32_t val = FLASH_GetAlignPadData(data + cnt, len - cnt);
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr + cnt, val);
        if (status != HAL_OK) {
            break;
        }
        cnt += sizeof(uint32_t);
    }
    HAL_FLASH_Lock();

    if (write != NULL) {
        *write = cnt;
    }

    return (DRV_HalStatusToMdsErr(status));
}

MDS_Err_t DRV_FLASH_Erase(uintptr_t addr, size_t size, size_t *erase)
{
    uintptr_t align = 0;
    uintptr_t sector = DRV_FLASH_Sector(addr, &align);
    if ((align != addr) || (sector > size)) {
        return (MDS_EINVAL);
    }

    FLASH_EraseInitTypeDef eraseInit = {
        .TypeErase = FLASH_TYPEERASE_PAGES,
        .Banks = FLASH_BANK_1,
        .PageAddress = addr,
        .NbPages = size / FLASH_PAGE_SIZE,
    };
    uint32_t pageError = 0;

    HAL_FLASH_Unlock();
    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&eraseInit, &pageError);
    HAL_FLASH_Lock();

    if (erase != NULL) {
        *erase = FLASH_PAGE_SIZE * ((pageError == 0xFFFFFFFFU) ? (eraseInit.NbPages) : (pageError));
    }

    return (DRV_HalStatusToMdsErr(status));
}

/* Driver ------------------------------------------------------------------ */
static MDS_Err_t DDRV_FLASH_Control(const DEV_STORAGE_Adaptr_t *storage, MDS_Item_t cmd, MDS_Arg_t *arg)
{
    MDS_ASSERT(storage != NULL);

    UNUSED(arg);

    switch (cmd) {
        case MDS_DEVICE_CMD_INIT:
        case MDS_DEVICE_CMD_DEINIT:
        case MDS_DEVICE_CMD_HANDLESZ:
            return (MDS_EOK);
        case MDS_DEVICE_CMD_OPEN:
        case MDS_DEVICE_CMD_CLOSE:
            return (MDS_EOK);
    }

    return (MDS_EACCES);
}

static MDS_Err_t DDRV_FLASH_Read(const DEV_STORAGE_Periph_t *periph, uintptr_t ofs, uint8_t *buff, size_t len,
                                 size_t *read)
{
    return (DRV_FLASH_Read(periph->object.baseAddr + ofs, buff, len, read));
}

static MDS_Err_t DDRV_FLASH_Write(const DEV_STORAGE_Periph_t *periph, uintptr_t ofs, const uint8_t *buff, size_t len,
                                  size_t *write)
{
    return (DRV_FLASH_Program(periph->object.baseAddr + ofs, buff, len, write));
}

static MDS_Err_t DDRV_FLASH_Erase(const DEV_STORAGE_Periph_t *periph, uintptr_t ofs, size_t size, size_t *erase)
{
    return (DRV_FLASH_Erase(periph->object.baseAddr + ofs, size, erase));
}

static size_t DDRV_FLASH_Sector(const DEV_STORAGE_Periph_t *periph, uintptr_t ofs, size_t *align)
{
    return (DRV_FLASH_Sector(periph->object.baseAddr + ofs, align));
}

const DEV_STORAGE_Driver_t G_DRV_STM32F1XX_FLASH = {
    .control = DDRV_FLASH_Control,
    .read = DDRV_FLASH_Read,
    .write = DDRV_FLASH_Write,
    .erase = DDRV_FLASH_Erase,
    .sector = DDRV_FLASH_Sector,
};
