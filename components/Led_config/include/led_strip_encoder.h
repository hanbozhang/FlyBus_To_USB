/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include "driver/rmt_encoder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief LED 灯条编码器配置类型
 */
typedef struct {
    uint32_t resolution; /*!< 编码器分辨率，以赫兹为单位 */
} led_strip_encoder_config_t;

/**
 * @brief 创建 RMT 编码器，用于将 LED 灯条像素编码为 RMT 符号
 *
 * @param[in] config 编码器配置
 * @param[out] ret_encoder 返回的编码器手柄
 * @return
 *      - ESP_ERR_INVALID_ARG任何无效参数
 *      - 创建 LED 灯条编码器时内存不足ESP_ERR_NO_MEM
 *      - ESP_OK是否成功创建编码器
 */
esp_err_t rmt_new_led_strip_encoder(const led_strip_encoder_config_t *config, rmt_encoder_handle_t *ret_encoder);

void led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b);

#ifdef __cplusplus
}
#endif
