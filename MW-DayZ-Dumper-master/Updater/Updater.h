#pragma once

enum class ScanType {
	MovReg,
	MovRegByte,
	MovRegSml,
	MovRegByteSml,
	MovRegXmm,
	MovRegXmmLrg,
	MovRegXmmByte,
	MovRegXmmLrgByte,
	TraceMovReg,		/* Unstable AF */
	TraceMovRegByte,	/* Unstable AF */
	MovCs,
	CmpCs,
};

class AutoOffset {
private:
	PBYTE m_Pattern;
	const char* m_Mask;
	const char* m_Section;
	INT64 m_Offset;
	INT64* m_Reference;
	ScanType m_Type;
	UINT32 m_InstructionOffset;
	INT32 m_LastOffset;

protected:
	INT64 ResolveMovCs(UINT64 Module, UINT64 Instruction);
	INT64 ResolveCmpCs(UINT64 Module, UINT64 Instruction);
	INT64 ResovleMovRegXmm(UINT64 Module, UINT64 Instruction);
	INT64 ResolveMovRegXmmLrg(UINT64 Module, UINT64 Instruction);
	INT64 ResovleMovRegXmmByte(UINT64 Module, UINT64 Instruction);
	INT64 ResovleMovRegXmmLrgByte(UINT64 Module, UINT64 Instruction);
	INT64 ResolveMovReg(UINT64 Module, UINT64 Instruction);
	INT64 ResolveMovRegByte(UINT64 Module, UINT64 Instruction);
	INT64 ResolveMovRegSml(UINT64 Module, UINT64 Instruction);
	INT64 ResovleMovRegByteSml(UINT64 Module, UINT64 Instruction);
	INT64 ResolveTraceMovReg(UINT64 Module, UINT64 Instruction);
	INT64 ResolveTraceMovRegByte(UINT64 Module, UINT64 Instruction);
	bool ResolveOffset(UINT64 Module, UINT64 Instruction);

public:
	bool UpdateReference();
	void SetReference(INT64* Reference);
	void SetPattern(PBYTE Pattern);
	void SetMask(const char* Mask);
	void SetSection(const char* Section);
	void SetType(ScanType Type);
	void SetOffset(UINT32 Offset);
	void SetLastOffset(INT32 Offset);

	INT64 GetOffset();

	virtual bool Scan(UINT64 Module, PBYTE Allocated);
};

#if _DEBUG
#define AUTO_OFFSET(Klass, Name, Pattern, Mask, Section, Type, Offset)	\
	AutoOffset Name;													\
	Name.SetPattern((PBYTE)Pattern);									\
	Name.SetMask(Mask);													\
	Name.SetSection(Section);											\
	Name.SetType(Type);													\
	Name.SetReference(&Offsets::Klass::Name);							\
	Name.SetOffset(Offset);												\
	m_Scans[#Klass##"::"#Name] = Name; 

#define AUTO_OFFSET_MANUAL(Klass, Name, Pattern, Mask, Section, Type, Offset, LastOffset)	\
	AutoOffset Name;													\
	Name.SetPattern((PBYTE)Pattern);									\
	Name.SetMask(Mask);													\
	Name.SetSection(Section);											\
	Name.SetType(Type);													\
	Name.SetReference(&Offsets::Klass::Name);							\
	Name.SetOffset(Offset);												\
	Name.SetLastOffset(LastOffset);										\
	m_Scans[#Klass##"::"#Name] = Name;

#else

#define AUTO_OFFSET(Klass, Name, Pattern, Mask, Section, Type, Offset)	\
	AutoOffset Name;													\
	Name.SetPattern((PBYTE)Pattern);									\
	Name.SetMask(Mask);													\
	Name.SetSection(Section);											\
	Name.SetType(Type);													\
	Name.SetReference(&Offsets::Klass::Name);							\
	Name.SetOffset(Offset);												\
	m_Scans.push_back(Name);


#define AUTO_OFFSET_MANUAL(Klass, Name, Pattern, Mask, Section, Type, Offset, LastOffset)	\
	AutoOffset Name;													\
	Name.SetPattern((PBYTE)Pattern);									\
	Name.SetMask(Mask);													\
	Name.SetSection(Section);											\
	Name.SetType(Type);													\
	Name.SetReference(&Offsets::Klass::Name);							\
	Name.SetOffset(Offset);												\
	Name.SetLastOffset(LastOffset);										\
	m_Scans.push_back(Name);

#endif


class Updater {
public:
#if _DEBUG
	std::unordered_map<std::string, AutoOffset> m_Scans;
#else
	std::vector<AutoOffset> m_Scans;
#endif
	
	UINT64 m_Module;
	PBYTE m_Allocated;

private:
	bool AllocateModule();
	bool DeallocateModule();

private: /* sub setuppatterns here :p */
	void SetupModbasePatterns();
	void SetupNetworkPatterns();
	void SetupPlayerIdentityPatterns();
	void SetupWorldPatterns();
	void SetupHumanPatterns();
	void SetupDayZInfectedPatterns();
	void SetupHumanTypePatterns();
	void SetupDayZLocalPatterns();
	void SetupDayZPlayerPatterns();
	void SetupDayZPlayerInventoryPatterns();
	void SetupInventoryItemPatterns();
	void SetupWeaponPatterns();
	void SetupWeaponInventoryPatterns();
	void SetupMagazinePatterns();
	void SetupAmmoTypePatterns();
	void SetupSkeletonPatterns();
	void SetupAnimClassPatterns();
	void SetupCameraPatterns();
	void SetupVisualStatePatterns();

private:
	bool SetupPatterns();

public:
	bool Init();
	bool Scan();
	bool Release();

};

extern Updater* g_Updater;