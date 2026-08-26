# Registers your C files so the compiler knows what to build
idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES nvs_flash
    REQUIRES esp_wifi
)
