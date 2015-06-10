/*
 * STM32 Cortex-M device emulation.
 *
 * Copyright (c) 2014 Liviu Ionescu
 * Copyright (c) 2010 Andre Beckus
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "hw/arm/stm32-mcu.h"

#if defined(CONFIG_VERBOSE)
#include "verbosity.h"
#endif

/*
 * Common layer for all STM32 devices.
 * Alias the flash memory to 0x08000000.
 *
 * TODO: define the special CCM region for the models that include it.
 */
static void stm32_mcu_instance_init(Object *obj)
{
    qemu_log_function_name();

    STM32MCUState *state = STM32_MCU_STATE(obj);

    object_initialize(&state->rcc, sizeof(state->rcc), TYPE_STM32_RCC);

    object_initialize(&state->flash, sizeof(state->flash), TYPE_STM32_FLASH);

    /* Cannot access capabilities here, since they are set later */

    /* Initialise all GPIOs, but will be realized only existing ones */
    for (int i = 0; i < STM32_MAX_GPIO; ++i) {
        object_initialize(&state->gpio[i], sizeof(state->gpio[i]),
        TYPE_STM32_GPIO);
    }
}

static void stm32_mcu_realize(DeviceState *dev, Error **errp)
{
    qemu_log_function_name();

    STM32MCUState *state = STM32_MCU_STATE(dev);
    STM32MCUClass *nc = STM32_MCU_GET_CLASS(state);
    Error *local_err = NULL;
    nc->parent_realize(dev, &local_err);
    if (local_err) {
        error_propagate(errp, local_err);
        return;
    }

    CortexMState *cm_state = CORTEXM_MCU_STATE(dev);
    STM32Capabilities *capabilities =
            (STM32Capabilities *) cm_state->capabilities;
    assert(capabilities != NULL);

    const char *family;
    switch (capabilities->stm32.family) {
    case STM32_FAMILY_F1:
        family = "F1";
        break;
    case STM32_FAMILY_F4:
        family = "F4";
        break;
    default:
        family = "unknown";
    }
    qemu_log_mask(LOG_TRACE, "STM32 Family: %s\n", family);


    STM32SysBusDevice *sbd;

    /* RCC */
    DeviceState *dev2 = DEVICE(&state->rcc);

    /* Copy capabilities into internal objects. */
    sbd = STM32_SYS_BUS_DEVICE_STATE(&state->rcc);
    sbd->capabilities = capabilities;

    /* Copy internal oscillator frequencies from capabilities. */
    qdev_prop_set_uint32(dev2, "hsi-freq-hz",
            sbd->capabilities->stm32.hsi_freq_hz);
    qdev_prop_set_uint32(dev2, "lsi-freq-hz",
            sbd->capabilities->stm32.lsi_freq_hz);

    object_property_set_bool(OBJECT(&state->rcc), true, "realized", &local_err);
    if (local_err != NULL) {
        error_propagate(errp, local_err);
        return;
    }
    qdev_set_parent_bus(DEVICE(&state->rcc), sysbus_get_default());

    /* FLASH */
    sbd = STM32_SYS_BUS_DEVICE_STATE(&state->flash);
    sbd->capabilities = capabilities;

    object_property_set_bool(OBJECT(&state->flash), true, "realized",
            &local_err);
    if (local_err != NULL) {
        error_propagate(errp, local_err);
        return;
    }
    qdev_set_parent_bus(DEVICE(&state->flash), sysbus_get_default());

    /* GPIOA */
    if (capabilities->stm32.has_gpioa) {
        sbd = STM32_SYS_BUS_DEVICE_STATE(&state->gpio[STM32_GPIO_PORT_A]);
        sbd->capabilities = capabilities;

        state->gpio[STM32_GPIO_PORT_A].port_index = STM32_GPIO_PORT_A;
        object_property_set_bool(OBJECT(&state->gpio[STM32_GPIO_PORT_A]), true,
                "realized", &local_err);
        if (local_err != NULL) {
            error_propagate(errp, local_err);
            return;
        }
        qdev_set_parent_bus(DEVICE(&state->gpio[STM32_GPIO_PORT_A]),
                sysbus_get_default());
    }

    /* GPIOB */
    if (capabilities->stm32.has_gpiob) {
        sbd = STM32_SYS_BUS_DEVICE_STATE(&state->gpio[STM32_GPIO_PORT_B]);
        sbd->capabilities = capabilities;

        state->gpio[STM32_GPIO_PORT_B].port_index = STM32_GPIO_PORT_B;
        object_property_set_bool(OBJECT(&state->gpio[STM32_GPIO_PORT_B]), true,
                "realized", &local_err);
        if (local_err != NULL) {
            error_propagate(errp, local_err);
            return;
        }
        qdev_set_parent_bus(DEVICE(&state->gpio[STM32_GPIO_PORT_B]),
                sysbus_get_default());
    }

    /* GPIOC */
    if (capabilities->stm32.has_gpioc) {
        sbd = STM32_SYS_BUS_DEVICE_STATE(&state->gpio[STM32_GPIO_PORT_C]);
        sbd->capabilities = capabilities;

        state->gpio[STM32_GPIO_PORT_C].port_index = STM32_GPIO_PORT_C;
        object_property_set_bool(OBJECT(&state->gpio[STM32_GPIO_PORT_C]), true,
                "realized", &local_err);
        if (local_err != NULL) {
            error_propagate(errp, local_err);
            return;
        }
        qdev_set_parent_bus(DEVICE(&state->gpio[STM32_GPIO_PORT_C]),
                sysbus_get_default());
    }

    /* GPIOD */
    if (capabilities->stm32.has_gpiod) {
        sbd = STM32_SYS_BUS_DEVICE_STATE(&state->gpio[STM32_GPIO_PORT_D]);
        sbd->capabilities = capabilities;

        state->gpio[STM32_GPIO_PORT_D].port_index = STM32_GPIO_PORT_D;
        object_property_set_bool(OBJECT(&state->gpio[STM32_GPIO_PORT_D]), true,
                "realized", &local_err);
        if (local_err != NULL) {
            error_propagate(errp, local_err);
            return;
        }
        qdev_set_parent_bus(DEVICE(&state->gpio[STM32_GPIO_PORT_D]),
                sysbus_get_default());
    }

    /* GPIOE */
    if (capabilities->stm32.has_gpioe) {
        sbd = STM32_SYS_BUS_DEVICE_STATE(&state->gpio[STM32_GPIO_PORT_E]);
        sbd->capabilities = capabilities;

        state->gpio[STM32_GPIO_PORT_E].port_index = STM32_GPIO_PORT_E;
        object_property_set_bool(OBJECT(&state->gpio[STM32_GPIO_PORT_E]), true,
                "realized", &local_err);
        if (local_err != NULL) {
            error_propagate(errp, local_err);
            return;
        }
        qdev_set_parent_bus(DEVICE(&state->gpio[STM32_GPIO_PORT_E]),
                sysbus_get_default());
    }

    /* GPIOF */
    if (capabilities->stm32.has_gpiof) {
        sbd = STM32_SYS_BUS_DEVICE_STATE(&state->gpio[STM32_GPIO_PORT_F]);
        sbd->capabilities = capabilities;

        state->gpio[STM32_GPIO_PORT_F].port_index = STM32_GPIO_PORT_F;
        object_property_set_bool(OBJECT(&state->gpio[STM32_GPIO_PORT_F]), true,
                "realized", &local_err);
        if (local_err != NULL) {
            error_propagate(errp, local_err);
            return;
        }
        qdev_set_parent_bus(DEVICE(&state->gpio[STM32_GPIO_PORT_F]),
                sysbus_get_default());
    }

    /* GPIOG */
    if (capabilities->stm32.has_gpiog) {
        sbd = STM32_SYS_BUS_DEVICE_STATE(&state->gpio[STM32_GPIO_PORT_G]);
        sbd->capabilities = capabilities;

        state->gpio[STM32_GPIO_PORT_G].port_index = STM32_GPIO_PORT_G;
        object_property_set_bool(OBJECT(&state->gpio[STM32_GPIO_PORT_G]), true,
                "realized", &local_err);
        if (local_err != NULL) {
            error_propagate(errp, local_err);
            return;
        }
        qdev_set_parent_bus(DEVICE(&state->gpio[STM32_GPIO_PORT_G]),
                sysbus_get_default());
    }

}

