use std::ffi::CString;
use crate::bindings::*;
use std::os::raw::{c_char};
use std::panic;
use std::panic::AssertUnwindSafe;
use crate::MtcError;
use crate::handle_mtc_result;

pub type MtcResult<T> = Result<T, MtcError>;

pub enum MtcKey {
    NrFreePages = 0x0,
    NrZoneInactiveAnon = 0x1,
    NrZoneActiveAnon = 0x2,
    NrZoneInactiveFile = 0x3,
    NrZoneActiveFile = 0x4,
    NrZoneUnevictable = 0x5,
    NrZoneWritePending = 0x6,
    NrMlock = 0x7,
    NrBounce = 0x8,
    NrZspages = 0x9,
    NrFreeCma = 0xa,
    NrUnaccepted = 0xb,
    NumaHit = 0xc,
    NumaMiss = 0xd,
    NumaForeign = 0xe,
    NumaInterleave = 0xf,
    NumaLocal = 0x10,
    NumaOther = 0x11,
    NrInactiveAnon = 0x12,
    NrActiveAnon = 0x13,
    NrInactiveFile = 0x14,
    NrActiveFile = 0x15,
    NrUnevictable = 0x16,
    NrSlabReclaimable = 0x17,
    NrSlabUnreclaimable = 0x18,
    NrIsolatedAnon = 0x19,
    NrIsolatedFile = 0x1a,
    WorkingsetNodes = 0x1b,
    WorkingsetRefaultAnon = 0x1c,
    WorkingsetRefaultFile = 0x1d,
    WorkingsetActivateAnon = 0x1e,
    WorkingsetActivateFile = 0x1f,
    WorkingsetRestoreAnon = 0x20,
    WorkingsetRestoreFile = 0x21,
    WorkingsetNodereclaim = 0x22,
    NrAnonPages = 0x23,
    NrMapped = 0x24,
    NrFilePages = 0x25,
    NrDirty = 0x26,
    NrWriteback = 0x27,
    NrWritebackTemp = 0x28,
    NrShmem = 0x29,
    NrShmemHugepages = 0x2a,
    NrShmemPmdmapped = 0x2b,
    NrFileHugepages = 0x2c,
    NrFilePmdmapped = 0x2d,
    NrAnonTransparentHugepages = 0x2e,
    NrDirtyThreshold = 0x2f,
    NrDirtyBackgroundThreshold = 0x30,
    Pgpgin = 0x31,
    Pgpgout = 0x32,
    Pswpin = 0x33,
    Pswpout = 0x34,
    Pgfault = 0x35,
    Pgmajfault = 0x36,
    PgallocDma = 0x37,
    PgallocDma32 = 0x38,
    PgallocNormal = 0x39,
    PgallocMovable = 0x3a,
    PgallocDevice = 0x3b,
    AllocstallDma = 0x3c,
    AllocstallDma32 = 0x3d,
    AllocstallNormal = 0x3e,
    AllocstallMovable = 0x3f,
    AllocstallDevice = 0x40,
    PgskipDma = 0x41,
    PgskipDma32 = 0x42,
    PgskipNormal = 0x43,
    PgskipMovable = 0x44,
    PgskipDevice = 0x45,
    Pgfree = 0x46,
    Pgactivate = 0x47,
    Pgdeactivate = 0x48,
    Pglazyfree = 0x49,
    Pglazyfreed = 0x4a,
    Pgrefill = 0x4b,
    Pgreuse = 0x4c,
    PgstealKswapd = 0x4d,
    PgstealDirect = 0x4e,
    PgstealKhugepaged = 0x4f,
    PgscanKswapd = 0x50,
    PgscanDirect = 0x51,
    PgscanKhugepaged = 0x52,
    PgscanDirectThrottle = 0x53,
    PgscanAnon = 0x54,
    PgscanFile = 0x55,
    PgstealAnon = 0x56,
    PgstealFile = 0x57,
    ZoneReclaimFailed = 0x58,
    Pginodesteal = 0x59,
    SlabsScanned = 0x5a,
    KswapdInodesteal = 0x5b,
    KswapdLowWmarkHitQuickly = 0x5c,
    KswapdHighWmarkHitQuickly = 0x5d,
    Pageoutrun = 0x5e,
    Pgrotated = 0x5f,
    DropPagecache = 0x60,
    DropSlab = 0x61,
    OomKill = 0x62,
    NumaPteUpdates = 0x63,
    NumaHugePteUpdates = 0x64,
    NumaHintFaults = 0x65,
    NumaHintFaultsLocal = 0x66,
    NumaPagesMigrated = 0x67,
    PgmigrateSuccess = 0x68,
    PgmigrateFail = 0x69,
    ThpMigrationSuccess = 0x6a,
    ThpMigrationFail = 0x6b,
    ThpMigrationSplit = 0x6c,
    CompactMigrateScanned = 0x6d,
    CompactFreeScanned = 0x6e,
    CompactIsolated = 0x6f,
    CompactStall = 0x70,
    CompactFail = 0x71,
    CompactSuccess = 0x72,
    CompactDaemonWake = 0x73,
    CompactDaemonMigrateScanned = 0x74,
    CompactDaemonFreeScanned = 0x75,
    HtlbBuddyAllocSuccess = 0x76,
    HtlbBuddyAllocFail = 0x77,
    UnevictablePgsCulled = 0x78,
    UnevictablePgsScanned = 0x79,
    UnevictablePgsRescued = 0x7a,
    UnevictablePgsMlocked = 0x7b,
    UnevictablePgsMunlocked = 0x7c,
    UnevictablePgsCleared = 0x7d,
    UnevictablePgsStranded = 0x7e,
    ThpFaultAlloc = 0x7f,
    ThpFaultFallback = 0x80,
    ThpFaultFallbackCharge = 0x81,
    ThpCollapseAlloc = 0x82,
    ThpCollapseAllocFailed = 0x83,
    ThpFileAlloc = 0x84,
    ThpFileFallback = 0x85,
    ThpFileFallbackCharge = 0x86,
    ThpFileMapped = 0x87,
    ThpSplitPage = 0x88,
    ThpSplitPageFailed = 0x89,
    ThpDeferredSplitPage = 0x8a,
    ThpSplitPmd = 0x8b,
    ThpScanExceedNonePte = 0x8c,
    ThpScanExceedSwapPte = 0x8d,
    ThpScanExceedSharePte = 0x8e,
    ThpSplitPud = 0x8f,
    ThpZeroPageAlloc = 0x90,
    ThpZeroPageAllocFailed = 0x91,
    ThpSwpout = 0x92,
    ThpSwpoutFallback = 0x93,
    BalloonInflate = 0x94,
    BalloonDeflate = 0x95,
    BalloonMigrate = 0x96,
    SwapRa = 0x97,
    SwapRaHit = 0x98,
    KsmSwpinCopy = 0x99,
    CowKsm = 0x9a,
    Zswpin = 0x9b,
    Zswpout = 0x9c,
    Zswpwb = 0x9d,
    DirectMapLevel2Splits = 0x9e,
    DirectMapLevel3Splits = 0x9f,
    NrUnstable = 0xa0,
    Total = 0xa1,
    Free = 0xa2,
    Available = 0xa3,
    Buffers = 0xa4,
    Cached = 0xa5,
    SwapCache = 0xa6,
    Active = 0xa7,
    InActive = 0xa8,
    ActiveAnon = 0xa9,
    InActiveAnon = 0xaa,
    ActiveFile = 0xab,
    InActiveFile = 0xac,
    Unevictable = 0xad,
    MLocked = 0xae,
    SwapTotal = 0xaf,
    SwapFree = 0xb0,
    Zswap = 0xb1,
    Zswapped = 0xb2,
    Dirty = 0xb3,
    Writeback = 0xb4,
    PagesAnon = 0xb5,
    PageMapped = 0xb6,
    Shmem = 0xb7,
    KreClaimable = 0xb8,
    Slab = 0xb9,
    SrClaimable = 0xba,
    SunReclaim = 0xbb,
    KernelStack = 0xbc,
    PageTables = 0xbd,
    SecPageTables = 0xbe,
    NfsUnstable = 0xbf,
    Bounce = 0xc0,
    WritebackTmp = 0xc1,
    CommitLimit = 0xc2,
    CommittedAllocs = 0xc3,
    VmallocTotal = 0xc4,
    VmallocUsed = 0xc5,
    VmallocChunk = 0xc6,
    PerCPU = 0xc7,
    HardwareCorrupted = 0xc8,
    HugePagesAnon = 0xc9,
    HugePagesShmem = 0xca,
    PmdMappedShmem = 0xcb,
    HugePagesFile = 0xcc,
    PmdMappedFile = 0xcd,
    Unaccepted = 0xce,
    HugePagesTotal = 0xcf,
    HugePagesFree = 0xd0,
    HugePagesRsvd = 0xd1,
    HugePagesSurp = 0xd2,
    HugePageSize = 0xd3,
    HugePageTLB = 0xd4,
    DirectMap4k = 0xd5,
    DirectMap2M = 0xd6,
    DirectMap1G = 0xd7,
    OomAdj = 0xd8,
    OomScore = 0xd9,
    OomScoreAdj = 0xda,
    PSize = 0xdb,
    PResident = 0xdc,
    PShared = 0xdd,
    PText = 0xde,
    PData = 0xdf,
    PDirt = 0xe0
}

