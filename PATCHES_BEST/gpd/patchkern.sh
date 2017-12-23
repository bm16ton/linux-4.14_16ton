patch -p1 < PATCHES_BEST/gpd/sdhci-acpi-let-devices-define-their-own-private-data.patch 
patch -p1 < PATCHES_BEST/gpd/sdhci-acpi_Use-helper-function-acpi_device_uid.patch 
patch -p1 < PATCHES_BEST/gpd/sdhci.c-Convert-timers-to-use-timer_setup
patch -p1 < PATCHES_BEST/gpd/sdhci-acpi_fix_intel_power_switch.patch 
patch -p1 < PATCHES_BEST/gpd/sdhci-acpi_Tidy-Intel-slot-probe-functions-into-one.patch  
patch -p1 < PATCHES_BEST/gpd/sdhci.c-catch-errors-regulators.patch         
patch -p1 < PATCHES_BEST/gpd/sdhci.c-delete-bounce-buffer-handling.patch

