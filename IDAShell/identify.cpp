// Copyright 2021 namazso <admin@namazso.eu>
// This file is part of IDAShell.
// 
// IDAShell is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// IDAShell is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with IDAShell.  If not, see <https://www.gnu.org/licenses/>.
#include "pch.h"
#include "identify.h"

enum : USHORT
{
  EM_NONE = 0,
  EM_M32 = 1,
  EM_SPARC = 2,
  EM_386 = 3,
  EM_68K = 4,
  EM_88K = 5,
  EM_486 = 6,
  EM_860 = 7,
  EM_MIPS = 8,
  EM_MIPS_RS3_LE = 10,
  EM_MIPS_RS4_BE = 10,
  EM_PARISC = 15,
  EM_SPARC32PLUS = 18,
  EM_PPC = 20,
  EM_PPC64 = 21,
  EM_SPU = 23,
  EM_ARM = 40,
  EM_SH = 42,
  EM_SPARCV9 = 43,
  EM_H8_300 = 46,
  EM_IA_64 = 50,
  EM_X86_64 = 62,
  EM_S390 = 22,
  EM_CRIS = 76,
  EM_M32R = 88,
  EM_MN10300 = 89,
  EM_OPENRISC = 92,
  EM_XTENSA = 94,
  EM_BLACKFIN = 106,
  EM_ALTERA_NIOS2 = 113,
  EM_TI_C6000 = 140,
  EM_AARCH64 = 183,
  EM_TILEPRO = 188,
  EM_MICROBLAZE = 189,
  EM_TILEGX = 191,
  EM_RISCV = 243,
  EM_BPF = 247,
  EM_CSKY = 252,
  EM_FRV = 0x5441,
  EM_ALPHA = 0x9026,
  EM_CYGNUS_M32R = 0x9041,
  EM_S390_OLD = 0xA390,
  EM_CYGNUS_MN10300 = 0xBEEF
};

constexpr static auto k_header_read = 4096u;

template <typename T>
static std::optional<T> read(char* buf, uint32_t offset)
{
  if (offset > k_header_read || offset + sizeof(T) - 1 > k_header_read)
    return {};
  T t{};
  memcpy(&t, buf + offset, sizeof(T));
  return { t };
}

using identify_fn = IDAType(*)(char* buf);

