#include "packet.h"

int calculate_checksum(Packet &packet) {
    int checksum = 0;
    checksum += packet.seq_num;
    for (char c : packet.data) {
        checksum += c;
    }
    return checksum;
}
