#include "ch.h"
#include "hal.h"
#include "can.h"
#include "can_telemetry.h"
#include "component_state.h"
#include "messaging.h"
#include "can_interface.h"

#include <string.h>

#if (HAL_USE_CAN == TRUE)

static const CANConfig cancfg = {
        .mcr =
        /* Automatic Bus Off Management enabled,
         * Automatic Wake Up Management enabled.
         */
        CAN_MCR_ABOM | CAN_MCR_AWUM,
        .btr =
        /* CAN is on APB1 at 42MHz, we want 1Mbit/s.
         * 1/Baud = (BRP+1)/(APB1) * (3+TS1+TS2)
         */
        CAN_BTR_BRP(5) | CAN_BTR_TS1(3) | CAN_BTR_TS2(1)
        /* Allow up to 2 time quanta clock synchronisation */
        | CAN_BTR_SJW(1)
};

static void can_send(uint16_t msg_id, bool can_rtr, uint8_t *data, uint8_t datalen) {
    static CANTxFrame txmsg;

    chDbgAssert(datalen <= 8, "CAN packet >8 bytes");

    if(can_rtr == false) {
        txmsg.RTR = CAN_RTR_DATA;
    } else {
        txmsg.RTR = CAN_RTR_REMOTE;
    }
    txmsg.IDE = CAN_IDE_STD;
    txmsg.DLC = datalen;
    txmsg.SID = msg_id;

    memcpy(&txmsg.data8, data, datalen);

    canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(100));
}

static can_interface_t interface = {can_send};

static bool can_send_telemetry(const telemetry_t* packet, message_metadata_t metadata) {
    return can_interface_send(&interface, packet, metadata);
}

MESSAGING_CONSUMER(can_telemetry_messaging_consumer, 0, 0, message_flags_send_over_can, message_flags_send_over_can, can_send_telemetry, 20);

void can_telemetry_start(void) {
    canStart(&CAND1, &cancfg);

    can_interface_init(&interface);
    messaging_consumer_init(&can_telemetry_messaging_consumer);
}

void can_telemetry_transmit_thread(void* arg) {
    (void)arg;
    while (true) {
        messaging_consumer_receive(&can_telemetry_messaging_consumer, true, false);
        platform_thread_yield();
    }
}

void can_telemetry_receive_thread(void* arg) {
    (void)arg;

    event_listener_t el;
    CANRxFrame rxmsg;

    chEvtRegister(&CAND1.rxfull_event, &el, 0);

    while(true) {
        if(chEvtWaitAnyTimeout(ALL_EVENTS, MS2ST(100)) == 0) {
            continue;
        }

        while(canReceive(&CAND1, CAN_ANY_MAILBOX, &rxmsg,
                         TIME_IMMEDIATE) == MSG_OK) {
            can_interface_receive(&interface, rxmsg.SID, rxmsg.RTR, rxmsg.data8, rxmsg.DLC);
        }
    }
}

bool can_telemetry_connected(void) {
    return true; // As CAN is connectionless this is always true
}

#endif