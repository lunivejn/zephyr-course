if(CONFIG_OUR_DRIVER)
  target_sources(app PRIVATE our_driver.c)
endif()

if(CONFIG_OUR_DRIVER AND CONFIG_SHELL)
  target_sources(app PRIVATE our_driver_shell.c)
endif()
