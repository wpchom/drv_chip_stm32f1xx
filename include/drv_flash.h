/**
 * @copyright   Copyright (c) 2024 Pchom & licensed under Mulan PSL v2
 * @file        drv_gpio.h
 * @brief       stm32f1xx flash driver for mds device
 * @date        2024-05-30
 */
#ifndef __DRV_FLASH_H__
#define __DRV_FLASH_H__

/* Include ----------------------------------------------------------------- */
#include "dev_storage.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_ll_exti.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Function ---------------------------------------------------------------- */
MDS_Err_t DRV_FLASH_Program(uintptr_t addr, const uint8_t *data, size_t len, size_t *write);
MDS_Err_t DRV_FLASH_Erase(uintptr_t addr, size_t blks, size_t *erase);

/* Driver ------------------------------------------------------------------ */
extern const DEV_STORAGE_Driver_t G_DRV_STM32F1XX_FLASH;

#ifdef __cplusplus
}
#endif

#endif /* __DRV_FLASH_H__ */
