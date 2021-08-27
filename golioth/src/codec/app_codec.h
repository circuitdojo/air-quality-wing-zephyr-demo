/**
 * @file app_codec.h
 * @author Jared Wolff (hello@jaredwolff.com)
 * @date 2021-07-25
 * 
 * @copyright Copyright Circuit Dojo (c) 2021
 * 
 */

#ifndef _APP_CODEC_H
#define _APP_CODEC_H

#include <zephyr.h>
#include <aqw.h>

/**
 * @brief Simplified payload fro app_codec
 * 
 */
struct app_codec_payload
{
    struct aqw_sensor_data *data;
    const size_t sensor_count;
};

/**
 * @brief Encode Air Quality Wing data as one CBOR map.
 * 
 * @param payload raw input from AQW library
 * @param p_buf pointer to the output buffer
 * @param buf_len size of said buffer
 * @param p_size pointer to size. Actual written size to p_buf
 * @return int 0 on success
 */
int app_codec_aqw_data_encode(struct app_codec_payload *payload, uint8_t *p_buf, size_t buf_len, size_t *p_size);

#endif /*_APP_CODEC_H*/