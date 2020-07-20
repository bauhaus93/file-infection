#include "function.h"

#include "checksum.h"
#include "pe.h"

void *get_function_pointer(void *base, uint32_t function_checksum) {
    if (base == NULL) {
        return NULL;
    }
    IMAGE_EXPORT_DIRECTORY *ed = get_export_directory_by_base(base);
    if (ed == NULL) {
        return NULL;
    }
    uint32_t *name_ptr = (uint32_t *)((uint8_t *)base + ed->AddressOfNames);
    uint32_t *addr_ptr = (uint32_t *)((uint8_t *)base + ed->AddressOfFunctions);
    uint16_t *name_ordinals_ptr =
        (uint16_t *)((uint8_t *)base + ed->AddressOfNameOrdinals);

    // name_ordinals_ptr += ed->Base; // TODO: check if really needed +ed->Base

    for (uint32_t i = 0; i < ed->NumberOfNames; i++) {
        void *function_addr = (void *)((
            void **)((uint8_t *)base + *(addr_ptr + *name_ordinals_ptr)));
        if (checksum((const char *)base + *name_ptr) == function_checksum) {
            return function_addr;
        }
        name_ptr++;
        name_ordinals_ptr++;
    }
    return NULL;
}