pub struct MtcObjectFfi {
    raw: MtcObject,
    values: Vec<MtcPointFfi>,
    owns_memory: bool,
}

pub struct MtcPointFfi {
    raw: MtcPoint,
}

pub struct MtcTimeFfi {
    raw: MtcTime,
}


impl MtcObjectFfi {

    pub fn new() -> MtcResult<Self> {
        let mut raw = MtcObject{
            point_map: std::ptr::null_mut(),
            times: std::ptr::null_mut(),
            size: 0,
            version: 0,
            _times_length: 0,
            _alloc_size_points: 0,
            _alloc_size_times: 0,
            file_length: 0,
            _key_size: 0,
        };

        let result = panic::catch_unwind(AssertUnwindSafe(|| unsafe {
            createMtcObject(&mut raw);
            ()
        }));

        handle_mtc_result!(result, MtcObjectFfi {raw, values: Vec::new(), owns_memory: true,},
                            initialisation_failure)
    }

    pub fn decode(&mut self, filename: &str) -> MtcResult<()> {
        let c_ptr: *const c_char = CString::new(filename).expect("CString::new failed").into_raw();
        let result = panic::catch_unwind(AssertUnwindSafe(|| unsafe {
            decode(c_ptr, &mut self.raw);
            ()
        }));

        handle_mtc_result!(result, (), decode_failure)
    }