static void stm32_mcu_memory_regions_create(DeviceState *dev)
{
    qemu_log_function_name();

    STM32MCUState *state = STM32_MCU_STATE(dev);
    STM32MCUClass *nc = STM32_MCU_GET_CLASS(state);

    /* Create the parent (Cortex-M) memory regions */
    nc->parent_memory_regions_create(dev);

    /**
     * The STM32 family stores its Flash memory at some base address in memory
     * (0x08000000 for medium density devices), and then aliases it to the
     * boot memory space, which starts at 0x00000000 (the "System Memory" can
     * also be aliased to 0x00000000, but this is not implemented here).
     * The processor executes the code in the aliased memory at 0x00000000.
     * We need to make a QEMU alias so that reads in the 0x08000000 area
     * are passed through to the 0x00000000 area. Note that this is the
     * opposite of real hardware, where the memory at 0x00000000 passes
     * reads through the "real" flash memory at 0x08000000, but it works
     * the same either way.
     */
    CortexMState *cm_state = CORTEXM_MCU_STATE(dev);
    int flash_size = cm_state->flash_size_kb * 1024;

    /* Allocate a new region for the alias */
    MemoryRegion *flash_alias_mem = g_malloc(sizeof(MemoryRegion));

    /* Initialise the new region */
    memory_region_init_alias(flash_alias_mem, NULL, "stm32-mem-flash-alias",
            &cm_state->flash_mem, 0, flash_size);
    memory_region_set_readonly(flash_alias_mem, true);

    /* Alias it as the STM specific 0x08000000 */
    memory_region_add_subregion(get_system_memory(), 0x08000000,
            flash_alias_mem);
}

static Property stm32_mcu_properties[] = {
    /* TODO: add STM32 specific properties */
    DEFINE_PROP_END_OF_LIST(), //
        };

static void stm32_mcu_class_init(ObjectClass *klass, void *data)
{
    STM32MCUClass *nc = STM32_MCU_CLASS(klass);
    CortexMClass *cm_class = CORTEXM_MCU_CLASS(klass);
    DeviceClass *dc = DEVICE_CLASS(klass);

    nc->parent_realize = dc->realize;
    dc->realize = stm32_mcu_realize;

    nc->parent_memory_regions_create = cm_class->memory_regions_create;
    cm_class->memory_regions_create = stm32_mcu_memory_regions_create;

    dc->props = stm32_mcu_properties;
}

static const TypeInfo stm32_mcu_type_info = {
    .abstract = true,
    .name = TYPE_STM32_MCU,
    .parent = TYPE_CORTEXM_MCU,
    .instance_size = sizeof(STM32MCUState),
    .instance_init = stm32_mcu_instance_init,
    .class_init = stm32_mcu_class_init,
    .class_size = sizeof(STM32MCUClass) };

/* ----- Type inits. ----- */

static void stm32_type_init()
{
    type_register_static(&stm32_mcu_type_info);

}

#if defined(CONFIG_GNU_ARM_ECLIPSE)
type_init(stm32_type_init);
#endif