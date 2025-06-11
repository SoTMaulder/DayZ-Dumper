#include "Framework.h"

INT64 AutoOffset::ResolveMovCs(UINT64 Module, UINT64 Instruction) {

    int Relative = *(int*)(Instruction + 3);

    Instruction += 7;

    return (Instruction + Relative) - Module;
}

INT64 AutoOffset::ResolveCmpCs(UINT64 Module, UINT64 Instruction) {

    int Relative = *(int*)(Instruction + 2);

    Instruction += 7;

    return (Instruction + Relative) - Module;
}

INT64 AutoOffset::ResovleMovRegXmm(UINT64 Module, UINT64 Instruction) {
    return *(int*)(Instruction + 4);
}

INT64 AutoOffset::ResolveMovRegXmmLrg(UINT64 Module, UINT64 Instruction) {
    return *(int*)(Instruction + 5);
}

INT64 AutoOffset::ResovleMovRegXmmByte(UINT64 Module, UINT64 Instruction) {
    return *(BYTE*)(Instruction + 4);
}

INT64 AutoOffset::ResovleMovRegXmmLrgByte(UINT64 Module, UINT64 Instruction) {
    return *(BYTE*)(Instruction + 5);
}

INT64 AutoOffset::ResolveMovReg(UINT64 Module, UINT64 Instruction) {
    return *(int*)(Instruction + 3);
}

INT64 AutoOffset::ResolveMovRegByte(UINT64 Module, UINT64 Instruction) {
    return *(BYTE*)(Instruction + 3);
}

INT64 AutoOffset::ResolveMovRegSml(UINT64 Module, UINT64 Instruction) {
    return *(int*)(Instruction + 2);
}

INT64 AutoOffset::ResovleMovRegByteSml(UINT64 Module, UINT64 Instruction) {
    return *(BYTE*)(Instruction + 2);
}

INT64 AutoOffset::ResolveTraceMovReg(UINT64 Module, UINT64 Instruction) {

    /* we save this cause we do the pattern scan on the call - and then the offset inside the call routine */
    Instruction -= m_InstructionOffset;

    auto JmpRva = *(int*)(Instruction + 1);

    Instruction += 5 + JmpRva;

    Instruction += m_InstructionOffset;

    return *(int*)(Instruction + 3);
}

INT64 AutoOffset::ResolveTraceMovRegByte(UINT64 Module, UINT64 Instruction) {

    /* we save this cause we do the pattern scan on the call - and then the offset inside the call routine */
    Instruction -= m_InstructionOffset;

    auto JmpRva = *(int*)(Instruction + 1);

    Instruction += 5 + JmpRva;

    Instruction += m_InstructionOffset;

    return *(BYTE*)(Instruction + 3);
}

bool AutoOffset::ResolveOffset(UINT64 Module, UINT64 Instruction) {

    Instruction += m_InstructionOffset;

    switch (m_Type) {

    case ScanType::MovReg:          m_Offset = ResolveMovReg(Module, Instruction);              break;
    case ScanType::MovRegByte:      m_Offset = ResolveMovRegByte(Module, Instruction);          break;
    case ScanType::MovRegXmm:       m_Offset = ResovleMovRegXmm(Module, Instruction);           break;
    case ScanType::MovRegXmmLrg:    m_Offset = ResolveMovRegXmmLrg(Module, Instruction);        break;
    case ScanType::MovRegXmmByte:   m_Offset = ResovleMovRegXmmByte(Module, Instruction);       break;
    case ScanType::MovRegXmmLrgByte:m_Offset = ResovleMovRegXmmLrgByte(Module, Instruction);    break;
    case ScanType::MovRegSml:       m_Offset = ResolveMovRegSml(Module, Instruction);           break;
    case ScanType::MovRegByteSml:   m_Offset = ResovleMovRegByteSml(Module, Instruction);       break;
    case ScanType::TraceMovReg:     m_Offset = ResolveTraceMovReg(Module, Instruction);         break;
    case ScanType::TraceMovRegByte: m_Offset = ResolveTraceMovRegByte(Module, Instruction);     break;
    case ScanType::MovCs:           m_Offset = ResolveMovCs(Module, Instruction);               break;
    case ScanType::CmpCs:           m_Offset = ResolveCmpCs(Module, Instruction);               break;

    }

    m_Offset += m_LastOffset;

    return m_Offset;
}

