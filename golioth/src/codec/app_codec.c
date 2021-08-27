#include <app_codec.h>
#include <drivers/sensor.h>

#include <qcbor/qcbor_encode.h>

int app_codec_aqw_data_encode(struct app_codec_payload *payload, uint8_t *p_buf, size_t buf_len, size_t *p_size)
{

    // Setup of the goods
    UsefulBuf buf = {
        .ptr = p_buf,
        .len = buf_len};
    QCBOREncodeContext ec;
    QCBOREncode_Init(&ec, buf);

    /* Create over-arching map */
    QCBOREncode_OpenMap(&ec);

    for (int i = 0; i < payload->sensor_count; i++)
    {
        char *label = "";

        switch (payload->data[i].type)
        {
        case AQW_TEMPERATURE_SENSOR:
            label = "temp";
            break;
        case AQW_HUMIDITY_SENSOR:
            label = "hum";
            break;
        case AQW_PM25_SENSOR:
            label = "pm25";
            break;
        case AQW_VOC_SENSOR:
            label = "voc";
            break;
        default:
            continue;
        }

        /* Add coordinates */
        QCBOREncode_AddDoubleToMap(&ec, label, sensor_value_to_double(&payload->data[i].val));
    }

    /* Close map */
    QCBOREncode_CloseMap(&ec);

    /* Finish things up */
    return QCBOREncode_FinishGetSize(&ec, p_size);
}