    pub fn get_version(&self) -> u8 {
        self.raw.version
    }

    pub fn get_size(&self) -> u64 {
        self.raw.size
    }
    
    pub fn get_times(&self) -> MtcResult<Vec<u64>> {
        if self.get_size() == 0 {

        }
        let mut times = Vec::with_capacity(self.get_size() as usize);

        let result = panic::catch_unwind(AssertUnwindSafe(|| unsafe {
            let mut time_summation: u64 = 0;
            for i in 0..self.get_times_length() {
                let mtc_time_point = (*self.raw.times.wrapping_add(i as usize));
                if mtc_time_point.time_offset.is_null() {
                    panic!("Time offset {} is null", i);
                }
                for _ in i..(*self.raw.times.wrapping_add(i as usize)).repeated {
                    time_summation += (*mtc_time_point.time_offset) as u64;
                    times.push(time_summation);
                }
            }
        }));

        handle_mtc_result!(result, times, data_read_failure)
    }

    fn get_times_length(&self) -> u64 {
        self.raw._times_length
    }

    fn get_alloc_size_points(&self) -> u64 {
        self.raw._alloc_size_points
    }

    fn get_alloc_size_times(&self) -> u64 {
        self.raw._alloc_size_times
    }

    fn get_file_length(&self) -> u64 {
        self.raw.file_length
    }

    fn get_key_size(&self) -> mk_size_t {
        self.raw._key_size
    }
}

impl Drop for MtcObjectFfi {
    fn drop(&mut self) {
        if self.owns_memory {
            unsafe {
                destroyMtcObject(&mut self.raw);
            }
            self.owns_memory = false;
        }
    }
}

