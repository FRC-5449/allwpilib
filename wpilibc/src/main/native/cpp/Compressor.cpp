// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Compressor.h"

#include <hal/Compressor.h>
#include <hal/FRCUsageReporting.h>
#include <hal/Ports.h>
#include <hal/Solenoid.h>

#include "frc/WPIErrors.h"
#include "frc/smartdashboard/SendableBuilder.h"
#include "frc/smartdashboard/SendableRegistry.h"

using namespace frc;

Compressor::Compressor(int pcmID) : m_module(pcmID) {
  int32_t status = 0;
  m_compressorHandle = HAL_InitializeCompressor(m_module, &status);
  if (status != 0) {
    wpi_setHALErrorWithRange(status, 0, HAL_GetNumPCMModules(), pcmID);
    return;
  }
  SetClosedLoopControl(true);

  HAL_Report(HALUsageReporting::kResourceType_Compressor, pcmID + 1);
  SendableRegistry::GetInstance().AddLW(this, "Compressor", pcmID);
}

void Compressor::Start() {
  if (StatusIsFatal()) {
    return;
  }
  SetClosedLoopControl(true);
}

void Compressor::Stop() {
  if (StatusIsFatal()) {
    return;
  }
  SetClosedLoopControl(false);
}

bool Compressor::Enabled() const {
  if (StatusIsFatal()) {
    return false;
  }
  int32_t status = 0;
  bool value;

  value = HAL_GetCompressor(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

bool Compressor::GetPressureSwitchValue() const {
  if (StatusIsFatal()) {
    return false;
  }
  int32_t status = 0;
  bool value;

  value = HAL_GetCompressorPressureSwitch(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

double Compressor::GetCompressorCurrent() const {
  if (StatusIsFatal()) {
    return 0;
  }
  int32_t status = 0;
  double value;

  value = HAL_GetCompressorCurrent(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

void Compressor::SetClosedLoopControl(bool on) {
  if (StatusIsFatal()) {
    return;
  }
  int32_t status = 0;

  HAL_SetCompressorClosedLoopControl(m_compressorHandle, on, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }
}

bool Compressor::GetClosedLoopControl() const {
  if (StatusIsFatal()) {
    return false;
  }
  int32_t status = 0;
  bool value;

  value = HAL_GetCompressorClosedLoopControl(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

bool Compressor::GetCompressorCurrentTooHighFault() const {
  if (StatusIsFatal()) {
    return false;
  }
  int32_t status = 0;
  bool value;

  value = HAL_GetCompressorCurrentTooHighFault(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

bool Compressor::GetCompressorCurrentTooHighStickyFault() const {
  if (StatusIsFatal()) {
    return false;
  }
  int32_t status = 0;
  bool value;

  value =
      HAL_GetCompressorCurrentTooHighStickyFault(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

bool Compressor::GetCompressorShortedStickyFault() const {
  if (StatusIsFatal()) {
    return false;
  }
  int32_t status = 0;
  bool value;

  value = HAL_GetCompressorShortedStickyFault(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

bool Compressor::GetCompressorShortedFault() const {
  if (StatusIsFatal()) {
    return false;
  }
  int32_t status = 0;
  bool value;

  value = HAL_GetCompressorShortedFault(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

bool Compressor::GetCompressorNotConnectedStickyFault() const {
  if (StatusIsFatal()) {
    return false;
  }
  int32_t status = 0;
  bool value;

  value = HAL_GetCompressorNotConnectedStickyFault(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

bool Compressor::GetCompressorNotConnectedFault() const {
  if (StatusIsFatal()) {
    return false;
  }
  int32_t status = 0;
  bool value;

  value = HAL_GetCompressorNotConnectedFault(m_compressorHandle, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }

  return value;
}

void Compressor::ClearAllPCMStickyFaults() {
  if (StatusIsFatal()) {
    return;
  }
  int32_t status = 0;

  HAL_ClearAllPCMStickyFaults(m_module, &status);

  if (status) {
    wpi_setWPIError(Timeout);
  }
}

int Compressor::GetModule() const {
  return m_module;
}

void Compressor::InitSendable(SendableBuilder& builder) {
  builder.SetSmartDashboardType("Compressor");
  builder.AddBooleanProperty(
      "Enabled", [=]() { return Enabled(); },
      [=](bool value) {
        if (value) {
          Start();
        } else {
          Stop();
        }
      });
  builder.AddBooleanProperty(
      "Pressure switch", [=]() { return GetPressureSwitchValue(); }, nullptr);
}