bool AutoOffset::UpdateReference() {

    if (!m_Offset || !m_Reference)
        return false;

    *m_Reference = m_Offset;

    return true;
}

void AutoOffset::SetReference(INT64* Reference) {
    m_Reference = Reference;
}

void AutoOffset::SetPattern(PBYTE Pattern) {
    m_Pattern = Pattern;
}

void AutoOffset::SetMask(const char* Mask) {
    m_Mask = Mask;
}

void AutoOffset::SetSection(const char* Section) {
    m_Section = Section;
}

void AutoOffset::SetType(ScanType Type) {
    m_Type = Type;
}

void AutoOffset::SetOffset(UINT32 Offset) {
    m_InstructionOffset = Offset;
}

void AutoOffset::SetLastOffset(INT32 Offset) {
    m_LastOffset = Offset;
}

INT64 AutoOffset::GetOffset() {
    return m_Offset;
}

bool AutoOffset::Scan(UINT64 Module, PBYTE Allocated) {

    auto Instruction = Utils::PatternScan(
        Module,
        Allocated,
        m_Section,
        m_Pattern,
        m_Mask
    );

    if (!Instruction) /* oh no ! */
        return false;

    return ResolveOffset(Module, Instruction);
}

bool Updater::AllocateModule() {

    DWORD Pid = 0;

    if (!Utils::GetProcessId("DayZ_x64.exe", &Pid))
        return false;

    if (!Utils::GetProcessBase(Pid, &m_Module, &m_Allocated))
        return false;

    return true;
}

bool Updater::DeallocateModule() {

    FreeLibrary((HMODULE)(m_Module));

    m_Module = NULL;
    m_Allocated = NULL;

    return true;
}

void Updater::SetupModbasePatterns() {

    AUTO_OFFSET(Modbase, World, "\x48\x8B\x05\x00\x00\x00\x00\x48\x8D\x54\x24\x00\x48\x8B\x48\x30", "xxx????xxxx?xxxx", ".text", ScanType::MovCs, 0);
    AUTO_OFFSET(Modbase, Network, "\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x84\xC0\x75\x00\xE8", "xxx????x????xxx?x", ".text", ScanType::MovCs, 0);
    AUTO_OFFSET(Modbase, Tick, "\x48\x8B\x05\x00\x00\x00\x00\x0F\x57\xC9\x66\x0F\x6E\x03", "xxx????xxxxxxx", ".text", ScanType::MovCs, 0);
    AUTO_OFFSET(Modbase, ScriptContext, "\x48\x8B\x05\x00\x00\x00\x00\x48\x8B\xD9\x4C\x8B\x80", "xxx????xxxxxx", ".text", ScanType::MovCs, 0);

}

void Updater::SetupScriptContextPatterns() {

    AUTO_OFFSET(ScriptContext, ConstantTable, "\x48\x8B\x43\x00\x4D\x8B\xC4\x4C\x8B\x4C\x24\x00\x48\x8B\xCB", "xxx?xxxxxxx?xxx", ".text", ScanType::MovRegByte, 0);

}

void Updater::SetupNetworkPatterns() {



}

void Updater::SetupPlayerIdentityPatterns() {



}