impl MtcKey {
    pub fn from_int(value: mk_size_t) -> Option<Self> {
        match value {
            0x0 => Some(Self::NrFreePages),
            0x1 => Some(Self::NrZoneInactiveAnon),
            0x2 => Some(Self::NrZoneActiveAnon),
            0x3 => Some(Self::NrZoneInactiveFile),
            0x4 => Some(Self::NrZoneActiveFile),
            0x5 => Some(Self::NrZoneUnevictable),
            0x6 => Some(Self::NrZoneWritePending),
            0x7 => Some(Self::NrMlock),
            0x8 => Some(Self::NrBounce),
            0x9 => Some(Self::NrZspages),
            0xa => Some(Self::NrFreeCma),
            0xb => Some(Self::NrUnaccepted),
            0xc => Some(Self::NumaHit),
            0xd => Some(Self::NumaMiss),
            0xe => Some(Self::NumaForeign),
            0xf => Some(Self::NumaInterleave),
            0x10 => Some(Self::NumaLocal),
            0x11 => Some(Self::NumaOther),
            0x12 => Some(Self::NrInactiveAnon),
            0x13 => Some(Self::NrActiveAnon),
            0x14 => Some(Self::NrInactiveFile),
            0x15 => Some(Self::NrActiveFile),
            0x16 => Some(Self::NrUnevictable),
            0x17 => Some(Self::NrSlabReclaimable),
            0x18 => Some(Self::NrSlabUnreclaimable),
            0x19 => Some(Self::NrIsolatedAnon),
            0x1a => Some(Self::NrIsolatedFile),
            0x1b => Some(Self::WorkingsetNodes),
            0x1c => Some(Self::WorkingsetRefaultAnon),
            0x1d => Some(Self::WorkingsetRefaultFile),
            0x1e => Some(Self::WorkingsetActivateAnon),
            0x1f => Some(Self::WorkingsetActivateFile),
            0x20 => Some(Self::WorkingsetRestoreAnon),
            0x21 => Some(Self::WorkingsetRestoreFile),
            0x22 => Some(Self::WorkingsetNodereclaim),
            0x23 => Some(Self::NrAnonPages),
            0x24 => Some(Self::NrMapped),
            0x25 => Some(Self::NrFilePages),
            0x26 => Some(Self::NrDirty),
            0x27 => Some(Self::NrWriteback),
            0x28 => Some(Self::NrWritebackTemp),
            0x29 => Some(Self::NrShmem),
            0x2a => Some(Self::NrShmemHugepages),
            0x2b => Some(Self::NrShmemPmdmapped),
            0x2c => Some(Self::NrFileHugepages),
            0x2d => Some(Self::NrFilePmdmapped),
            0x2e => Some(Self::NrAnonTransparentHugepages),
            0x2f => Some(Self::NrDirtyThreshold),
            0x30 => Some(Self::NrDirtyBackgroundThreshold),
            0x31 => Some(Self::Pgpgin),
            0x32 => Some(Self::Pgpgout),
            0x33 => Some(Self::Pswpin),
            0x34 => Some(Self::Pswpout),
            0x35 => Some(Self::Pgfault),
            0x36 => Some(Self::Pgmajfault),
            0x37 => Some(Self::PgallocDma),
            0x38 => Some(Self::PgallocDma32),
            0x39 => Some(Self::PgallocNormal),
            0x3a => Some(Self::PgallocMovable),
            0x3b => Some(Self::PgallocDevice),
            0x3c => Some(Self::AllocstallDma),
            0x3d => Some(Self::AllocstallDma32),
            0x3e => Some(Self::AllocstallNormal),
            0x3f => Some(Self::AllocstallMovable),
            0x40 => Some(Self::AllocstallDevice),
            0x41 => Some(Self::PgskipDma),
            0x42 => Some(Self::PgskipDma32),
            0x43 => Some(Self::PgskipNormal),
            0x44 => Some(Self::PgskipMovable),
            0x45 => Some(Self::PgskipDevice),
            0x46 => Some(Self::Pgfree),
            0x47 => Some(Self::Pgactivate),
            0x48 => Some(Self::Pgdeactivate),
            0x49 => Some(Self::Pglazyfree),
            0x4a => Some(Self::Pglazyfreed),
            0x4b => Some(Self::Pgrefill),
            0x4c => Some(Self::Pgreuse),
            0x4d => Some(Self::PgstealKswapd),
            0x4e => Some(Self::PgstealDirect),
            0x4f => Some(Self::PgstealKhugepaged),
            0x50 => Some(Self::PgscanKswapd),
            0x51 => Some(Self::PgscanDirect),
            0x52 => Some(Self::PgscanKhugepaged),
            0x53 => Some(Self::PgscanDirectThrottle),
            0x54 => Some(Self::PgscanAnon),
            0x55 => Some(Self::PgscanFile),
            0x56 => Some(Self::PgstealAnon),
            0x57 => Some(Self::PgstealFile),
            0x58 => Some(Self::ZoneReclaimFailed),
            0x59 => Some(Self::Pginodesteal),
            0x5a => Some(Self::SlabsScanned),
            0x5b => Some(Self::KswapdInodesteal),
            0x5c => Some(Self::KswapdLowWmarkHitQuickly),
            0x5d => Some(Self::KswapdHighWmarkHitQuickly),
            0x5e => Some(Self::Pageoutrun),
            0x5f => Some(Self::Pgrotated),
            0x60 => Some(Self::DropPagecache),
            0x61 => Some(Self::DropSlab),
            0x62 => Some(Self::OomKill),
            0x63 => Some(Self::NumaPteUpdates),
            0x64 => Some(Self::NumaHugePteUpdates),
            0x65 => Some(Self::NumaHintFaults),
            0x66 => Some(Self::NumaHintFaultsLocal),
            0x67 => Some(Self::NumaPagesMigrated),
            0x68 => Some(Self::PgmigrateSuccess),
            0x69 => Some(Self::PgmigrateFail),
            0x6a => Some(Self::ThpMigrationSuccess),
            0x6b => Some(Self::ThpMigrationFail),
            0x6c => Some(Self::ThpMigrationSplit),
            0x6d => Some(Self::CompactMigrateScanned),
            0x6e => Some(Self::CompactFreeScanned),
            0x6f => Some(Self::CompactIsolated),
            0x70 => Some(Self::CompactStall),
            0x71 => Some(Self::CompactFail),
            0x72 => Some(Self::CompactSuccess),
            0x73 => Some(Self::CompactDaemonWake),
            0x74 => Some(Self::CompactDaemonMigrateScanned),
            0x75 => Some(Self::CompactDaemonFreeScanned),
            0x76 => Some(Self::HtlbBuddyAllocSuccess),
            0x77 => Some(Self::HtlbBuddyAllocFail),
            0x78 => Some(Self::UnevictablePgsCulled),
            0x79 => Some(Self::UnevictablePgsScanned),
            0x7a => Some(Self::UnevictablePgsRescued),
            0x7b => Some(Self::UnevictablePgsMlocked),
            0x7c => Some(Self::UnevictablePgsMunlocked),
            0x7d => Some(Self::UnevictablePgsCleared),
            0x7e => Some(Self::UnevictablePgsStranded),
            0x7f => Some(Self::ThpFaultAlloc),
            0x80 => Some(Self::ThpFaultFallback),
            0x81 => Some(Self::ThpFaultFallbackCharge),
            0x82 => Some(Self::ThpCollapseAlloc),
            0x83 => Some(Self::ThpCollapseAllocFailed),
            0x84 => Some(Self::ThpFileAlloc),
            0x85 => Some(Self::ThpFileFallback),
            0x86 => Some(Self::ThpFileFallbackCharge),
            0x87 => Some(Self::ThpFileMapped),
            0x88 => Some(Self::ThpSplitPage),
            0x89 => Some(Self::ThpSplitPageFailed),
            0x8a => Some(Self::ThpDeferredSplitPage),
            0x8b => Some(Self::ThpSplitPmd),
            0x8c => Some(Self::ThpScanExceedNonePte),
            0x8d => Some(Self::ThpScanExceedSwapPte),
            0x8e => Some(Self::ThpScanExceedSharePte),
            0x8f => Some(Self::ThpSplitPud),
            0x90 => Some(Self::ThpZeroPageAlloc),
            0x91 => Some(Self::ThpZeroPageAllocFailed),
            0x92 => Some(Self::ThpSwpout),
            0x93 => Some(Self::ThpSwpoutFallback),
            0x94 => Some(Self::BalloonInflate),
            0x95 => Some(Self::BalloonDeflate),
            0x96 => Some(Self::BalloonMigrate),
            0x97 => Some(Self::SwapRa),
            0x98 => Some(Self::SwapRaHit),
            0x99 => Some(Self::KsmSwpinCopy),
            0x9a => Some(Self::CowKsm),
            0x9b => Some(Self::Zswpin),
            0x9c => Some(Self::Zswpout),
            0x9d => Some(Self::Zswpwb),
            0x9e => Some(Self::DirectMapLevel2Splits),
            0x9f => Some(Self::DirectMapLevel3Splits),
            0xa0 => Some(Self::NrUnstable),
            0xa1 => Some(Self::Total),
            0xa2 => Some(Self::Free),
            0xa3 => Some(Self::Available),
            0xa4 => Some(Self::Buffers),
            0xa5 => Some(Self::Cached),
            0xa6 => Some(Self::SwapCache),
            0xa7 => Some(Self::Active),
            0xa8 => Some(Self::InActive),
            0xa9 => Some(Self::ActiveAnon),
            0xaa => Some(Self::InActiveAnon),
            0xab => Some(Self::ActiveFile),
            0xac => Some(Self::InActiveFile),
            0xad => Some(Self::Unevictable),
            0xae => Some(Self::MLocked),
            0xaf => Some(Self::SwapTotal),
            0xb0 => Some(Self::SwapFree),
            0xb1 => Some(Self::Zswap),
            0xb2 => Some(Self::Zswapped),
            0xb3 => Some(Self::Dirty),
            0xb4 => Some(Self::Writeback),
            0xb5 => Some(Self::PagesAnon),
            0xb6 => Some(Self::PageMapped),
            0xb7 => Some(Self::Shmem),
            0xb8 => Some(Self::KreClaimable),
            0xb9 => Some(Self::Slab),
            0xba => Some(Self::SrClaimable),
            0xbb => Some(Self::SunReclaim),
            0xbc => Some(Self::KernelStack),
            0xbd => Some(Self::PageTables),
            0xbe => Some(Self::SecPageTables),
            0xbf => Some(Self::NfsUnstable),
            0xc0 => Some(Self::Bounce),
            0xc1 => Some(Self::WritebackTmp),
            0xc2 => Some(Self::CommitLimit),
            0xc3 => Some(Self::CommittedAllocs),
            0xc4 => Some(Self::VmallocTotal),
            0xc5 => Some(Self::VmallocUsed),
            0xc6 => Some(Self::VmallocChunk),
            0xc7 => Some(Self::PerCPU),
            0xc8 => Some(Self::HardwareCorrupted),
            0xc9 => Some(Self::HugePagesAnon),
            0xca => Some(Self::HugePagesShmem),
            0xcb => Some(Self::PmdMappedShmem),
            0xcc => Some(Self::HugePagesFile),
            0xcd => Some(Self::PmdMappedFile),
            0xce => Some(Self::Unaccepted),
            0xcf => Some(Self::HugePagesTotal),
            0xd0 => Some(Self::HugePagesFree),
            0xd1 => Some(Self::HugePagesRsvd),
            0xd2 => Some(Self::HugePagesSurp),
            0xd3 => Some(Self::HugePageSize),
            0xd4 => Some(Self::HugePageTLB),
            0xd5 => Some(Self::DirectMap4k),
            0xd6 => Some(Self::DirectMap2M),
            0xd7 => Some(Self::DirectMap1G),
            0xd8 => Some(Self::OomAdj),
            0xd9 => Some(Self::OomScore),
            0xda => Some(Self::OomScoreAdj),
            0xdb => Some(Self::PSize),
            0xdc => Some(Self::PResident),
            0xdd => Some(Self::PShared),
            0xde => Some(Self::PText),
            0xdf => Some(Self::PData),
            0xe0 => Some(Self::PDirt),
            _ => None,
        }
    }
}

trait MtcPointVariant {

    fn get_time_offset(&self) -> u16;

    fn get_repeated(&self) -> u64;

}


impl MtcPointVariant for MtcPointFfi {


    fn get_time_offset(&self) -> u16 {
        unsafe {
            *self.raw.time_offset
        }
    }

    fn get_repeated(&self) -> u64 {
        self.raw.repeated
    }
}

impl MtcPointFfi {
    pub fn get_value(&self) -> mtc_point_size_t {
        self.raw.value
    }
}

impl MtcPointVariant for MtcTimeFfi {

    fn get_time_offset(&self) -> u16 {
        unsafe {
            *self.raw.time_offset
        }
    }

    fn get_repeated(&self) -> u64 {
        self.raw.repeated
    }

}


