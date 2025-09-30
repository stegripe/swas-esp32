#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <Arduino.h>
#include <vector>

#include "models.h"

void initStorage();
void loadPersistentState();
void persistUsers();
void persistFingerprintIds();
void persistAll();
void ensureFingerprintTemplatesSynced();

void addOrUpdateFingerprintRecord(const FingerprintRecord& record);
bool removeFingerprintRecord(uint16_t fingerprintId);
bool getFingerprintRecord(uint16_t fingerprintId, FingerprintRecord* outRecord);
const std::vector<uint16_t>& getAdminFingerprintIds();
const std::vector<uint16_t>& getMahasiswaFingerprintIds();

#endif

