#pragma once
#include "../../Types.h"

#define PFS_FILE 2
#define PFS_DIR 3
#define PFS_CURRENT_DIR 4
#define PFS_PARENT_DIR 5


enum PfsMode : unsigned short {
    None = 0,
    Signed = 0x1,
    Is64Bit = 0x2,
    Encrypted = 0x4,
    UnknownFlagAlwaysSet = 0x8
};


struct PSFHeader_ { 
    S64 version;
    S64 magic;
    S64 Id;
    std::byte Fmode; // size = 1 byte
    std::byte Clean;
    std::byte ReadOnly;
    std::byte Rsv;
    PfsMode Mode;
    S16 Unk1;
    S32 BlockSize;
    S32 NBackup;
    S64 NBlock;
    S64 DinodeCount;
    S64 Ndblock;
    S64 DinodeBlockCount;
    S64 superroot_ino;
};

struct PFSCHdr
{
    S32 Magic;
    S32 Unk4;
    S32 Unk8;
    S32 BlockSz;
    S64 BlockSz2;
    S64 BlockOffsets;
    U64 DataStart;
    S64 DataLength;
};

enum InodeMode : U16
{
    o_read = 1,
    o_write = 2,
    o_execute = 4,
    g_read = 8,
    g_write = 16,
    g_execute = 32,
    u_read = 64,
    u_write = 128,
    u_execute = 256,
    dir = 16384,
    file = 32768,
};

enum InodeFlags : U32
{
    compressed = 0x1,
    unk1 = 0x2,
    unk2 = 0x4,
    unk3 = 0x8,
    readonly = 0x10,
    unk4 = 0x20,
    unk5 = 0x40,
    unk6 = 0x80,
    unk7 = 0x100,
    unk8 = 0x200,
    unk9 = 0x400,
    unk10 = 0x800,
    unk11 = 0x1000,
    unk12 = 0x2000,
    unk13 = 0x4000,
    unk14 = 0x8000,
    unk15 = 0x10000,
    internal = 0x20000
};

struct Inode //UNSIGNED, LE
{
    U16 Mode;
    U16 Nlink;
    U32 Flags;
    S64 Size;
    S64 SizeCompressed;
    S64 Time1_sec;
    S64 Time2_sec;
    S64 Time3_sec;
    S64 Time4_sec;
    U32 Time1_nsec;
    U32 Time2_nsec;
    U32 Time3_nsec;
    U32 Time4_nsec;
    U32 Uid;
    U32 Gid;
    U64 Unk1;
    U64 Unk2;
    U32 Blocks;
    U32 loc;
};

typedef struct {
    std::string name;
    U32 inode;
    U32 type;
} pfs_fs_table;


struct Dirent {
    S32 ino;
    S32 type;
    S32 namelen;
    S32 entsize;
    char name[512];
};

