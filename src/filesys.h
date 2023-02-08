#include <nvs.h>
#include <nvs_flash.h>
#include <stdint.h>

#define FS_Init() nvs_flash_init()

void StoreBright(uint8_t bright){
    nvs_handle_t my_handle;
    nvs_open("storage", NVS_READWRITE, &my_handle);
    nvs_set_u8(my_handle, "bright", bright);
    nvs_commit(my_handle);
    nvs_close(my_handle);
}

uint8_t ReadBright(){
    uint8_t bright = 10;
    nvs_handle_t my_handle;
    nvs_open("storage", NVS_READWRITE, &my_handle);
    nvs_get_u8(my_handle, "bright", &bright);
    nvs_close(my_handle);
    return bright;
}

void StoreWiFi(){
    
}
