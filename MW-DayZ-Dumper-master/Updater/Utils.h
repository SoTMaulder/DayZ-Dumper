#pragma once

namespace Utils {

	bool GetProcessId(const char* Process, PDWORD Pid);
	bool GetFullPath(DWORD ProcessId, const char** Path);
	bool GetProcessBase(const char* Process, PUINT64 ModuleBase, PBYTE* Allocated = NULL);
	bool GetProcessBase(DWORD ProcessId, PUINT64 ModuleBase, PBYTE* Allocated = NULL);

	UINT64 PatternScan(UINT64 Module, PBYTE Allocated, const char* Section, PBYTE Pattern, const char* Mask);

	bool DataCompare(PBYTE Data, PBYTE Pattern, const char* Mask);
}