idf_component_register(
    SRCS "FlyBus_To_USB_main.c"
    INCLUDE_DIRS "."
    REQUIRES driver
            FlyUSB_config
            FlyBus
            ConfigStore
            Led_config
            WiFi_Config
            Web_config
    )