void Updater::SetupWorldPatterns() {

    AUTO_OFFSET(World, BulletList, "\x48\x8B\x83\x00\x00\x00\x00\x48\x8D\x94\x24\x00\x00\x00\x00\x4C\x8B\x3C\xF0", "xxx????xxxx????xxxx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(World, NearEntList, "\x48\x8B\x83\x00\x00\x00\x00\x49\x8B\x14\x06\x48\x3B\xD5", "xxx????xxxxxxx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(World, FarEntList, "\x48\x8B\x83\x00\x00\x00\x00\x49\x8B\x0C\x06\x48\x3B\xCD\x74\x17\x80\xB9\x00\x00\x00\x00\x00\x75\x0E\x41\xB8\x00\x00\x00\x00\x0F\x28\xCE\xE8\x00\x00\x00\x00\xFF\xC6\x49\x83\xC6\x08\x3B\xB3\x00\x00\x00\x00\x7C\xCB", "xxx????xxxxxxxxxxx?????xxxx????xxxx????xxxxxxxx????xx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(World, Camera, "\x4C\x8B\x83\x00\x00\x00\x00\x4C\x8B\x11\x48\x89\x70\x08", "xxx????xxxxxxx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(World, LocalPlayer, "\x48\x8B\x81\x60\x00\x00\x00\x00\xD2\x48\x8B\x48\x08", "xxxx????xxxxx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(World, LocalOffset, "\xE8\x00\x00\x00\x00\x48\x8B\xC8\xC7\x44\x24\x00\x00\x00\x00\x00\x4C\x8D\x0D\x00\x00\x00\x00", "x????xxxxxx?????xxx????", ".text", ScanType::TraceMovReg, 16);
    AUTO_OFFSET(World, NoGrass, "\x8B\x81\x00\x00\x00\x00\x41\x89\x00\x77", "xx????xxxx", ".text", ScanType::MovRegSml, 0);

}

void Updater::SetupHumanPatterns() {

    AUTO_OFFSET(Human, HumanType, "\x4C\x8B\x91\x00\x00\x00\x00\x48\x8B\xCE\x4C\x89\x64\x24", "xxx????xxxxxxx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(Human, VisualState, "\x48\x8B\x9F\x00\x00\x00\x00\x49\x8B\xCE\xFF\x90\x00\x00\x00\x00\x8B\x10", "xxx????xxxxx????xx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(Human, LodShape, "\x4C\x8B\x91\x00\x00\x00\x00\x48\x0F\x44\xD7\x8B\x4B", "xxx????xxxxxx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(Human, Inventory, "\x48\x8B\x8B\x00\x00\x00\x00\x48\x8B\x01\xFF\x90\x00\x00\x00\x00\xEB\x02", "xxx????xxxxx????xx", ".text", ScanType::MovReg, 0);
}

void Updater::SetupDayZInfectedPatterns() {



}

void Updater::SetupHumanTypePatterns() {

    AUTO_OFFSET(HumanType, ObjectName, "\x48\x8B\x58\x70\x48\x85\xDB\x74\x03\xF0\xFF\x03\x48\x8B\x44\x24\x00\xBE\x00\x00\x00\x00\x48\x89\x5C\x24\x00\xEB\x21\x48\x8D\x0D\x00\x00\x00\x00", "xxxxxxxxxxxxxxxx?x????xxxx?xxxxx????", ".text", ScanType::MovRegByte, 0);
    AUTO_OFFSET(HumanType, CategoryName, "\x48\x8B\x81\x00\x00\x00\x00\x48\x8B\xF9\x0F\xB6\xF2\x48\x8D\x48\x10\x48\x85\xC0\x75\x07\x48\x8D\x0D\x00\x00\x00\x00\x48\x8D\x15\x00\x00\x00\x00\x48\x89\x6C\x24\x00\xE8\x00\x00\x00\x00", "xxx????xxxxxxxxxxxxxxxxxx????xxx????xxxx?x????", ".text", ScanType::MovReg, 0);

}

void Updater::SetupDayZLocalPatterns() {

    AUTO_OFFSET(DayZInfected, Skeleton, "\x48\x8B\x8B\x00\x00\x00\x00\x48\x8D\x55\x00\x40\x32\xFF", "xxx????xxx?xxx", ".text", ScanType::MovReg, 0);

}

void Updater::SetupDayZPlayerPatterns() {

    AUTO_OFFSET(DayZPlayer, Skeleton, "\x49\x8B\x97\x00\x00\x00\x00\xF3\x0F\x10\xB3", "xxx????xxxx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(DayZPlayer, NetworkID, "\x41\x8B\x92\x00\x00\x00\x00\x48\x8B\xC8", "xxx????xxx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(DayZPlayer, Inventory, "\x48\x8B\x8B\x00\x00\x00\x00\x48\x8B\x01\xFF\x90\x00\x00\x00\x00\xEB\x02", "xxx????xxxxx????xx", ".text", ScanType::MovReg, 0);

}

void Updater::SetupDayZPlayerInventoryPatterns() {

    AUTO_OFFSET(DayZPlayerInventory, Hands, "\x48\x8B\x8B\x00\x00\x00\x00\x48\x8B\xF8\x48\x85\xC9", "xxx????xxxxxx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(DayZPlayerInventory, Clothing, "\x48\x8B\x99\x00\x00\x00\x00\x41\x8B\xF0\x8B\xB9\x00\x00\x00\x00\x48\x8B\xE9", "xxx????xxxxx????xxx", ".text", ScanType::MovReg, 0);

}

void Updater::SetupInventoryItemPatterns() {

    AUTO_OFFSET(InventoryItem, ItemInventory, "\x48\x8B\x8B\x00\x00\x00\x00\x48\x8B\x01\xFF\x90\x00\x00\x00\x00\xEB\x02", "xxx????xxxxx????xx", ".text", ScanType::MovReg, 0);


}

void Updater::SetupWeaponPatterns() {

    AUTO_OFFSET(Weapon, WeaponIndex, "\x48\x8B\x81\x00\x00\x00\x00\x44\x8B\xC2\x49\xC1\xE0\x08", "xxx????xxxxxxx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(Weapon, WeaponInfoTable, "\x48\x8B\x81\x00\x00\x00\x00\x48\x85\xC0\x74\x00\xC7\x80", "xxx????xxxx?xx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(Weapon, MuzzleCount, "\x48\x89\xBB\x00\x00\x00\x00\x48\x89\xBB\x00\x00\x00\x00\x48\x89\xBB\x00\x00\x00\x00\x66\xC7\x83\x00\x00\x00\x00\x00\x00\xC6\x83", "xxx????xxx????xxx????xxx??????xx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(Weapon, WeaponInfoSize, "\x3B\x91\x00\x00\x00\x00\x73\x00\x8B\xCA", "xx????x?xx", ".text", ScanType::MovRegSml, 0);

}

void Updater::SetupWeaponInventoryPatterns() {

    AUTO_OFFSET(WeaponInventory, MagazineRef, "\x48\x8B\x83\x00\x00\x00\x00\xF3\x0F\x5D\x35", "xxx????xxxx", ".text", ScanType::MovReg, 0);

}

void Updater::SetupMagazinePatterns() {

    AUTO_OFFSET(Magazine, BulletList, "\x48\x8B\x83\x00\x00\x00\x00\x48\x8D\x94\x24\x00\x00\x00\x00\x4C\x8B\x3C\xF0", "xxx????xxxx????xxxx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(Magazine, BulletList2, "\x48\x8D\xB9\x00\x00\x00\x00\x48\x8B\xDA\xBD\xFF\xFF", "xxx????xxxxxx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(Magazine, MagazineType, "\x4C\x8B\x8F\x00\x00\x00\x00\x41\xB8", "xxx????xx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(Magazine, AmmoCount, "\x8B\xA9\x00\x00\x00\x00\x4C\x89\x7C\x24\x40\x4C\x8B\x00\x00\x00\x00", "xx????xxxxxxx????", ".text", ScanType::MovRegSml, 0);
}



void Updater::SetupAmmoTypePatterns() {

    AUTO_OFFSET(AmmoType, InitSpeed, "\x45\x8B\x95\x00\x00\x00\x00\x4C\x8D\x46", "xxx????xxx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(AmmoType, AirFriction, "\xF3\x45\x0F\x10\xAE\x00\x00\x00\x00\x44\x0F\x57\x2D", "xxxxx????xxxx", ".text", ScanType::MovRegXmmLrg, 0);

}

void Updater::SetupSkeletonPatterns() {

    AUTO_OFFSET(Skeleton, AnimClass1, "\x48\x8B\x8F\x00\x00\x00\x00\x48\x85\xC9\x74\x00\x8B\x01\xF0\x0F\xC1\x19", "xxx????xxxx?xxxxxx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(Skeleton, AnimClass2, "\x48\x8B\x50\x00\x48\x85\xD2\x74\x00\x48\x83\xC2\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xE9", "xxx?xxxx?xxx?xxx????x????x", ".text", ScanType::MovRegByte, 0);

}

void Updater::SetupAnimClassPatterns() {

    AUTO_OFFSET(AnimClass, MatrixArray, "\x49\x8B\x93\x00\x00\x00\x00\x8B\x01", "xxx????xx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(AnimClass, AnimComponent, "\x49\x8B\x9B\x00\x00\x00\x00\x66\x44\x89\x4C\x24", "xxx????xxxxx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(AnimClass, MatrixB, "\xF3\x0F\x10\x43\x00\xF3\x41\x0F\x59\xDB", "xxxx?xxxxx", ".text", ScanType::MovRegXmmByte, 0);

}

void Updater::SetupCameraPatterns() {

    AUTO_OFFSET(Camera, ViewMatrix, "\xF3\x0F\x10\x40\x00\xF3\x0F\x10\x50\x00\xF3\x0F\x10\x58\x00", "xxxx?xxxx?xxxx?", ".text", ScanType::MovRegXmmByte, 0);
    AUTO_OFFSET(Camera, ViewPortMatrix, "\xF3\x0F\x11\x47\x00\xF3\x0F\x10\x44\x24\x00\xF3\x0F\x11\x47\x00\xF3\x0F\x11\x4F\x00\x48\x89\x47\x00\xEB", "xxxx?xxxxx?xxxx?xxxx?xxx?x", ".text", ScanType::MovRegXmmByte, 0);

    AUTO_OFFSET(Camera, ViewProjection, "\x0F\x11\x86\x00\x00\x00\x00\x48\x8D\x8E\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8B\x96", "xxx????xxx????x????xxx", ".text", ScanType::MovRegByte, 0); //DONE

}

void Updater::SetupVisualStatePatterns() {

    AUTO_OFFSET(VisualState, Transform, "\xF3\x0F\x10\x40\x00\xF3\x0F\x10\x50\x00\xF3\x0F\x10\x58\x00", "xxxx?xxxx?xxxx?", ".text", ScanType::MovRegXmmByte, 0);
    AUTO_OFFSET(VisualState, InverseTransform, "\x89\x8B\x00\x00\x00\x00\x8B\x4A\x04", "xx????xxx", ".text", ScanType::MovRegSml, 0);

}

void Updater::SetupItemInventoryPatterns() {

    AUTO_OFFSET(ItemInventory, CargoGrid, "\x48\x8B\x8B\x00\x00\x00\x00\x49\x8B\xD6\x48\x8B\x01", "xxx????xxxxxx", ".text", ScanType::MovReg, 0);
    AUTO_OFFSET(ItemInventory, Quality, "\x0F\xBE\x89\x00\x00\x00\x00\x33\xD2\x84\xC9", "xxx????xxxx", ".text", ScanType::MovReg, 0);

}

void Updater::SetupCargoGridPatterns() {

    AUTO_OFFSET(CargoGrid, ItemList, "\x4C\x8B\x51\x00\x48\x8B\xF9\x41\x8B\xD9\x49\x8B\xF0", "xxx?xxxxxxxxx", ".text", ScanType::MovRegByte, 0);

}


bool Updater::SetupPatterns() {


    SetupModbasePatterns();
    SetupNetworkPatterns();
    SetupPlayerIdentityPatterns();
    SetupWorldPatterns();
    SetupHumanPatterns();
    SetupDayZInfectedPatterns();
    SetupHumanTypePatterns();
    SetupDayZLocalPatterns();
    SetupDayZPlayerPatterns();
    SetupDayZPlayerInventoryPatterns();
    SetupInventoryItemPatterns();
    SetupWeaponPatterns();
    SetupWeaponInventoryPatterns();
    SetupMagazinePatterns();
    SetupAmmoTypePatterns();
    SetupSkeletonPatterns();
    SetupAnimClassPatterns();
    SetupCameraPatterns();
    SetupVisualStatePatterns();
    SetupItemInventoryPatterns();
    SetupCargoGridPatterns();
    SetupScriptContextPatterns();

    return true;
}

bool Updater::Init() {

    if (!AllocateModule())
        return false;

    if (!SetupPatterns())
        return false;

    return true;
}

bool Updater::Scan() {

    for (auto& Data : m_Scans) {

#if _DEBUG
        Data.second.Scan(m_Module, m_Allocated);
#else 
        Data.Scan(m_Module, m_Allocated);
#endif

    }


    return true;
}

/* cluster fuck #if */
bool Updater::Release() {

    bool Result = true;

    for (auto& Data : m_Scans) {
#if _DEBUG
        if (!Data.second.UpdateReference()) {
#else
        if (!Data.UpdateReference()) {
#endif

#if _DEBUG
            printf("[OFFSET] Failed to get offset: %s\n", Data.first.c_str());
#endif

            Result = false;
        }

#if _DEBUG
        printf("[OFFSET: ] %-36s -> 0x%llX\n",
            Data.first.c_str(),
            (unsigned long long)Data.second.GetOffset());
#endif


        }

    m_Scans.clear();

    (void)DeallocateModule();

    return Result;
    }