constexpr static identify_fn k_identify_functions[] =
{
  // MZ NE PE
  [](char* buf)
  {
    if (read<SHORT>(buf, 0) != IMAGE_DOS_SIGNATURE)
      return IDAType::Unsupported;
    const auto e_lfanew = read<ULONG>(buf, offsetof(IMAGE_DOS_HEADER, e_lfanew));
    if (!e_lfanew)
      return IDAType::Unsupported;
    if (read<ULONG>(buf, e_lfanew.value()) != IMAGE_NT_SIGNATURE)
      return IDAType::IDA32; // DOS or NE
    const auto machine = read<USHORT>(buf, e_lfanew.value() + offsetof(IMAGE_NT_HEADERS, FileHeader.Machine));
    if (!machine)
      return IDAType::Unsupported; // might be DOS but it'd be pretty impressive
    switch(machine.value())
    {
    case IMAGE_FILE_MACHINE_I386:
    case IMAGE_FILE_MACHINE_R3000:
    case IMAGE_FILE_MACHINE_R4000:
    case IMAGE_FILE_MACHINE_R10000:
    case IMAGE_FILE_MACHINE_WCEMIPSV2:
    case IMAGE_FILE_MACHINE_ALPHA:
    case IMAGE_FILE_MACHINE_SH3:
    case IMAGE_FILE_MACHINE_SH3DSP:
    case IMAGE_FILE_MACHINE_SH3E:
    case IMAGE_FILE_MACHINE_SH4:
    case IMAGE_FILE_MACHINE_SH5:
    case IMAGE_FILE_MACHINE_ARM:
    case IMAGE_FILE_MACHINE_THUMB:
    case IMAGE_FILE_MACHINE_ARMNT:
    case IMAGE_FILE_MACHINE_AM33:
    case IMAGE_FILE_MACHINE_POWERPC:
    case IMAGE_FILE_MACHINE_POWERPCFP:
    case IMAGE_FILE_MACHINE_MIPS16:
    case IMAGE_FILE_MACHINE_MIPSFPU:
    case IMAGE_FILE_MACHINE_MIPSFPU16:
    case IMAGE_FILE_MACHINE_TRICORE:
    case IMAGE_FILE_MACHINE_CEF:
    case IMAGE_FILE_MACHINE_EBC:
    case IMAGE_FILE_MACHINE_M32R:
    case 0x5032: // IMAGE_FILE_MACHINE_RISCV32 
      return IDAType::IDA32;

    case IMAGE_FILE_MACHINE_ARM64:
    case IMAGE_FILE_MACHINE_AMD64:
    case IMAGE_FILE_MACHINE_ALPHA64:
    case IMAGE_FILE_MACHINE_IA64:
    default: // who even makes 32 bit cpus anymore
      return IDAType::IDA64;
    }
  },

  // ELF
  [](char* buf)
  {
    if (read<LONG>(buf, 0) != 0x464C457F)
      return IDAType::Unsupported;
    const auto machine = read<USHORT>(buf, 0x12);
    if (!machine)
      return IDAType::Unsupported;
    switch (machine.value())
    {
    case EM_M32:
    case EM_SPARC:
    case EM_386:
    case EM_68K:
    case EM_88K:
    case EM_486:
    case EM_860:
    case EM_MIPS:
    case EM_MIPS_RS3_LE:
    //case EM_MIPS_RS4_BE:
    case EM_PARISC:
    case EM_SPARC32PLUS:
    case EM_PPC:
    case EM_SPU:
    case EM_ARM:
    case EM_SH:
    case EM_H8_300:
    case EM_S390:
    case EM_CRIS:
    case EM_M32R:
    case EM_MN10300:
    case EM_OPENRISC:
    case EM_XTENSA:
    case EM_BLACKFIN:
    case EM_ALTERA_NIOS2:
    case EM_TI_C6000:
    case EM_TILEPRO:
    case EM_MICROBLAZE:
    case EM_TILEGX:
    case EM_RISCV:
    case EM_BPF:
    case EM_CSKY:
    case EM_FRV:
    case EM_ALPHA:
    case EM_CYGNUS_M32R:
    case EM_S390_OLD:
    case EM_CYGNUS_MN10300:
      return IDAType::IDA32;
      
    case EM_PPC64:
    case EM_SPARCV9:
    case EM_IA_64:
    case EM_X86_64:
    case EM_AARCH64:
    default:
      return IDAType::IDA64;
    }
  },

  // Mach-O
  [](char* buf)
  {
    const auto magic = read<ULONG>(buf, 0);
    if (magic == 0xfeedface || magic == 0xcefaedfe)
      return IDAType::IDA32;
    if (magic == 0xfeedfacf || magic == 0xcffaedfe)
      return IDAType::IDA64;
    return IDAType::Unsupported;
  },
  
  // FAT Mach-O
  [](char* buf)
  {
    struct fat_header
    {
      uint32_t  magic;
      uint32_t  fat_arch_count;
    };

    struct fat_arch
    {
      uint32_t  cpu_type;
      uint32_t  cpu_subtype;
      uint32_t  offset;
      uint32_t  size;
      uint32_t  align;
    };

    enum : uint32_t
    {
      CPU_ARCH_MASK     = 0xff000000,
      CPU_ARCH_ABI64    = 0x01000000,
      CPU_ARCH_ABI64_32 = 0x02000000,
    };
    
    const auto hdr = read<fat_header>(buf, 0);

    unsigned long(*to_native)(unsigned long) = [](unsigned long v) { return v; };

    if (hdr->magic == 0xbebafeca)
      to_native = &_byteswap_ulong;
    else if (hdr->magic != 0xcafebabe)
      return IDAType::Unsupported;

    auto has_64 = false;
    auto has_32 = false;
    auto has_any = false;

    const auto count = to_native(hdr->fat_arch_count);

    for (auto i = 0u; i < count; ++i)
    {
      const auto arch = read<fat_arch>(buf, sizeof(fat_header) + i * sizeof(fat_arch));
      if (!arch)
        break;

      switch (to_native(arch->cpu_type) & CPU_ARCH_MASK)
      {
      case 0:
        has_32 = true;
        break;
      case CPU_ARCH_ABI64:
      case CPU_ARCH_ABI64_32: // watchOS?
        has_64 = true;
        break;
      case 0xFF000000: // -1 = any
        has_any = true;
        break;
      default:
        break;
      }
    }

    if (has_64)
      return IDAType::IDA64;
    if (has_32)
      return IDAType::IDA32;
    if (has_any)
      return IDAType::IDA64;

    // empty, corrupt, or wrong detection
    return IDAType::Unsupported;
  }
};

IDAType try_identify(const wchar_t* path)
{
  auto ret = IDAType::Unsupported;

  const auto handle = CreateFileW(
    path,
    GENERIC_READ,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
    nullptr,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    nullptr
  );

  if (handle != INVALID_HANDLE_VALUE)
  {
    char data[k_header_read]{};
    DWORD read{};
    const auto success = ReadFile(
      handle,
      data,
      (DWORD)std::size(data),
      &read,
      nullptr
    );

    if (success)
    {
      for (const auto fp : k_identify_functions)
      {
        ret = fp(data);
        if (ret != IDAType::Unsupported)
          break;
      }
    }

    CloseHandle(handle);
  }

  return ret;
}
