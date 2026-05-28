#include <string.h>
#include "packet_parser.h"
#include "shared_state.h"

/* Compile-time size validation (GCC extension, C99-compatible) */
typedef char accel_sz [(sizeof(AccelGnssPacket_t)   == PACKET_SIZE) ? 1 : -1];
typedef char drsint_sz[(sizeof(DRSINTPacket_t)      == PACKET_SIZE) ? 1 : -1];
typedef char stest_sz [(sizeof(SelfTestPacket_t)    == PACKET_SIZE) ? 1 : -1];
typedef char temp_sz  [(sizeof(TemperaturePacket_t) == PACKET_SIZE) ? 1 : -1];

static uint8_t  s_buf[PACKET_SIZE];
static uint32_t s_len   = 0;
static int      s_synced = 0;

void packet_parser_reset(void)
{
    s_len    = 0;
    s_synced = 0;
}

static int is_valid_header(int32_t h)
{
    return (h == HEADER_ACCEL_GNSS || h == HEADER_DRSINT ||
            h == HEADER_SELFTEST   || h == HEADER_TEMP);
}

void packet_parser_feed(uint8_t byte)
{
    if(s_len < PACKET_SIZE) {
        s_buf[s_len++] = byte;
    }

    /* Need at least 4 bytes to validate header */
    if(s_len < 4) return;

    if(!s_synced) {
        int32_t h;
        memcpy(&h, s_buf, sizeof(h));
        if(is_valid_header(h)) {
            s_synced = 1;
        } else {
            /* Slide window: discard first byte and try again */
            memmove(s_buf, s_buf + 1, s_len - 1);
            s_len--;
            return;
        }
    }

    if(s_len < PACKET_SIZE) return;

    /* Full packet assembled — dispatch by header */
    int32_t h;
    memcpy(&h, s_buf, sizeof(h));

    switch(h) {
        case HEADER_ACCEL_GNSS: {
            AccelGnssPacket_t pkt;
            memcpy(&pkt, s_buf, PACKET_SIZE);
            shared_state_update_accel(&pkt);
            break;
        }
        case HEADER_DRSINT: {
            DRSINTPacket_t pkt;
            memcpy(&pkt, s_buf, PACKET_SIZE);
            shared_state_update_drsint(&pkt);
            break;
        }
        case HEADER_SELFTEST: {
            SelfTestPacket_t pkt;
            memcpy(&pkt, s_buf, PACKET_SIZE);
            shared_state_update_selftest(&pkt);
            break;
        }
        case HEADER_TEMP: {
            TemperaturePacket_t pkt;
            memcpy(&pkt, s_buf, PACKET_SIZE);
            shared_state_update_temp(&pkt);
            break;
        }
        default:
            /* Corrupt packet — re-sync */
            s_synced = 0;
            break;
    }

    s_len    = 0;
    s_synced = 0;
}
