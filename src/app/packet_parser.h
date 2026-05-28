#ifndef PACKET_PARSER_H
#define PACKET_PARSER_H

#include <stdint.h>

#define PACKET_SIZE          65U
#define HEADER_ACCEL_GNSS    ((int32_t)0x00FFFFFF)
#define HEADER_DRSINT        ((int32_t)0x00AAAAAA)
#define HEADER_SELFTEST      ((int32_t)0x00CCCCCC)
#define HEADER_TEMP          ((int32_t)0x00DDDDDD)

#pragma pack(push, 1)

typedef struct {
    int32_t  header;
    float    acc[6];     /* [0..2]=raw XYZ, [3..5]=filtered XYZ (mg) */
    int32_t  date;       /* DDMMYY */
    int16_t  hh;
    int16_t  mm;
    int16_t  ss;
    int16_t  msec;
    uint8_t  status;     /* 'A'=valid fix, 'V'=void */
    float    speed;      /* knots */
    double   lat;
    double   lon;
    int16_t  tot_sats;
    int16_t  sats_used;
} AccelGnssPacket_t;

typedef struct {
    int32_t  header;
    float    gyro_x_avg;
    float    gyro_y_avg;
    float    gyro_z_avg;
    float    acc_x_avg;
    float    acc_y_avg;
    float    acc_z_avg;
    float    M_pitch;
    float    M_roll;
    float    M_yaw;
    float    gyro_x_offset;
    float    gyro_y_offset;
    float    gyro_z_offset;
    float    acc_x_offset;
    float    acc_y_offset;
    float    acc_z_offset;
    uint8_t  reserved10;
} DRSINTPacket_t;

typedef struct {
    int32_t  header;
    float    selfTestAcc[3];       /* delta XYZ (mg) */
    uint8_t  isAccSelfTestFail;    /* 0=pass, 1=fail */
    float    offsetFilterAcc[3];   /* filtered offset XYZ (mg) */
    uint8_t  gnssErrCode;          /* 0=OK,1=Gyro,2=Acc,3=Odo,4=GNSS,... */
    uint8_t  gnssImuSelfTestResult;/* 0=pass, 1=fail */
    uint8_t  reserved[34];
} SelfTestPacket_t;

typedef struct {
    int32_t  header;
    uint16_t temp_raw;
    float    temp_celsius;
    uint8_t  reserved[55];
} TemperaturePacket_t;

#pragma pack(pop)

/* Reset internal sync state (call once before starting the reader thread) */
void packet_parser_reset(void);

/* Feed one byte. Assembles complete 65-byte packets internally and calls
 * shared_state_update_*() on success. Called from reader thread only. */
void packet_parser_feed(uint8_t byte);

#endif
