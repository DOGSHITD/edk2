/** @file
  Produces the SMM CPU Features Protocol.

Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiSmm.h>
#include <Protocol/SmmCpuFeatures.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/SmmCpuFeaturesLib.h>

//
// SMM CPU I/O Protocol instance
//
EDKII_SMM_CPU_FEATURES_PROTOCOL mSmmCpuFeatures = {
  SmmCpuFeaturesInitializeProcessor,
  SmmCpuFeaturesHookReturnFromSmm,
  SmmCpuFeaturesSmmRelocationComplete,
  SmmCpuFeaturesGetSmiHandlerSize,
  SmmCpuFeaturesInstallSmiHandler,
  SmmCpuFeaturesNeedConfigureMtrrs,
  SmmCpuFeaturesDisableSmrr,
  SmmCpuFeaturesReenableSmrr,
  SmmCpuFeaturesRendezvousEntry,
  SmmCpuFeaturesRendezvousExit,
  SmmCpuFeaturesIsSmmRegisterSupported,
  SmmCpuFeaturesGetSmmRegister,
  SmmCpuFeaturesSetSmmRegister,
  SmmCpuFeaturesReadSaveStateRegister,
  SmmCpuFeaturesWriteSaveStateRegister,
  SmmCpuFeaturesCompleteSmmReadyToLock,
  SmmCpuFeaturesAllocatePageTableMemory
};

/**
  The module Entry Point CpuFeaturesSmm driver

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS  The entry point is executed successfully.
  @retval Other        Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
CpuFeaturesSmmInitialize (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;

  Handle = NULL;
  //
  // Install the SMM CPU I/O Protocol into the SMM protocol database
  //
  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gEdkiiSmmCpuFeaturesProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mSmmCpuFeatures
                    );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
