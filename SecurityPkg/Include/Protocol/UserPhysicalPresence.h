/** @file
  This library is intended to be used by BDS modules.
  This library will execute TPM2 request.

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _USER_PHYSICAL_PRESENCE_PROTOCOL_H_
#define _USER_PHYSICAL_PRESENCE_PROTOCOL_H_

#define EFI_USER_PHYSICAL_PRESENCE_PROTOCOL_GUID \
  { 0x712954f3, 0x26c, 0x4057, { 0x90, 0xdb, 0x70, 0x98, 0xed, 0xff, 0x3c, 0x47 } }

typedef struct _EFI_USER_PHYSICALPRESENCE_PROTOCOL USER_PHYSICAL_PRESENCE_PROTOCOL;

/**

  This function provides a platform-specific method to detect whether the platform
  is operating by a physically present user.

  Programmatic changing of platform security policy (such as disable Secure Boot,
  or switch between Standard/Custom Secure Boot mode) MUST NOT be possible during
  Boot Services or after exiting EFI Boot Services. Only a physically present user
  is allowed to perform these operations.

  NOTE THAT: This function cannot depend on any EFI Variable Service since they are
  not available when this function is called in AuthenticateVariable driver.

  @retval  TRUE       The platform is operated by a physically present user.
  @retval  FALSE      The platform is NOT operated by a physically present user.

**/
typedef
BOOLEAN
(EFIAPI *USER_PHYSICAL_PRESENT) (
  VOID
  );


struct _EFI_USER_PHYSICALPRESENCE_PROTOCOL {
  USER_PHYSICAL_PRESENT               UserPhysicalPresent;
};

extern EFI_GUID gEdkiiUserPhysicalPresenceProtocolGuid;

#endif
