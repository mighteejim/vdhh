/*
 * QEMU<->ACPI BIOS PCI hotplug interface
 *
 * QEMU supports PCI hotplug via ACPI. This module
 * implements the interface between QEMU and the ACPI BIOS.
 * Interface specification - see docs/specs/acpi_pci_hotplug.txt
 *
 * Copyright (c) 2013, Red Hat Inc, Michael S. Tsirkin (mst@redhat.com)
 * Copyright (c) 2006 Fabrice Bellard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 *
 */

#ifndef HW_ACPI_PCIHP_H
#define HW_ACPI_PCIHP_H

#include <inttypes.h>
#include <qemu/typedefs.h>
#include "nacpi.h"
#include "vmstate.h"

typedef struct AcpiPciHpPciStatus {
    uint32_t up;
    uint32_t down;
    uint32_t hotplug_enable;
} AcpiPciHpPciStatus;

#define ACPI_PCIHP_PROP_BSEL "acpi-pcihp-bsel"
#define ACPI_PCIHP_MAX_HOTPLUG_BUS 256
#define ACPI_PCIHP_BSEL_DEFAULT 0x0

typedef struct AcpiPciHpState {
    AcpiPciHpPciStatus acpi_pcihp_pci_status[ACPI_PCIHP_MAX_HOTPLUG_BUS];
    uint32_t hotplug_select;
    PCIBus *root;
    VeertuMemArea io;
    bool legacy_piix;
} AcpiPciHpState;

void acpi_pcihp_init(AcpiPciHpState *, PCIBus *root,
                     VeertuMemArea *address_space_io, bool bridges_enabled);

void acpi_pcihp_device_plug_cb(ACPIREGS *ar, vmx_irq irq, AcpiPciHpState *s,
                               DeviceState *dev, Error **errp);
void acpi_pcihp_device_unplug_cb(ACPIREGS *ar, vmx_irq irq, AcpiPciHpState *s,
                                 DeviceState *dev, Error **errp);

/* Called on reset */
void acpi_pcihp_reset(AcpiPciHpState *s);

extern const VMStateDescription vmstate_acpi_pcihp_pci_status;

#define VMSTATE_PCI_HOTPLUG(pcihp, state, test_pcihp) \
        VMSTATE_UINT32_TEST(pcihp.hotplug_select, state, \
                            test_pcihp), \
        VMSTATE_STRUCT_ARRAY_TEST(pcihp.acpi_pcihp_pci_status, state, \
                                  ACPI_PCIHP_MAX_HOTPLUG_BUS, \
                                  test_pcihp, 1, \
                                  vmstate_acpi_pcihp_pci_status, \
                                  AcpiPciHpPciStatus)

#endif
