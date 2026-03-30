if(CONFIG_SETTINGS)
  zephyr_library_sources(save_layer_state.c)
endif()

if(CONFIG_RGBLED_WIDGET)
    zephyr_library_sources(led_timeout.c)
endif()
