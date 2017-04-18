
# Really simple expression types to handle arithmetic expressions referring
# to other # defines
class MacroExp: pass
class MacroRef(MacroExp):
  def __init__(s, name):
    s._name = name;

  def eval(s):
    return lookup(s._name)

  def __add__(s, o):
    return MacroAdd(s, o)

  def __radd__(s, o):
    return MacroAdd(o, s)

class MacroAdd(MacroExp):
  def __init__(s, l, r):
    s.l = l;
    s.r = r;

  def eval(s):
    l = s.l.eval() if (isinstance(s.l, MacroExp)) else s.l
    r = s.r.eval() if (isinstance(s.r, MacroExp)) else s.r
      
    return l + r

  def __add__(s, o):
    return MacroAdd(s, o)

  def __radd__(s, o):
    return MacroAdd(o, s)


def lookup(n):
  v = _consts[n]
  if isinstance(v, MacroExp):
    return v.eval()
  else:
    return v
  
# Macro constants
_consts = {
		"EI_NIDENT": 16	, # 
		"EI_MAG0": 0		, #  File identification byte 0 index 
		"ELFMAG0": 0x7f		, #  Magic number byte 0 
		"EI_MAG1": 1		, #  File identification byte 1 index 
		"ELFMAG1": 'E'		, #  Magic number byte 1 
		"EI_MAG2": 2		, #  File identification byte 2 index 
		"ELFMAG2": 'L'		, #  Magic number byte 2 
		"EI_MAG3": 3		, #  File identification byte 3 index 
		"ELFMAG3": 'F'		, #  Magic number byte 3 
		"ELFMAG": "\177ELF"	, # 
		"SELFMAG": 4	, # 
		"EI_CLASS": 4		, #  File class byte index 
		"ELFCLASSNONE": 0		, #  Invalid class 
		"ELFCLASS32": 1		, #  32-bit objects 
		"ELFCLASS64": 2		, #  64-bit objects 
		"ELFCLASSNUM": 3	, # 
		"EI_DATA": 5		, #  Data encoding byte index 
		"ELFDATANONE": 0		, #  Invalid data encoding 
		"ELFDATA2LSB": 1		, #  2's complement, little endian 
		"ELFDATA2MSB": 2		, #  2's complement, big endian 
		"ELFDATANUM": 3	, # 
		"EI_VERSION": 6		, #  File version byte index 
		"EI_OSABI": 7		, #  OS ABI identification 
		"ELFOSABI_NONE": 0	, #  UNIX System V ABI 
		"ELFOSABI_SYSV": 0	, #  Alias.  
		"ELFOSABI_HPUX": 1	, #  HP-UX 
		"ELFOSABI_NETBSD": 2	, #  NetBSD.  
		"ELFOSABI_GNU": 3	, #  Object uses GNU ELF extensions.  
		"ELFOSABI_LINUX": MacroRef("ELFOSABI_GNU") , #  Compatibility alias.  
		"ELFOSABI_SOLARIS": 6	, #  Sun Solaris.  
		"ELFOSABI_AIX": 7	, #  IBM AIX.  
		"ELFOSABI_IRIX": 8	, #  SGI Irix.  
		"ELFOSABI_FREEBSD": 9	, #  FreeBSD.  
		"ELFOSABI_TRU64": 10	, #  Compaq TRU64 UNIX.  
		"ELFOSABI_MODESTO": 11	, #  Novell Modesto.  
		"ELFOSABI_OPENBSD": 12	, #  OpenBSD.  
		"ELFOSABI_ARM_AEABI": 64	, #  ARM EABI 
		"ELFOSABI_ARM": 97	, #  ARM 
		"ELFOSABI_STANDALONE": 255	, #  Standalone (embedded) application 
		"EI_ABIVERSION": 8		, #  ABI version 
		"EI_PAD": 9		, #  Byte index of padding bytes 
		"ET_NONE": 0		, #  No file type 
		"ET_REL": 1		, #  Relocatable file 
		"ET_EXEC": 2		, #  Executable file 
		"ET_DYN": 3		, #  Shared object file 
		"ET_CORE": 4		, #  Core file 
		"ET_NUM": 5		, #  Number of defined types 
		"ET_LOOS": 0xfe00		, #  OS-specific range start 
		"ET_HIOS": 0xfeff		, #  OS-specific range end 
		"ET_LOPROC": 0xff00		, #  Processor-specific range start 
		"ET_HIPROC": 0xffff		, #  Processor-specific range end 
		"EM_NONE": 0		, #  No machine 
		"EM_M32": 1		, #  AT&T WE 32100 
		"EM_SPARC": 2		, #  SUN SPARC 
		"EM_386": 3		, #  Intel 80386 
		"EM_68K": 4		, #  Motorola m68k family 
		"EM_88K": 5		, #  Motorola m88k family 
		"EM_860": 7		, #  Intel 80860 
		"EM_MIPS": 8		, #  MIPS R3000 big-endian 
		"EM_S370": 9		, #  IBM System/370 
		"EM_MIPS_RS3_LE": 10		, #  MIPS R3000 little-endian 
		"EM_PARISC": 15		, #  HPPA 
		"EM_VPP500": 17		, #  Fujitsu VPP500 
		"EM_SPARC32PLUS": 18		, #  Sun's "v8plus" 
		"EM_960": 19		, #  Intel 80960 
		"EM_PPC": 20		, #  PowerPC 
		"EM_PPC64": 21		, #  PowerPC 64-bit 
		"EM_S390": 22		, #  IBM S390 
		"EM_V800": 36		, #  NEC V800 series 
		"EM_FR20": 37		, #  Fujitsu FR20 
		"EM_RH32": 38		, #  TRW RH-32 
		"EM_RCE": 39		, #  Motorola RCE 
		"EM_ARM": 40		, #  ARM 
		"EM_FAKE_ALPHA": 41		, #  Digital Alpha 
		"EM_SH": 42		, #  Hitachi SH 
		"EM_SPARCV9": 43		, #  SPARC v9 64-bit 
		"EM_TRICORE": 44		, #  Siemens Tricore 
		"EM_ARC": 45		, #  Argonaut RISC Core 
		"EM_H8_300": 46		, #  Hitachi H8/300 
		"EM_H8_300H": 47		, #  Hitachi H8/300H 
		"EM_H8S": 48		, #  Hitachi H8S 
		"EM_H8_500": 49		, #  Hitachi H8/500 
		"EM_IA_64": 50		, #  Intel Merced 
		"EM_MIPS_X": 51		, #  Stanford MIPS-X 
		"EM_COLDFIRE": 52		, #  Motorola Coldfire 
		"EM_68HC12": 53		, #  Motorola M68HC12 
		"EM_MMA": 54		, #  Fujitsu MMA Multimedia Accelerator
		"EM_PCP": 55		, #  Siemens PCP 
		"EM_NCPU": 56		, #  Sony nCPU embeeded RISC 
		"EM_NDR1": 57		, #  Denso NDR1 microprocessor 
		"EM_STARCORE": 58		, #  Motorola Start*Core processor 
		"EM_ME16": 59		, #  Toyota ME16 processor 
		"EM_ST100": 60		, #  STMicroelectronic ST100 processor 
		"EM_TINYJ": 61		, #  Advanced Logic Corp. Tinyj emb.fam
		"EM_X86_64": 62		, #  AMD x86-64 architecture 
		"EM_PDSP": 63		, #  Sony DSP Processor 
		"EM_FX66": 66		, #  Siemens FX66 microcontroller 
		"EM_ST9PLUS": 67		, #  STMicroelectronics ST9+ 8/16 mc 
		"EM_ST7": 68		, #  STmicroelectronics ST7 8 bit mc 
		"EM_68HC16": 69		, #  Motorola MC68HC16 microcontroller 
		"EM_68HC11": 70		, #  Motorola MC68HC11 microcontroller 
		"EM_68HC08": 71		, #  Motorola MC68HC08 microcontroller 
		"EM_68HC05": 72		, #  Motorola MC68HC05 microcontroller 
		"EM_SVX": 73		, #  Silicon Graphics SVx 
		"EM_ST19": 74		, #  STMicroelectronics ST19 8 bit mc 
		"EM_VAX": 75		, #  Digital VAX 
		"EM_CRIS": 76		, #  Axis Communications 32-bit embedded processor 
		"EM_JAVELIN": 77		, #  Infineon Technologies 32-bit embedded processor 
		"EM_FIREPATH": 78		, #  Element 14 64-bit DSP Processor 
		"EM_ZSP": 79		, #  LSI Logic 16-bit DSP Processor 
		"EM_MMIX": 80		, #  Donald Knuth's educational 64-bit processor 
		"EM_HUANY": 81		, #  Harvard University machine-independent object files 
		"EM_PRISM": 82		, #  SiTera Prism 
		"EM_AVR": 83		, #  Atmel AVR 8-bit microcontroller 
		"EM_FR30": 84		, #  Fujitsu FR30 
		"EM_D10V": 85		, #  Mitsubishi D10V 
		"EM_D30V": 86		, #  Mitsubishi D30V 
		"EM_V850": 87		, #  NEC v850 
		"EM_M32R": 88		, #  Mitsubishi M32R 
		"EM_MN10300": 89		, #  Matsushita MN10300 
		"EM_MN10200": 90		, #  Matsushita MN10200 
		"EM_PJ": 91		, #  picoJava 
		"EM_OPENRISC": 92		, #  OpenRISC 32-bit embedded processor 
		"EM_ARC_A5": 93		, #  ARC Cores Tangent-A5 
		"EM_XTENSA": 94		, #  Tensilica Xtensa Architecture 
		"EM_NUM": 95	, # 
		"EM_ALPHA": 0x9026	, # 
		"EV_NONE": 0		, #  Invalid ELF version 
		"EV_CURRENT": 1		, #  Current version 
		"EV_NUM": 2	, # 
		"SHN_UNDEF": 0		, #  Undefined section 
		"SHN_LORESERVE": 0xff00		, #  Start of reserved indices 
		"SHN_LOPROC": 0xff00		, #  Start of processor-specific 
		"SHN_BEFORE": 0xff00		, #  Order section before all others
		"SHN_AFTER": 0xff01		, #  Order section after all others
		"SHN_HIPROC": 0xff1f		, #  End of processor-specific 
		"SHN_LOOS": 0xff20		, #  Start of OS-specific 
		"SHN_HIOS": 0xff3f		, #  End of OS-specific 
		"SHN_ABS": 0xfff1		, #  Associated symbol is absolute 
		"SHN_COMMON": 0xfff2		, #  Associated symbol is common 
		"SHN_XINDEX": 0xffff		, #  Index is in extra table.  
		"SHN_HIRESERVE": 0xffff		, #  End of reserved indices 
		"SHT_NULL": 0		, #  Section header table entry unused 
		"SHT_PROGBITS": 1		, #  Program data 
		"SHT_SYMTAB": 2		, #  Symbol table 
		"SHT_STRTAB": 3		, #  String table 
		"SHT_RELA": 4		, #  Relocation entries with addends 
		"SHT_HASH": 5		, #  Symbol hash table 
		"SHT_DYNAMIC": 6		, #  Dynamic linking information 
		"SHT_NOTE": 7		, #  Notes 
		"SHT_NOBITS": 8		, #  Program space with no data (bss) 
		"SHT_REL": 9		, #  Relocation entries, no addends 
		"SHT_SHLIB": 10		, #  Reserved 
		"SHT_DYNSYM": 11		, #  Dynamic linker symbol table 
		"SHT_INIT_ARRAY": 14		, #  Array of constructors 
		"SHT_FINI_ARRAY": 15		, #  Array of destructors 
		"SHT_PREINIT_ARRAY": 16		, #  Array of pre-constructors 
		"SHT_GROUP": 17		, #  Section group 
		"SHT_SYMTAB_SHNDX": 18		, #  Extended section indeces 
		"SHT_NUM": 19		, #  Number of defined types.  
		"SHT_LOOS": 0x60000000	, #  Start OS-specific.  
		"SHT_GNU_ATTRIBUTES": 0x6ffffff5	, #  Object attributes.  
		"SHT_GNU_HASH": 0x6ffffff6	, #  GNU-style hash table.  
		"SHT_GNU_LIBLIST": 0x6ffffff7	, #  Prelink library list 
		"SHT_CHECKSUM": 0x6ffffff8	, #  Checksum for DSO content.  
		"SHT_LOSUNW": 0x6ffffffa	, #  Sun-specific low bound.  
		"SHT_SUNW_move": 0x6ffffffa	, # 
		"SHT_SUNW_COMDAT": 0x6ffffffb	, # 
		"SHT_SUNW_syminfo": 0x6ffffffc	, # 
		"SHT_GNU_verdef": 0x6ffffffd	, #  Version definition section.  
		"SHT_GNU_verneed": 0x6ffffffe	, #  Version needs section.  
		"SHT_GNU_versym": 0x6fffffff	, #  Version symbol table.  
		"SHT_HISUNW": 0x6fffffff	, #  Sun-specific high bound.  
		"SHT_HIOS": 0x6fffffff	, #  End OS-specific type 
		"SHT_LOPROC": 0x70000000	, #  Start of processor-specific 
		"SHT_HIPROC": 0x7fffffff	, #  End of processor-specific 
		"SHT_LOUSER": 0x80000000	, #  Start of application-specific 
		"SHT_HIUSER": 0x8fffffff	, #  End of application-specific 
		"SHF_MASKOS": 0x0ff00000	, #  OS-specific.  
		"SHF_MASKPROC": 0xf0000000	, #  Processor-specific 
		"SHF_ORDERED": (1 << 30)	, #  Special ordering requirement
		"SHF_EXCLUDE": (1 << 31)	, #  Section is excluded unless
		"GRP_COMDAT": 0x1		, #  Mark group as COMDAT.  
		"SYMINFO_BT_SELF": 0xffff	, #  Symbol bound to self 
		"SYMINFO_BT_PARENT": 0xfffe	, #  Symbol bound to parent 
		"SYMINFO_BT_LOWRESERVE": 0xff00	, #  Beginning of reserved entries 
		"SYMINFO_FLG_DIRECT": 0x0001	, #  Direct bound symbol 
		"SYMINFO_FLG_PASSTHRU": 0x0002	, #  Pass-thru symbol for translator 
		"SYMINFO_FLG_COPY": 0x0004	, #  Symbol is a copy-reloc 
		"SYMINFO_FLG_LAZYLOAD": 0x0008	, #  Symbol bound to object to be lazy
		"SYMINFO_NONE": 0	, # 
		"SYMINFO_CURRENT": 1	, # 
		"SYMINFO_NUM": 2	, # 
		"STB_LOCAL": 0		, #  Local symbol 
		"STB_GLOBAL": 1		, #  Global symbol 
		"STB_WEAK": 2		, #  Weak symbol 
		"STB_NUM": 3		, #  Number of defined types.  
		"STB_LOOS": 10		, #  Start of OS-specific 
		"STB_GNU_UNIQUE": 10		, #  Unique symbol.  
		"STB_HIOS": 12		, #  End of OS-specific 
		"STB_LOPROC": 13		, #  Start of processor-specific 
		"STB_HIPROC": 15		, #  End of processor-specific 
		"STT_NOTYPE": 0		, #  Symbol type is unspecified 
		"STT_OBJECT": 1		, #  Symbol is a data object 
		"STT_FUNC": 2		, #  Symbol is a code object 
		"STT_SECTION": 3		, #  Symbol associated with a section 
		"STT_FILE": 4		, #  Symbol's name is file name 
		"STT_COMMON": 5		, #  Symbol is a common data object 
		"STT_TLS": 6		, #  Symbol is thread-local data object
		"STT_NUM": 7		, #  Number of defined types.  
		"STT_LOOS": 10		, #  Start of OS-specific 
		"STT_GNU_IFUNC": 10		, #  Symbol is indirect code object 
		"STT_HIOS": 12		, #  End of OS-specific 
		"STT_LOPROC": 13		, #  Start of processor-specific 
		"STT_HIPROC": 15		, #  End of processor-specific 
		"STN_UNDEF": 0		, #  End of a chain.  
		"STV_DEFAULT": 0		, #  Default symbol visibility rules 
		"STV_INTERNAL": 1		, #  Processor specific hidden class 
		"STV_HIDDEN": 2		, #  Sym unavailable in other modules 
		"STV_PROTECTED": 3		, #  Not preemptible, not exported 
		"PN_XNUM": 0xffff	, # 
		"PT_NULL": 0		, #  Program header table entry unused 
		"PT_LOAD": 1		, #  Loadable program segment 
		"PT_DYNAMIC": 2		, #  Dynamic linking information 
		"PT_INTERP": 3		, #  Program interpreter 
		"PT_NOTE": 4		, #  Auxiliary information 
		"PT_SHLIB": 5		, #  Reserved 
		"PT_PHDR": 6		, #  Entry for header table itself 
		"PT_TLS": 7		, #  Thread-local storage segment 
		"PT_NUM": 8		, #  Number of defined types 
		"PT_LOOS": 0x60000000	, #  Start of OS-specific 
		"PT_GNU_EH_FRAME": 0x6474e550	, #  GCC .eh_frame_hdr segment 
		"PT_GNU_STACK": 0x6474e551	, #  Indicates stack executability 
		"PT_GNU_RELRO": 0x6474e552	, #  Read-only after relocation 
		"PT_LOSUNW": 0x6ffffffa	, # 
		"PT_SUNWBSS": 0x6ffffffa	, #  Sun Specific segment 
		"PT_SUNWSTACK": 0x6ffffffb	, #  Stack segment 
		"PT_HISUNW": 0x6fffffff	, # 
		"PT_HIOS": 0x6fffffff	, #  End of OS-specific 
		"PT_LOPROC": 0x70000000	, #  Start of processor-specific 
		"PT_HIPROC": 0x7fffffff	, #  End of processor-specific 
		"PF_X": (1 << 0)	, #  Segment is executable 
		"PF_W": (1 << 1)	, #  Segment is writable 
		"PF_R": (1 << 2)	, #  Segment is readable 
		"PF_MASKOS": 0x0ff00000	, #  OS-specific 
		"PF_MASKPROC": 0xf0000000	, #  Processor-specific 
		"NT_PRSTATUS": 1		, #  Contains copy of prstatus struct 
		"NT_FPREGSET": 2		, #  Contains copy of fpregset struct 
		"NT_PRPSINFO": 3		, #  Contains copy of prpsinfo struct 
		"NT_PRXREG": 4		, #  Contains copy of prxregset struct 
		"NT_TASKSTRUCT": 4		, #  Contains copy of task structure 
		"NT_PLATFORM": 5		, #  String from sysinfo(SI_PLATFORM) 
		"NT_AUXV": 6		, #  Contains copy of auxv array 
		"NT_GWINDOWS": 7		, #  Contains copy of gwindows struct 
		"NT_ASRS": 8		, #  Contains copy of asrset struct 
		"NT_PSTATUS": 10		, #  Contains copy of pstatus struct 
		"NT_PSINFO": 13		, #  Contains copy of psinfo struct 
		"NT_PRCRED": 14		, #  Contains copy of prcred struct 
		"NT_UTSNAME": 15		, #  Contains copy of utsname struct 
		"NT_LWPSTATUS": 16		, #  Contains copy of lwpstatus struct 
		"NT_LWPSINFO": 17		, #  Contains copy of lwpinfo struct 
		"NT_PRFPXREG": 20		, #  Contains copy of fprxregset struct 
		"NT_PRXFPREG": 0x46e62b7f	, #  Contains copy of user_fxsr_struct 
		"NT_PPC_VMX": 0x100		, #  PowerPC Altivec/VMX registers 
		"NT_PPC_SPE": 0x101		, #  PowerPC SPE/EVR registers 
		"NT_PPC_VSX": 0x102		, #  PowerPC VSX registers 
		"NT_386_TLS": 0x200		, #  i386 TLS slots (struct user_desc) 
		"NT_386_IOPERM": 0x201		, #  x86 io permission bitmap (1=deny) 
		"NT_X86_XSTATE": 0x202		, #  x86 extended state using xsave 
		"NT_VERSION": 1		, #  Contains a version string.  
		"DT_NULL": 0		, #  Marks end of dynamic section 
		"DT_NEEDED": 1		, #  Name of needed library 
		"DT_PLTRELSZ": 2		, #  Size in bytes of PLT relocs 
		"DT_PLTGOT": 3		, #  Processor defined value 
		"DT_HASH": 4		, #  Address of symbol hash table 
		"DT_STRTAB": 5		, #  Address of string table 
		"DT_SYMTAB": 6		, #  Address of symbol table 
		"DT_RELA": 7		, #  Address of Rela relocs 
		"DT_RELASZ": 8		, #  Total size of Rela relocs 
		"DT_RELAENT": 9		, #  Size of one Rela reloc 
		"DT_STRSZ": 10		, #  Size of string table 
		"DT_SYMENT": 11		, #  Size of one symbol table entry 
		"DT_INIT": 12		, #  Address of init function 
		"DT_FINI": 13		, #  Address of termination function 
		"DT_SONAME": 14		, #  Name of shared object 
		"DT_RPATH": 15		, #  Library search path (deprecated) 
		"DT_SYMBOLIC": 16		, #  Start symbol search here 
		"DT_REL": 17		, #  Address of Rel relocs 
		"DT_RELSZ": 18		, #  Total size of Rel relocs 
		"DT_RELENT": 19		, #  Size of one Rel reloc 
		"DT_PLTREL": 20		, #  Type of reloc in PLT 
		"DT_DEBUG": 21		, #  For debugging; unspecified 
		"DT_TEXTREL": 22		, #  Reloc might modify .text 
		"DT_JMPREL": 23		, #  Address of PLT relocs 
		"DT_BIND_NOW": 24		, #  Process relocations of object 
		"DT_INIT_ARRAY": 25		, #  Array with addresses of init fct 
		"DT_FINI_ARRAY": 26		, #  Array with addresses of fini fct 
		"DT_INIT_ARRAYSZ": 27		, #  Size in bytes of DT_INIT_ARRAY 
		"DT_FINI_ARRAYSZ": 28		, #  Size in bytes of DT_FINI_ARRAY 
		"DT_RUNPATH": 29		, #  Library search path 
		"DT_FLAGS": 30		, #  Flags for the object being loaded 
		"DT_ENCODING": 32		, #  Start of encoded range 
		"DT_PREINIT_ARRAY": 32		, #  Array with addresses of preinit fct
		"DT_PREINIT_ARRAYSZ": 33		, #  size in bytes of DT_PREINIT_ARRAY 
		"DT_NUM": 34		, #  Number used 
		"DT_LOOS": 0x6000000d	, #  Start of OS-specific 
		"DT_HIOS": 0x6ffff000	, #  End of OS-specific 
		"DT_LOPROC": 0x70000000	, #  Start of processor-specific 
		"DT_HIPROC": 0x7fffffff	, #  End of processor-specific 
		"DT_PROCNUM": MacroRef("DT_MIPS_NUM")	, #  Most used by any processor 
		"DT_VALRNGLO": 0x6ffffd00	, # 
		"DT_GNU_PRELINKED": 0x6ffffdf5	, #  Prelinking timestamp 
		"DT_GNU_CONFLICTSZ": 0x6ffffdf6	, #  Size of conflict section 
		"DT_GNU_LIBLISTSZ": 0x6ffffdf7	, #  Size of library list 
		"DT_CHECKSUM": 0x6ffffdf8	, # 
		"DT_PLTPADSZ": 0x6ffffdf9	, # 
		"DT_MOVEENT": 0x6ffffdfa	, # 
		"DT_MOVESZ": 0x6ffffdfb	, # 
		"DT_FEATURE_1": 0x6ffffdfc	, #  Feature selection (DTF_*).  
		"DT_POSFLAG_1": 0x6ffffdfd	, #  Flags for DT_* entries, effecting
		"DT_SYMINSZ": 0x6ffffdfe	, #  Size of syminfo table (in bytes) 
		"DT_SYMINENT": 0x6ffffdff	, #  Entry size of syminfo 
		"DT_VALRNGHI": 0x6ffffdff	, # 
		"DT_VALNUM": 12	, # 
		"DT_ADDRRNGLO": 0x6ffffe00	, # 
		"DT_GNU_HASH": 0x6ffffef5	, #  GNU-style hash table.  
		"DT_TLSDESC_PLT": 0x6ffffef6	, # 
		"DT_TLSDESC_GOT": 0x6ffffef7	, # 
		"DT_GNU_CONFLICT": 0x6ffffef8	, #  Start of conflict section 
		"DT_GNU_LIBLIST": 0x6ffffef9	, #  Library list 
		"DT_CONFIG": 0x6ffffefa	, #  Configuration information.  
		"DT_DEPAUDIT": 0x6ffffefb	, #  Dependency auditing.  
		"DT_AUDIT": 0x6ffffefc	, #  Object auditing.  
		"DT_PLTPAD": 0x6ffffefd	, #  PLT padding.  
		"DT_MOVETAB": 0x6ffffefe	, #  Move table.  
		"DT_SYMINFO": 0x6ffffeff	, #  Syminfo table.  
		"DT_ADDRRNGHI": 0x6ffffeff	, # 
		"DT_ADDRNUM": 11	, # 
		"DT_VERSYM": 0x6ffffff0	, # 
		"DT_RELACOUNT": 0x6ffffff9	, # 
		"DT_RELCOUNT": 0x6ffffffa	, # 
		"DT_FLAGS_1": 0x6ffffffb	, #  State flags, see DF_1_* below.  
		"DT_VERDEF": 0x6ffffffc	, #  Address of version definition
		"DT_VERDEFNUM": 0x6ffffffd	, #  Number of version definitions 
		"DT_VERNEED": 0x6ffffffe	, #  Address of table with needed
		"DT_VERNEEDNUM": 0x6fffffff	, #  Number of needed versions 
		"DT_VERSIONTAGNUM": 16	, # 
		"DT_AUXILIARY": 0x7ffffffd      , #  Shared object to load before self 
		"DT_FILTER": 0x7fffffff      , #  Shared object to get values from 
		"DT_EXTRANUM": 3	, # 
		"DF_ORIGIN": 0x00000001	, #  Object may use DF_ORIGIN 
		"DF_SYMBOLIC": 0x00000002	, #  Symbol resolutions starts here 
		"DF_TEXTREL": 0x00000004	, #  Object contains text relocations 
		"DF_BIND_NOW": 0x00000008	, #  No lazy binding for this object 
		"DF_STATIC_TLS": 0x00000010	, #  Module uses the static TLS model 
		"DF_1_NOW": 0x00000001	, #  Set RTLD_NOW for this object.  
		"DF_1_GLOBAL": 0x00000002	, #  Set RTLD_GLOBAL for this object.  
		"DF_1_GROUP": 0x00000004	, #  Set RTLD_GROUP for this object.  
		"DF_1_NODELETE": 0x00000008	, #  Set RTLD_NODELETE for this object.
		"DF_1_LOADFLTR": 0x00000010	, #  Trigger filtee loading at runtime.
		"DF_1_INITFIRST": 0x00000020	, #  Set RTLD_INITFIRST for this object
		"DF_1_NOOPEN": 0x00000040	, #  Set RTLD_NOOPEN for this object.  
		"DF_1_ORIGIN": 0x00000080	, #  $ORIGIN must be handled.  
		"DF_1_DIRECT": 0x00000100	, #  Direct binding enabled.  
		"DF_1_TRANS": 0x00000200	, # 
		"DF_1_INTERPOSE": 0x00000400	, #  Object is used to interpose.  
		"DF_1_NODEFLIB": 0x00000800	, #  Ignore default lib search path.  
		"DF_1_NODUMP": 0x00001000	, #  Object can't be dldump'ed.  
		"DF_1_CONFALT": 0x00002000	, #  Configuration alternative created.
		"DF_1_ENDFILTEE": 0x00004000	, #  Filtee terminates filters search. 
		"DF_1_DISPRELDNE": 0x00008000	, #  Disp reloc applied at build time. 
		"DF_1_DISPRELPND": 0x00010000	, #  Disp reloc applied at run-time.  
		"DTF_1_PARINIT": 0x00000001	, # 
		"DTF_1_CONFEXP": 0x00000002	, # 
		"DF_P1_LAZYLOAD": 0x00000001	, #  Lazyload following object.  
		"DF_P1_GROUPPERM": 0x00000002	, #  Symbols from next object are not
		"VER_DEF_NONE": 0		, #  No version 
		"VER_DEF_CURRENT": 1		, #  Current version 
		"VER_DEF_NUM": 2		, #  Given version number 
		"VER_FLG_BASE": 0x1		, #  Version definition of file itself 
		"VER_FLG_WEAK": 0x2		, #  Weak version identifier 
		"VER_NDX_LOCAL": 0	, #  Symbol is local.  
		"VER_NDX_GLOBAL": 1	, #  Symbol is global.  
		"VER_NDX_LORESERVE": 0xff00	, #  Beginning of reserved entries.  
		"VER_NDX_ELIMINATE": 0xff01	, #  Symbol is to be eliminated.  
		"VER_NEED_NONE": 0		, #  No version 
		"VER_NEED_CURRENT": 1		, #  Current version 
		"VER_NEED_NUM": 2		, #  Given version number 
		"VER_FLG_WEAK": 0x2		, #  Weak version identifier 
		"AT_NULL": 0		, #  End of vector 
		"AT_IGNORE": 1		, #  Entry should be ignored 
		"AT_EXECFD": 2		, #  File descriptor of program 
		"AT_PHDR": 3		, #  Program headers for program 
		"AT_PHENT": 4		, #  Size of program header entry 
		"AT_PHNUM": 5		, #  Number of program headers 
		"AT_PAGESZ": 6		, #  System page size 
		"AT_BASE": 7		, #  Base address of interpreter 
		"AT_FLAGS": 8		, #  Flags 
		"AT_ENTRY": 9		, #  Entry point of program 
		"AT_NOTELF": 10		, #  Program is not ELF 
		"AT_UID": 11		, #  Real uid 
		"AT_EUID": 12		, #  Effective uid 
		"AT_GID": 13		, #  Real gid 
		"AT_EGID": 14		, #  Effective gid 
		"AT_CLKTCK": 17		, #  Frequency of times() 
		"AT_PLATFORM": 15		, #  String identifying platform.  
		"AT_HWCAP": 16		, #  Machine dependent hints about
		"AT_FPUCW": 18		, #  Used FPU control word.  
		"AT_DCACHEBSIZE": 19		, #  Data cache block size.  
		"AT_ICACHEBSIZE": 20		, #  Instruction cache block size.  
		"AT_UCACHEBSIZE": 21		, #  Unified cache block size.  
		"AT_IGNOREPPC": 22		, #  Entry should be ignored.  
		"AT_SECURE": 23		, #  Boolean, was exec setuid-like?  
		"AT_BASE_PLATFORM": 24		, #  String identifying real platforms.
		"AT_RANDOM": 25		, #  Address of 16 random bytes.  
		"AT_EXECFN": 31		, #  Filename of executable.  
		"AT_SYSINFO": 32	, # 
		"AT_SYSINFO_EHDR": 33	, # 
		"AT_L1I_CACHESHAPE": 34	, # 
		"AT_L1D_CACHESHAPE": 35	, # 
		"AT_L2_CACHESHAPE": 36	, # 
		"AT_L3_CACHESHAPE": 37	, # 
		"ELF_NOTE_SOLARIS": "SUNW Solaris"	, # 
		"ELF_NOTE_GNU": "GNU"	, # 
		"ELF_NOTE_PAGESIZE_HINT": 1	, # 
		"NT_GNU_ABI_TAG": 1	, # 
		"ELF_NOTE_ABI": MacroRef("NT_GNU_ABI_TAG") , #  Old name.  
		"ELF_NOTE_OS_LINUX": 0	, # 
		"ELF_NOTE_OS_GNU": 1	, # 
		"ELF_NOTE_OS_SOLARIS2": 2	, # 
		"ELF_NOTE_OS_FREEBSD": 3	, # 
		"NT_GNU_HWCAP": 2	, # 
		"NT_GNU_BUILD_ID": 3	, # 
		"NT_GNU_GOLD_VERSION": 4	, # 
		"EF_CPU32": 0x00810000	, # 
		"R_68K_NONE": 0		, #  No reloc 
		"R_68K_32": 1		, #  Direct 32 bit  
		"R_68K_16": 2		, #  Direct 16 bit  
		"R_68K_8": 3		, #  Direct 8 bit  
		"R_68K_PC32": 4		, #  PC relative 32 bit 
		"R_68K_PC16": 5		, #  PC relative 16 bit 
		"R_68K_PC8": 6		, #  PC relative 8 bit 
		"R_68K_GOT32": 7		, #  32 bit PC relative GOT entry 
		"R_68K_GOT16": 8		, #  16 bit PC relative GOT entry 
		"R_68K_GOT8": 9		, #  8 bit PC relative GOT entry 
		"R_68K_GOT32O": 10		, #  32 bit GOT offset 
		"R_68K_GOT16O": 11		, #  16 bit GOT offset 
		"R_68K_GOT8O": 12		, #  8 bit GOT offset 
		"R_68K_PLT32": 13		, #  32 bit PC relative PLT address 
		"R_68K_PLT16": 14		, #  16 bit PC relative PLT address 
		"R_68K_PLT8": 15		, #  8 bit PC relative PLT address 
		"R_68K_PLT32O": 16		, #  32 bit PLT offset 
		"R_68K_PLT16O": 17		, #  16 bit PLT offset 
		"R_68K_PLT8O": 18		, #  8 bit PLT offset 
		"R_68K_COPY": 19		, #  Copy symbol at runtime 
		"R_68K_GLOB_DAT": 20		, #  Create GOT entry 
		"R_68K_JMP_SLOT": 21		, #  Create PLT entry 
		"R_68K_RELATIVE": 22		, #  Adjust by program base 
		"R_68K_TLS_GD32": 25          , #  32 bit GOT offset for GD 
		"R_68K_TLS_GD16": 26          , #  16 bit GOT offset for GD 
		"R_68K_TLS_GD8": 27          , #  8 bit GOT offset for GD 
		"R_68K_TLS_LDM32": 28          , #  32 bit GOT offset for LDM 
		"R_68K_TLS_LDM16": 29          , #  16 bit GOT offset for LDM 
		"R_68K_TLS_LDM8": 30          , #  8 bit GOT offset for LDM 
		"R_68K_TLS_LDO32": 31          , #  32 bit module-relative offset 
		"R_68K_TLS_LDO16": 32          , #  16 bit module-relative offset 
		"R_68K_TLS_LDO8": 33          , #  8 bit module-relative offset 
		"R_68K_TLS_IE32": 34          , #  32 bit GOT offset for IE 
		"R_68K_TLS_IE16": 35          , #  16 bit GOT offset for IE 
		"R_68K_TLS_IE8": 36          , #  8 bit GOT offset for IE 
		"R_68K_TLS_LE32": 37          , #  32 bit offset relative to
		"R_68K_TLS_LE16": 38          , #  16 bit offset relative to
		"R_68K_TLS_LE8": 39          , #  8 bit offset relative to
		"R_68K_TLS_DTPMOD32": 40          , #  32 bit module number 
		"R_68K_TLS_DTPREL32": 41          , #  32 bit module-relative offset 
		"R_68K_TLS_TPREL32": 42          , #  32 bit TP-relative offset 
		"R_68K_NUM": 43	, # 
		"R_386_NONE": 0		, #  No reloc 
		"R_386_32": 1		, #  Direct 32 bit  
		"R_386_PC32": 2		, #  PC relative 32 bit 
		"R_386_GOT32": 3		, #  32 bit GOT entry 
		"R_386_PLT32": 4		, #  32 bit PLT address 
		"R_386_COPY": 5		, #  Copy symbol at runtime 
		"R_386_GLOB_DAT": 6		, #  Create GOT entry 
		"R_386_JMP_SLOT": 7		, #  Create PLT entry 
		"R_386_RELATIVE": 8		, #  Adjust by program base 
		"R_386_GOTOFF": 9		, #  32 bit offset to GOT 
		"R_386_GOTPC": 10		, #  32 bit PC relative offset to GOT 
		"R_386_32PLT": 11	, # 
		"R_386_TLS_TPOFF": 14		, #  Offset in static TLS block 
		"R_386_TLS_IE": 15		, #  Address of GOT entry for static TLS
		"R_386_TLS_GOTIE": 16		, #  GOT entry for static TLS block
		"R_386_TLS_LE": 17		, #  Offset relative to static TLS
		"R_386_TLS_GD": 18		, #  Direct 32 bit for GNU version of
		"R_386_TLS_LDM": 19		, #  Direct 32 bit for GNU version of
		"R_386_16": 20	, # 
		"R_386_PC16": 21	, # 
		"R_386_8": 22	, # 
		"R_386_PC8": 23	, # 
		"R_386_TLS_GD_32": 24		, #  Direct 32 bit for general dynamic
		"R_386_TLS_GD_PUSH": 25		, #  Tag for pushl in GD TLS code 
		"R_386_TLS_GD_CALL": 26		, #  Relocation for call to
		"R_386_TLS_GD_POP": 27		, #  Tag for popl in GD TLS code 
		"R_386_TLS_LDM_32": 28		, #  Direct 32 bit for local dynamic
		"R_386_TLS_LDM_PUSH": 29		, #  Tag for pushl in LDM TLS code 
		"R_386_TLS_LDM_CALL": 30		, #  Relocation for call to
		"R_386_TLS_LDM_POP": 31		, #  Tag for popl in LDM TLS code 
		"R_386_TLS_LDO_32": 32		, #  Offset relative to TLS block 
		"R_386_TLS_IE_32": 33		, #  GOT entry for negated static TLS
		"R_386_TLS_LE_32": 34		, #  Negated offset relative to static
		"R_386_TLS_DTPMOD32": 35		, #  ID of module containing symbol 
		"R_386_TLS_DTPOFF32": 36		, #  Offset in TLS block 
		"R_386_TLS_TPOFF32": 37		, #  Negated offset in static TLS block 
		"R_386_TLS_GOTDESC": 39		, #  GOT offset for TLS descriptor.  
		"R_386_TLS_DESC_CALL": 40		, #  Marker of call through TLS
		"R_386_TLS_DESC": 41		, #  TLS descriptor containing
		"R_386_IRELATIVE": 42		, #  Adjust indirectly by program base 
		"R_386_NUM": 43	, # 
		"STT_SPARC_REGISTER": 13	, #  Global register reserved to app. 
		"EF_SPARCV9_MM": 3	, # 
		"EF_SPARCV9_TSO": 0	, # 
		"EF_SPARCV9_PSO": 1	, # 
		"EF_SPARCV9_RMO": 2	, # 
		"EF_SPARC_LEDATA": 0x800000 , #  little endian data 
		"EF_SPARC_EXT_MASK": 0xFFFF00	, # 
		"EF_SPARC_32PLUS": 0x000100 , #  generic V8+ features 
		"EF_SPARC_SUN_US1": 0x000200 , #  Sun UltraSPARC1 extensions 
		"EF_SPARC_HAL_R1": 0x000400 , #  HAL R1 extensions 
		"EF_SPARC_SUN_US3": 0x000800 , #  Sun UltraSPARCIII extensions 
		"R_SPARC_NONE": 0	, #  No reloc 
		"R_SPARC_8": 1	, #  Direct 8 bit 
		"R_SPARC_16": 2	, #  Direct 16 bit 
		"R_SPARC_32": 3	, #  Direct 32 bit 
		"R_SPARC_DISP8": 4	, #  PC relative 8 bit 
		"R_SPARC_DISP16": 5	, #  PC relative 16 bit 
		"R_SPARC_DISP32": 6	, #  PC relative 32 bit 
		"R_SPARC_WDISP30": 7	, #  PC relative 30 bit shifted 
		"R_SPARC_WDISP22": 8	, #  PC relative 22 bit shifted 
		"R_SPARC_HI22": 9	, #  High 22 bit 
		"R_SPARC_22": 10	, #  Direct 22 bit 
		"R_SPARC_13": 11	, #  Direct 13 bit 
		"R_SPARC_LO10": 12	, #  Truncated 10 bit 
		"R_SPARC_GOT10": 13	, #  Truncated 10 bit GOT entry 
		"R_SPARC_GOT13": 14	, #  13 bit GOT entry 
		"R_SPARC_GOT22": 15	, #  22 bit GOT entry shifted 
		"R_SPARC_PC10": 16	, #  PC relative 10 bit truncated 
		"R_SPARC_PC22": 17	, #  PC relative 22 bit shifted 
		"R_SPARC_WPLT30": 18	, #  30 bit PC relative PLT address 
		"R_SPARC_COPY": 19	, #  Copy symbol at runtime 
		"R_SPARC_GLOB_DAT": 20	, #  Create GOT entry 
		"R_SPARC_JMP_SLOT": 21	, #  Create PLT entry 
		"R_SPARC_RELATIVE": 22	, #  Adjust by program base 
		"R_SPARC_UA32": 23	, #  Direct 32 bit unaligned 
		"R_SPARC_PLT32": 24	, #  Direct 32 bit ref to PLT entry 
		"R_SPARC_HIPLT22": 25	, #  High 22 bit PLT entry 
		"R_SPARC_LOPLT10": 26	, #  Truncated 10 bit PLT entry 
		"R_SPARC_PCPLT32": 27	, #  PC rel 32 bit ref to PLT entry 
		"R_SPARC_PCPLT22": 28	, #  PC rel high 22 bit PLT entry 
		"R_SPARC_PCPLT10": 29	, #  PC rel trunc 10 bit PLT entry 
		"R_SPARC_10": 30	, #  Direct 10 bit 
		"R_SPARC_11": 31	, #  Direct 11 bit 
		"R_SPARC_64": 32	, #  Direct 64 bit 
		"R_SPARC_OLO10": 33	, #  10bit with secondary 13bit addend 
		"R_SPARC_HH22": 34	, #  Top 22 bits of direct 64 bit 
		"R_SPARC_HM10": 35	, #  High middle 10 bits of ... 
		"R_SPARC_LM22": 36	, #  Low middle 22 bits of ... 
		"R_SPARC_PC_HH22": 37	, #  Top 22 bits of pc rel 64 bit 
		"R_SPARC_PC_HM10": 38	, #  High middle 10 bit of ... 
		"R_SPARC_PC_LM22": 39	, #  Low miggle 22 bits of ... 
		"R_SPARC_WDISP16": 40	, #  PC relative 16 bit shifted 
		"R_SPARC_WDISP19": 41	, #  PC relative 19 bit shifted 
		"R_SPARC_GLOB_JMP": 42	, #  was part of v9 ABI but was removed 
		"R_SPARC_7": 43	, #  Direct 7 bit 
		"R_SPARC_5": 44	, #  Direct 5 bit 
		"R_SPARC_6": 45	, #  Direct 6 bit 
		"R_SPARC_DISP64": 46	, #  PC relative 64 bit 
		"R_SPARC_PLT64": 47	, #  Direct 64 bit ref to PLT entry 
		"R_SPARC_HIX22": 48	, #  High 22 bit complemented 
		"R_SPARC_LOX10": 49	, #  Truncated 11 bit complemented 
		"R_SPARC_H44": 50	, #  Direct high 12 of 44 bit 
		"R_SPARC_M44": 51	, #  Direct mid 22 of 44 bit 
		"R_SPARC_L44": 52	, #  Direct low 10 of 44 bit 
		"R_SPARC_REGISTER": 53	, #  Global register usage 
		"R_SPARC_UA64": 54	, #  Direct 64 bit unaligned 
		"R_SPARC_UA16": 55	, #  Direct 16 bit unaligned 
		"R_SPARC_TLS_GD_HI22": 56	, # 
		"R_SPARC_TLS_GD_LO10": 57	, # 
		"R_SPARC_TLS_GD_ADD": 58	, # 
		"R_SPARC_TLS_GD_CALL": 59	, # 
		"R_SPARC_TLS_LDM_HI22": 60	, # 
		"R_SPARC_TLS_LDM_LO10": 61	, # 
		"R_SPARC_TLS_LDM_ADD": 62	, # 
		"R_SPARC_TLS_LDM_CALL": 63	, # 
		"R_SPARC_TLS_LDO_HIX22": 64	, # 
		"R_SPARC_TLS_LDO_LOX10": 65	, # 
		"R_SPARC_TLS_LDO_ADD": 66	, # 
		"R_SPARC_TLS_IE_HI22": 67	, # 
		"R_SPARC_TLS_IE_LO10": 68	, # 
		"R_SPARC_TLS_IE_LD": 69	, # 
		"R_SPARC_TLS_IE_LDX": 70	, # 
		"R_SPARC_TLS_IE_ADD": 71	, # 
		"R_SPARC_TLS_LE_HIX22": 72	, # 
		"R_SPARC_TLS_LE_LOX10": 73	, # 
		"R_SPARC_TLS_DTPMOD32": 74	, # 
		"R_SPARC_TLS_DTPMOD64": 75	, # 
		"R_SPARC_TLS_DTPOFF32": 76	, # 
		"R_SPARC_TLS_DTPOFF64": 77	, # 
		"R_SPARC_TLS_TPOFF32": 78	, # 
		"R_SPARC_TLS_TPOFF64": 79	, # 
		"R_SPARC_GOTDATA_HIX22": 80	, # 
		"R_SPARC_GOTDATA_LOX10": 81	, # 
		"R_SPARC_GOTDATA_OP_HIX22": 82	, # 
		"R_SPARC_GOTDATA_OP_LOX10": 83	, # 
		"R_SPARC_GOTDATA_OP": 84	, # 
		"R_SPARC_H34": 85	, # 
		"R_SPARC_SIZE32": 86	, # 
		"R_SPARC_SIZE64": 87	, # 
		"R_SPARC_JMP_IREL": 248	, # 
		"R_SPARC_IRELATIVE": 249	, # 
		"R_SPARC_GNU_VTINHERIT": 250	, # 
		"R_SPARC_GNU_VTENTRY": 251	, # 
		"R_SPARC_REV32": 252	, # 
		"R_SPARC_NUM": 253	, # 
		"DT_SPARC_REGISTER": 0x70000001	, # 
		"DT_SPARC_NUM": 2	, # 
		"EF_MIPS_NOREORDER": 1		, #  A .noreorder directive was used 
		"EF_MIPS_PIC": 2		, #  Contains PIC code 
		"EF_MIPS_CPIC": 4		, #  Uses PIC calling sequence 
		"EF_MIPS_XGOT": 8	, # 
		"EF_MIPS_64BIT_WHIRL": 16	, # 
		"EF_MIPS_ABI2": 32	, # 
		"EF_MIPS_ABI_ON32": 64	, # 
		"EF_MIPS_ARCH": 0xf0000000	, #  MIPS architecture level 
		"EF_MIPS_ARCH_1": 0x00000000	, #  -mips1 code.  
		"EF_MIPS_ARCH_2": 0x10000000	, #  -mips2 code.  
		"EF_MIPS_ARCH_3": 0x20000000	, #  -mips3 code.  
		"EF_MIPS_ARCH_4": 0x30000000	, #  -mips4 code.  
		"EF_MIPS_ARCH_5": 0x40000000	, #  -mips5 code.  
		"EF_MIPS_ARCH_32": 0x60000000	, #  MIPS32 code.  
		"EF_MIPS_ARCH_64": 0x70000000	, #  MIPS64 code.  
		"E_MIPS_ARCH_1": 0x00000000	, #  -mips1 code.  
		"E_MIPS_ARCH_2": 0x10000000	, #  -mips2 code.  
		"E_MIPS_ARCH_3": 0x20000000	, #  -mips3 code.  
		"E_MIPS_ARCH_4": 0x30000000	, #  -mips4 code.  
		"E_MIPS_ARCH_5": 0x40000000	, #  -mips5 code.  
		"E_MIPS_ARCH_32": 0x60000000	, #  MIPS32 code.  
		"E_MIPS_ARCH_64": 0x70000000	, #  MIPS64 code.  
		"SHN_MIPS_ACOMMON": 0xff00	, #  Allocated common symbols 
		"SHN_MIPS_TEXT": 0xff01	, #  Allocated test symbols.  
		"SHN_MIPS_DATA": 0xff02	, #  Allocated data symbols.  
		"SHN_MIPS_SCOMMON": 0xff03	, #  Small common symbols 
		"SHN_MIPS_SUNDEFINED": 0xff04	, #  Small undefined symbols 
		"SHT_MIPS_LIBLIST": 0x70000000 , #  Shared objects used in link 
		"SHT_MIPS_MSYM": 0x70000001	, # 
		"SHT_MIPS_CONFLICT": 0x70000002 , #  Conflicting symbols 
		"SHT_MIPS_GPTAB": 0x70000003 , #  Global data area sizes 
		"SHT_MIPS_UCODE": 0x70000004 , #  Reserved for SGI/MIPS compilers 
		"SHT_MIPS_DEBUG": 0x70000005 , #  MIPS ECOFF debugging information
		"SHT_MIPS_REGINFO": 0x70000006 , #  Register usage information 
		"SHT_MIPS_PACKAGE": 0x70000007	, # 
		"SHT_MIPS_PACKSYM": 0x70000008	, # 
		"SHT_MIPS_RELD": 0x70000009	, # 
		"SHT_MIPS_IFACE": 0x7000000b	, # 
		"SHT_MIPS_CONTENT": 0x7000000c	, # 
		"SHT_MIPS_OPTIONS": 0x7000000d , #  Miscellaneous options.  
		"SHT_MIPS_SHDR": 0x70000010	, # 
		"SHT_MIPS_FDESC": 0x70000011	, # 
		"SHT_MIPS_EXTSYM": 0x70000012	, # 
		"SHT_MIPS_DENSE": 0x70000013	, # 
		"SHT_MIPS_PDESC": 0x70000014	, # 
		"SHT_MIPS_LOCSYM": 0x70000015	, # 
		"SHT_MIPS_AUXSYM": 0x70000016	, # 
		"SHT_MIPS_OPTSYM": 0x70000017	, # 
		"SHT_MIPS_LOCSTR": 0x70000018	, # 
		"SHT_MIPS_LINE": 0x70000019	, # 
		"SHT_MIPS_RFDESC": 0x7000001a	, # 
		"SHT_MIPS_DELTASYM": 0x7000001b	, # 
		"SHT_MIPS_DELTAINST": 0x7000001c	, # 
		"SHT_MIPS_DELTACLASS": 0x7000001d	, # 
		"SHT_MIPS_DWARF": 0x7000001e , #  DWARF debugging information.  
		"SHT_MIPS_DELTADECL": 0x7000001f	, # 
		"SHT_MIPS_SYMBOL_LIB": 0x70000020	, # 
		"SHT_MIPS_EVENTS": 0x70000021 , #  Event section.  
		"SHT_MIPS_TRANSLATE": 0x70000022	, # 
		"SHT_MIPS_PIXIE": 0x70000023	, # 
		"SHT_MIPS_XLATE": 0x70000024	, # 
		"SHT_MIPS_XLATE_DEBUG": 0x70000025	, # 
		"SHT_MIPS_WHIRL": 0x70000026	, # 
		"SHT_MIPS_EH_REGION": 0x70000027	, # 
		"SHT_MIPS_XLATE_OLD": 0x70000028	, # 
		"SHT_MIPS_PDR_EXCEPTION": 0x70000029	, # 
		"SHF_MIPS_GPREL": 0x10000000	, #  Must be part of global data area 
		"SHF_MIPS_MERGE": 0x20000000	, # 
		"SHF_MIPS_ADDR": 0x40000000	, # 
		"SHF_MIPS_STRINGS": 0x80000000	, # 
		"SHF_MIPS_NOSTRIP": 0x08000000	, # 
		"SHF_MIPS_LOCAL": 0x04000000	, # 
		"SHF_MIPS_NAMES": 0x02000000	, # 
		"SHF_MIPS_NODUPE": 0x01000000	, # 
		"STO_MIPS_DEFAULT": 0x0	, # 
		"STO_MIPS_INTERNAL": 0x1	, # 
		"STO_MIPS_HIDDEN": 0x2	, # 
		"STO_MIPS_PROTECTED": 0x3	, # 
		"STO_MIPS_PLT": 0x8	, # 
		"STO_MIPS_SC_ALIGN_UNUSED": 0xff	, # 
		"STB_MIPS_SPLIT_COMMON": 13	, # 
		"ODK_NULL": 0	, #  Undefined.  
		"ODK_REGINFO": 1	, #  Register usage information.  
		"ODK_EXCEPTIONS": 2	, #  Exception processing options.  
		"ODK_PAD": 3	, #  Section padding options.  
		"ODK_HWPATCH": 4	, #  Hardware workarounds performed 
		"ODK_FILL": 5	, #  record the fill value used by the linker. 
		"ODK_TAGS": 6	, #  reserve space for desktop tools to write. 
		"ODK_HWAND": 7	, #  HW workarounds.  'AND' bits when merging. 
		"ODK_HWOR": 8	, #  HW workarounds.  'OR' bits when merging.  
		"OEX_FPU_MIN": 0x1f	, #  FPE's which MUST be enabled.  
		"OEX_FPU_MAX": 0x1f00	, #  FPE's which MAY be enabled.  
		"OEX_PAGE0": 0x10000	, #  page zero must be mapped.  
		"OEX_SMM": 0x20000	, #  Force sequential memory mode?  
		"OEX_FPDBUG": 0x40000	, #  Force floating point debug mode?  
		"OEX_PRECISEFP": MacroRef("OEX_FPDBUG")	, # 
		"OEX_DISMISS": 0x80000	, #  Dismiss invalid address faults?  
		"OEX_FPU_INVAL": 0x10	, # 
		"OEX_FPU_DIV0": 0x08	, # 
		"OEX_FPU_OFLO": 0x04	, # 
		"OEX_FPU_UFLO": 0x02	, # 
		"OEX_FPU_INEX": 0x01	, # 
		"OHW_R4KEOP": 0x1	, #  R4000 end-of-page patch.  
		"OHW_R8KPFETCH": 0x2	, #  may need R8000 prefetch patch.  
		"OHW_R5KEOP": 0x4	, #  R5000 end-of-page patch.  
		"OHW_R5KCVTL": 0x8	, #  R5000 cvt.[ds].l bug.  clean=1.  
		"OPAD_PREFIX": 0x1	, # 
		"OPAD_POSTFIX": 0x2	, # 
		"OPAD_SYMBOL": 0x4	, # 
		"OHWA0_R4KEOP_CHECKED": 0x00000001	, # 
		"OHWA1_R4KEOP_CLEAN": 0x00000002	, # 
		"R_MIPS_NONE": 0	, #  No reloc 
		"R_MIPS_16": 1	, #  Direct 16 bit 
		"R_MIPS_32": 2	, #  Direct 32 bit 
		"R_MIPS_REL32": 3	, #  PC relative 32 bit 
		"R_MIPS_26": 4	, #  Direct 26 bit shifted 
		"R_MIPS_HI16": 5	, #  High 16 bit 
		"R_MIPS_LO16": 6	, #  Low 16 bit 
		"R_MIPS_GPREL16": 7	, #  GP relative 16 bit 
		"R_MIPS_LITERAL": 8	, #  16 bit literal entry 
		"R_MIPS_GOT16": 9	, #  16 bit GOT entry 
		"R_MIPS_PC16": 10	, #  PC relative 16 bit 
		"R_MIPS_CALL16": 11	, #  16 bit GOT entry for function 
		"R_MIPS_GPREL32": 12	, #  GP relative 32 bit 
		"R_MIPS_SHIFT5": 16	, # 
		"R_MIPS_SHIFT6": 17	, # 
		"R_MIPS_64": 18	, # 
		"R_MIPS_GOT_DISP": 19	, # 
		"R_MIPS_GOT_PAGE": 20	, # 
		"R_MIPS_GOT_OFST": 21	, # 
		"R_MIPS_GOT_HI16": 22	, # 
		"R_MIPS_GOT_LO16": 23	, # 
		"R_MIPS_SUB": 24	, # 
		"R_MIPS_INSERT_A": 25	, # 
		"R_MIPS_INSERT_B": 26	, # 
		"R_MIPS_DELETE": 27	, # 
		"R_MIPS_HIGHER": 28	, # 
		"R_MIPS_HIGHEST": 29	, # 
		"R_MIPS_CALL_HI16": 30	, # 
		"R_MIPS_CALL_LO16": 31	, # 
		"R_MIPS_SCN_DISP": 32	, # 
		"R_MIPS_REL16": 33	, # 
		"R_MIPS_ADD_IMMEDIATE": 34	, # 
		"R_MIPS_PJUMP": 35	, # 
		"R_MIPS_RELGOT": 36	, # 
		"R_MIPS_JALR": 37	, # 
		"R_MIPS_TLS_DTPMOD32": 38	, #  Module number 32 bit 
		"R_MIPS_TLS_DTPREL32": 39	, #  Module-relative offset 32 bit 
		"R_MIPS_TLS_DTPMOD64": 40	, #  Module number 64 bit 
		"R_MIPS_TLS_DTPREL64": 41	, #  Module-relative offset 64 bit 
		"R_MIPS_TLS_GD": 42	, #  16 bit GOT offset for GD 
		"R_MIPS_TLS_LDM": 43	, #  16 bit GOT offset for LDM 
		"R_MIPS_TLS_DTPREL_HI16": 44	, #  Module-relative offset, high 16 bits 
		"R_MIPS_TLS_DTPREL_LO16": 45	, #  Module-relative offset, low 16 bits 
		"R_MIPS_TLS_GOTTPREL": 46	, #  16 bit GOT offset for IE 
		"R_MIPS_TLS_TPREL32": 47	, #  TP-relative offset, 32 bit 
		"R_MIPS_TLS_TPREL64": 48	, #  TP-relative offset, 64 bit 
		"R_MIPS_TLS_TPREL_HI16": 49	, #  TP-relative offset, high 16 bits 
		"R_MIPS_TLS_TPREL_LO16": 50	, #  TP-relative offset, low 16 bits 
		"R_MIPS_GLOB_DAT": 51	, # 
		"R_MIPS_COPY": 126	, # 
		"R_MIPS_JUMP_SLOT": 127	, # 
		"R_MIPS_NUM": 128	, # 
		"PT_MIPS_REGINFO": 0x70000000	, #  Register usage information 
		"PT_MIPS_RTPROC": 0x70000001	, #  Runtime procedure table. 
		"PT_MIPS_OPTIONS": 0x70000002	, # 
		"PF_MIPS_LOCAL": 0x10000000	, # 
		"DT_MIPS_RLD_VERSION": 0x70000001	, #  Runtime linker interface version 
		"DT_MIPS_TIME_STAMP": 0x70000002	, #  Timestamp 
		"DT_MIPS_ICHECKSUM": 0x70000003	, #  Checksum 
		"DT_MIPS_IVERSION": 0x70000004	, #  Version string (string tbl index) 
		"DT_MIPS_FLAGS": 0x70000005	, #  Flags 
		"DT_MIPS_BASE_ADDRESS": 0x70000006	, #  Base address 
		"DT_MIPS_MSYM": 0x70000007	, # 
		"DT_MIPS_CONFLICT": 0x70000008	, #  Address of CONFLICT section 
		"DT_MIPS_LIBLIST": 0x70000009	, #  Address of LIBLIST section 
		"DT_MIPS_LOCAL_GOTNO": 0x7000000a	, #  Number of local GOT entries 
		"DT_MIPS_CONFLICTNO": 0x7000000b	, #  Number of CONFLICT entries 
		"DT_MIPS_LIBLISTNO": 0x70000010	, #  Number of LIBLIST entries 
		"DT_MIPS_SYMTABNO": 0x70000011	, #  Number of DYNSYM entries 
		"DT_MIPS_UNREFEXTNO": 0x70000012	, #  First external DYNSYM 
		"DT_MIPS_GOTSYM": 0x70000013	, #  First GOT entry in DYNSYM 
		"DT_MIPS_HIPAGENO": 0x70000014	, #  Number of GOT page table entries 
		"DT_MIPS_RLD_MAP": 0x70000016	, #  Address of run time loader map.  
		"DT_MIPS_DELTA_CLASS": 0x70000017	, #  Delta C++ class definition.  
		"DT_MIPS_DELTA_CLASS_NO": 0x70000018 , #  Number of entries in
		"DT_MIPS_DELTA_INSTANCE": 0x70000019 , #  Delta C++ class instances.  
		"DT_MIPS_DELTA_INSTANCE_NO": 0x7000001a , #  Number of entries in
		"DT_MIPS_DELTA_RELOC": 0x7000001b , #  Delta relocations.  
		"DT_MIPS_DELTA_RELOC_NO": 0x7000001c , #  Number of entries in
		"DT_MIPS_DELTA_SYM": 0x7000001d , #  Delta symbols that Delta
		"DT_MIPS_DELTA_SYM_NO": 0x7000001e , #  Number of entries in
		"DT_MIPS_DELTA_CLASSSYM": 0x70000020 , #  Delta symbols that hold the
		"DT_MIPS_DELTA_CLASSSYM_NO": 0x70000021 , #  Number of entries in
		"DT_MIPS_CXX_FLAGS": 0x70000022 , #  Flags indicating for C++ flavor.  
		"DT_MIPS_PIXIE_INIT": 0x70000023	, # 
		"DT_MIPS_SYMBOL_LIB": 0x70000024	, # 
		"DT_MIPS_LOCALPAGE_GOTIDX": 0x70000025	, # 
		"DT_MIPS_LOCAL_GOTIDX": 0x70000026	, # 
		"DT_MIPS_HIDDEN_GOTIDX": 0x70000027	, # 
		"DT_MIPS_PROTECTED_GOTIDX": 0x70000028	, # 
		"DT_MIPS_OPTIONS": 0x70000029 , #  Address of .options.  
		"DT_MIPS_INTERFACE": 0x7000002a , #  Address of .interface.  
		"DT_MIPS_DYNSTR_ALIGN": 0x7000002b	, # 
		"DT_MIPS_INTERFACE_SIZE": 0x7000002c , #  Size of the .interface section. 
		"DT_MIPS_RLD_TEXT_RESOLVE_ADDR": 0x7000002d , #  Address of rld_text_rsolve
		"DT_MIPS_PERF_SUFFIX": 0x7000002e , #  Default suffix of dso to be added
		"DT_MIPS_COMPACT_SIZE": 0x7000002f , #  (O32)Size of compact rel section. 
		"DT_MIPS_GP_VALUE": 0x70000030 , #  GP value for aux GOTs.  
		"DT_MIPS_AUX_DYNAMIC": 0x70000031 , #  Address of aux .dynamic.  
		"DT_MIPS_PLTGOT": 0x70000032	, # 
		"DT_MIPS_RWPLT": 0x70000034	, # 
		"DT_MIPS_NUM": 0x35	, # 
		"RHF_NONE": 0		, #  No flags 
		"RHF_QUICKSTART": (1 << 0)	, #  Use quickstart 
		"RHF_NOTPOT": (1 << 1)	, #  Hash size not power of 2 
		"RHF_NO_LIBRARY_REPLACEMENT": (1 << 2)	, #  Ignore LD_LIBRARY_PATH 
		"RHF_NO_MOVE": (1 << 3)	, # 
		"RHF_SGI_ONLY": (1 << 4)	, # 
		"RHF_GUARANTEE_INIT": (1 << 5)	, # 
		"RHF_DELTA_C_PLUS_PLUS": (1 << 6)	, # 
		"RHF_GUARANTEE_START_INIT": (1 << 7)	, # 
		"RHF_PIXIE": (1 << 8)	, # 
		"RHF_DEFAULT_DELAY_LOAD": (1 << 9)	, # 
		"RHF_REQUICKSTART": (1 << 10)	, # 
		"RHF_REQUICKSTARTED": (1 << 11)	, # 
		"RHF_CORD": (1 << 12)	, # 
		"RHF_NO_UNRES_UNDEF": (1 << 13)	, # 
		"RHF_RLD_ORDER_SAFE": (1 << 14)	, # 
		"LL_NONE": 0	, # 
		"LL_EXACT_MATCH": (1 << 0)	, #  Require exact match 
		"LL_IGNORE_INT_VER": (1 << 1)	, #  Ignore interface version 
		"LL_REQUIRE_MINOR": (1 << 2)	, # 
		"LL_EXPORTS": (1 << 3)	, # 
		"LL_DELAY_LOAD": (1 << 4)	, # 
		"LL_DELTA": (1 << 5)	, # 
		"EF_PARISC_TRAPNIL": 0x00010000 , #  Trap nil pointer dereference.  
		"EF_PARISC_EXT": 0x00020000 , #  Program uses arch. extensions. 
		"EF_PARISC_LSB": 0x00040000 , #  Program expects little endian. 
		"EF_PARISC_WIDE": 0x00080000 , #  Program expects wide mode.  
		"EF_PARISC_NO_KABP": 0x00100000 , #  No kernel assisted branch
		"EF_PARISC_LAZYSWAP": 0x00400000 , #  Allow lazy swapping.  
		"EF_PARISC_ARCH": 0x0000ffff , #  Architecture version.  
		"EFA_PARISC_1_0": 0x020b , #  PA-RISC 1.0 big-endian.  
		"EFA_PARISC_1_1": 0x0210 , #  PA-RISC 1.1 big-endian.  
		"EFA_PARISC_2_0": 0x0214 , #  PA-RISC 2.0 big-endian.  
		"SHN_PARISC_ANSI_COMMON": 0xff00	   , #  Section for tenatively declared
		"SHN_PARISC_HUGE_COMMON": 0xff01	   , #  Common blocks in huge model.  
		"SHT_PARISC_EXT": 0x70000000 , #  Contains product specific ext. 
		"SHT_PARISC_UNWIND": 0x70000001 , #  Unwind information.  
		"SHT_PARISC_DOC": 0x70000002 , #  Debug info for optimized code. 
		"SHF_PARISC_SHORT": 0x20000000 , #  Section with short addressing. 
		"SHF_PARISC_HUGE": 0x40000000 , #  Section far from gp.  
		"SHF_PARISC_SBP": 0x80000000 , #  Static branch prediction code. 
		"STT_PARISC_MILLICODE": 13	, #  Millicode function entry point.  
		"STT_HP_OPAQUE": (MacroRef("STT_LOOS") + 0x1)	, # 
		"STT_HP_STUB": (MacroRef("STT_LOOS") + 0x2)	, # 
		"R_PARISC_NONE": 0	, #  No reloc.  
		"R_PARISC_DIR32": 1	, #  Direct 32-bit reference.  
		"R_PARISC_DIR21L": 2	, #  Left 21 bits of eff. address.  
		"R_PARISC_DIR17R": 3	, #  Right 17 bits of eff. address.  
		"R_PARISC_DIR17F": 4	, #  17 bits of eff. address.  
		"R_PARISC_DIR14R": 6	, #  Right 14 bits of eff. address.  
		"R_PARISC_PCREL32": 9	, #  32-bit rel. address.  
		"R_PARISC_PCREL21L": 10	, #  Left 21 bits of rel. address.  
		"R_PARISC_PCREL17R": 11	, #  Right 17 bits of rel. address.  
		"R_PARISC_PCREL17F": 12	, #  17 bits of rel. address.  
		"R_PARISC_PCREL14R": 14	, #  Right 14 bits of rel. address.  
		"R_PARISC_DPREL21L": 18	, #  Left 21 bits of rel. address.  
		"R_PARISC_DPREL14R": 22	, #  Right 14 bits of rel. address.  
		"R_PARISC_GPREL21L": 26	, #  GP-relative, left 21 bits.  
		"R_PARISC_GPREL14R": 30	, #  GP-relative, right 14 bits.  
		"R_PARISC_LTOFF21L": 34	, #  LT-relative, left 21 bits.  
		"R_PARISC_LTOFF14R": 38	, #  LT-relative, right 14 bits.  
		"R_PARISC_SECREL32": 41	, #  32 bits section rel. address.  
		"R_PARISC_SEGBASE": 48	, #  No relocation, set segment base.  
		"R_PARISC_SEGREL32": 49	, #  32 bits segment rel. address.  
		"R_PARISC_PLTOFF21L": 50	, #  PLT rel. address, left 21 bits.  
		"R_PARISC_PLTOFF14R": 54	, #  PLT rel. address, right 14 bits.  
		"R_PARISC_LTOFF_FPTR32": 57	, #  32 bits LT-rel. function pointer. 
		"R_PARISC_LTOFF_FPTR21L": 58	, #  LT-rel. fct ptr, left 21 bits. 
		"R_PARISC_LTOFF_FPTR14R": 62	, #  LT-rel. fct ptr, right 14 bits. 
		"R_PARISC_FPTR64": 64	, #  64 bits function address.  
		"R_PARISC_PLABEL32": 65	, #  32 bits function address.  
		"R_PARISC_PLABEL21L": 66	, #  Left 21 bits of fdesc address.  
		"R_PARISC_PLABEL14R": 70	, #  Right 14 bits of fdesc address.  
		"R_PARISC_PCREL64": 72	, #  64 bits PC-rel. address.  
		"R_PARISC_PCREL22F": 74	, #  22 bits PC-rel. address.  
		"R_PARISC_PCREL14WR": 75	, #  PC-rel. address, right 14 bits.  
		"R_PARISC_PCREL14DR": 76	, #  PC rel. address, right 14 bits.  
		"R_PARISC_PCREL16F": 77	, #  16 bits PC-rel. address.  
		"R_PARISC_PCREL16WF": 78	, #  16 bits PC-rel. address.  
		"R_PARISC_PCREL16DF": 79	, #  16 bits PC-rel. address.  
		"R_PARISC_DIR64": 80	, #  64 bits of eff. address.  
		"R_PARISC_DIR14WR": 83	, #  14 bits of eff. address.  
		"R_PARISC_DIR14DR": 84	, #  14 bits of eff. address.  
		"R_PARISC_DIR16F": 85	, #  16 bits of eff. address.  
		"R_PARISC_DIR16WF": 86	, #  16 bits of eff. address.  
		"R_PARISC_DIR16DF": 87	, #  16 bits of eff. address.  
		"R_PARISC_GPREL64": 88	, #  64 bits of GP-rel. address.  
		"R_PARISC_GPREL14WR": 91	, #  GP-rel. address, right 14 bits.  
		"R_PARISC_GPREL14DR": 92	, #  GP-rel. address, right 14 bits.  
		"R_PARISC_GPREL16F": 93	, #  16 bits GP-rel. address.  
		"R_PARISC_GPREL16WF": 94	, #  16 bits GP-rel. address.  
		"R_PARISC_GPREL16DF": 95	, #  16 bits GP-rel. address.  
		"R_PARISC_LTOFF64": 96	, #  64 bits LT-rel. address.  
		"R_PARISC_LTOFF14WR": 99	, #  LT-rel. address, right 14 bits.  
		"R_PARISC_LTOFF14DR": 100	, #  LT-rel. address, right 14 bits.  
		"R_PARISC_LTOFF16F": 101	, #  16 bits LT-rel. address.  
		"R_PARISC_LTOFF16WF": 102	, #  16 bits LT-rel. address.  
		"R_PARISC_LTOFF16DF": 103	, #  16 bits LT-rel. address.  
		"R_PARISC_SECREL64": 104	, #  64 bits section rel. address.  
		"R_PARISC_SEGREL64": 112	, #  64 bits segment rel. address.  
		"R_PARISC_PLTOFF14WR": 115	, #  PLT-rel. address, right 14 bits.  
		"R_PARISC_PLTOFF14DR": 116	, #  PLT-rel. address, right 14 bits.  
		"R_PARISC_PLTOFF16F": 117	, #  16 bits LT-rel. address.  
		"R_PARISC_PLTOFF16WF": 118	, #  16 bits PLT-rel. address.  
		"R_PARISC_PLTOFF16DF": 119	, #  16 bits PLT-rel. address.  
		"R_PARISC_LTOFF_FPTR64": 120	, #  64 bits LT-rel. function ptr.  
		"R_PARISC_LTOFF_FPTR14WR": 123	, #  LT-rel. fct. ptr., right 14 bits. 
		"R_PARISC_LTOFF_FPTR14DR": 124	, #  LT-rel. fct. ptr., right 14 bits. 
		"R_PARISC_LTOFF_FPTR16F": 125	, #  16 bits LT-rel. function ptr.  
		"R_PARISC_LTOFF_FPTR16WF": 126	, #  16 bits LT-rel. function ptr.  
		"R_PARISC_LTOFF_FPTR16DF": 127	, #  16 bits LT-rel. function ptr.  
		"R_PARISC_LORESERVE": 128	, # 
		"R_PARISC_COPY": 128	, #  Copy relocation.  
		"R_PARISC_IPLT": 129	, #  Dynamic reloc, imported PLT 
		"R_PARISC_EPLT": 130	, #  Dynamic reloc, exported PLT 
		"R_PARISC_TPREL32": 153	, #  32 bits TP-rel. address.  
		"R_PARISC_TPREL21L": 154	, #  TP-rel. address, left 21 bits.  
		"R_PARISC_TPREL14R": 158	, #  TP-rel. address, right 14 bits.  
		"R_PARISC_LTOFF_TP21L": 162	, #  LT-TP-rel. address, left 21 bits. 
		"R_PARISC_LTOFF_TP14R": 166	, #  LT-TP-rel. address, right 14 bits.
		"R_PARISC_LTOFF_TP14F": 167	, #  14 bits LT-TP-rel. address.  
		"R_PARISC_TPREL64": 216	, #  64 bits TP-rel. address.  
		"R_PARISC_TPREL14WR": 219	, #  TP-rel. address, right 14 bits.  
		"R_PARISC_TPREL14DR": 220	, #  TP-rel. address, right 14 bits.  
		"R_PARISC_TPREL16F": 221	, #  16 bits TP-rel. address.  
		"R_PARISC_TPREL16WF": 222	, #  16 bits TP-rel. address.  
		"R_PARISC_TPREL16DF": 223	, #  16 bits TP-rel. address.  
		"R_PARISC_LTOFF_TP64": 224	, #  64 bits LT-TP-rel. address.  
		"R_PARISC_LTOFF_TP14WR": 227	, #  LT-TP-rel. address, right 14 bits.
		"R_PARISC_LTOFF_TP14DR": 228	, #  LT-TP-rel. address, right 14 bits.
		"R_PARISC_LTOFF_TP16F": 229	, #  16 bits LT-TP-rel. address.  
		"R_PARISC_LTOFF_TP16WF": 230	, #  16 bits LT-TP-rel. address.  
		"R_PARISC_LTOFF_TP16DF": 231	, #  16 bits LT-TP-rel. address.  
		"R_PARISC_GNU_VTENTRY": 232	, # 
		"R_PARISC_GNU_VTINHERIT": 233	, # 
		"R_PARISC_TLS_GD21L": 234	, #  GD 21-bit left.  
		"R_PARISC_TLS_GD14R": 235	, #  GD 14-bit right.  
		"R_PARISC_TLS_GDCALL": 236	, #  GD call to __t_g_a.  
		"R_PARISC_TLS_LDM21L": 237	, #  LD module 21-bit left.  
		"R_PARISC_TLS_LDM14R": 238	, #  LD module 14-bit right.  
		"R_PARISC_TLS_LDMCALL": 239	, #  LD module call to __t_g_a.  
		"R_PARISC_TLS_LDO21L": 240	, #  LD offset 21-bit left.  
		"R_PARISC_TLS_LDO14R": 241	, #  LD offset 14-bit right.  
		"R_PARISC_TLS_DTPMOD32": 242	, #  DTP module 32-bit.  
		"R_PARISC_TLS_DTPMOD64": 243	, #  DTP module 64-bit.  
		"R_PARISC_TLS_DTPOFF32": 244	, #  DTP offset 32-bit.  
		"R_PARISC_TLS_DTPOFF64": 245	, #  DTP offset 32-bit.  
		"R_PARISC_TLS_LE21L": MacroRef("R_PARISC_TPREL21L"), # 
		"R_PARISC_TLS_LE14R": MacroRef("R_PARISC_TPREL14R"), # 
		"R_PARISC_TLS_IE21L": MacroRef("R_PARISC_LTOFF_TP21L")	, # 
		"R_PARISC_TLS_IE14R": MacroRef("R_PARISC_LTOFF_TP14R")	, # 
		"R_PARISC_TLS_TPREL32": MacroRef("R_PARISC_TPREL32")	, # 
		"R_PARISC_TLS_TPREL64": MacroRef("R_PARISC_TPREL64")	, # 
		"R_PARISC_HIRESERVE": 255	, # 
		"PT_HP_TLS": (MacroRef("PT_LOOS") + 0x0)	, # 
		"PT_HP_CORE_NONE": (MacroRef("PT_LOOS") + 0x1)	, # 
		"PT_HP_CORE_VERSION": (MacroRef("PT_LOOS") + 0x2)	, # 
		"PT_HP_CORE_KERNEL": (MacroRef("PT_LOOS") + 0x3)	, # 
		"PT_HP_CORE_COMM": (MacroRef("PT_LOOS") + 0x4)	, # 
		"PT_HP_CORE_PROC": (MacroRef("PT_LOOS") + 0x5)	, # 
		"PT_HP_CORE_LOADABLE": (MacroRef("PT_LOOS") + 0x6)	, # 
		"PT_HP_CORE_STACK": (MacroRef("PT_LOOS") + 0x7)	, # 
		"PT_HP_CORE_SHM": (MacroRef("PT_LOOS") + 0x8)	, # 
		"PT_HP_CORE_MMF": (MacroRef("PT_LOOS") + 0x9)	, # 
		"PT_HP_PARALLEL": (MacroRef("PT_LOOS") + 0x10)	, # 
		"PT_HP_FASTBIND": (MacroRef("PT_LOOS") + 0x11)	, # 
		"PT_HP_OPT_ANNOT": (MacroRef("PT_LOOS") + 0x12)	, # 
		"PT_HP_HSL_ANNOT": (MacroRef("PT_LOOS") + 0x13)	, # 
		"PT_HP_STACK": (MacroRef("PT_LOOS") + 0x14)	, # 
		"PT_PARISC_ARCHEXT": 0x70000000	, # 
		"PT_PARISC_UNWIND": 0x70000001	, # 
		"PF_PARISC_SBP": 0x08000000	, # 
		"PF_HP_PAGE_SIZE": 0x00100000	, # 
		"PF_HP_FAR_SHARED": 0x00200000	, # 
		"PF_HP_NEAR_SHARED": 0x00400000	, # 
		"PF_HP_CODE": 0x01000000	, # 
		"PF_HP_MODIFY": 0x02000000	, # 
		"PF_HP_LAZYSWAP": 0x04000000	, # 
		"PF_HP_SBP": 0x08000000	, # 
		"EF_ALPHA_32BIT": 1	, #  All addresses must be < 2GB.  
		"EF_ALPHA_CANRELAX": 2	, #  Relocations for relaxing exist.  
		"SHT_ALPHA_DEBUG": 0x70000001	, # 
		"SHT_ALPHA_REGINFO": 0x70000002	, # 
		"SHF_ALPHA_GPREL": 0x10000000	, # 
		"STO_ALPHA_NOPV": 0x80	, #  No PV required.  
		"STO_ALPHA_STD_GPLOAD": 0x88	, #  PV only used for initial ldgp.  
		"R_ALPHA_NONE": 0	, #  No reloc 
		"R_ALPHA_REFLONG": 1	, #  Direct 32 bit 
		"R_ALPHA_REFQUAD": 2	, #  Direct 64 bit 
		"R_ALPHA_GPREL32": 3	, #  GP relative 32 bit 
		"R_ALPHA_LITERAL": 4	, #  GP relative 16 bit w/optimization 
		"R_ALPHA_LITUSE": 5	, #  Optimization hint for LITERAL 
		"R_ALPHA_GPDISP": 6	, #  Add displacement to GP 
		"R_ALPHA_BRADDR": 7	, #  PC+4 relative 23 bit shifted 
		"R_ALPHA_HINT": 8	, #  PC+4 relative 16 bit shifted 
		"R_ALPHA_SREL16": 9	, #  PC relative 16 bit 
		"R_ALPHA_SREL32": 10	, #  PC relative 32 bit 
		"R_ALPHA_SREL64": 11	, #  PC relative 64 bit 
		"R_ALPHA_GPRELHIGH": 17	, #  GP relative 32 bit, high 16 bits 
		"R_ALPHA_GPRELLOW": 18	, #  GP relative 32 bit, low 16 bits 
		"R_ALPHA_GPREL16": 19	, #  GP relative 16 bit 
		"R_ALPHA_COPY": 24	, #  Copy symbol at runtime 
		"R_ALPHA_GLOB_DAT": 25	, #  Create GOT entry 
		"R_ALPHA_JMP_SLOT": 26	, #  Create PLT entry 
		"R_ALPHA_RELATIVE": 27	, #  Adjust by program base 
		"R_ALPHA_TLS_GD_HI": 28	, # 
		"R_ALPHA_TLSGD": 29	, # 
		"R_ALPHA_TLS_LDM": 30	, # 
		"R_ALPHA_DTPMOD64": 31	, # 
		"R_ALPHA_GOTDTPREL": 32	, # 
		"R_ALPHA_DTPREL64": 33	, # 
		"R_ALPHA_DTPRELHI": 34	, # 
		"R_ALPHA_DTPRELLO": 35	, # 
		"R_ALPHA_DTPREL16": 36	, # 
		"R_ALPHA_GOTTPREL": 37	, # 
		"R_ALPHA_TPREL64": 38	, # 
		"R_ALPHA_TPRELHI": 39	, # 
		"R_ALPHA_TPRELLO": 40	, # 
		"R_ALPHA_TPREL16": 41	, # 
		"R_ALPHA_NUM": 46	, # 
		"LITUSE_ALPHA_ADDR": 0	, # 
		"LITUSE_ALPHA_BASE": 1	, # 
		"LITUSE_ALPHA_BYTOFF": 2	, # 
		"LITUSE_ALPHA_JSR": 3	, # 
		"LITUSE_ALPHA_TLS_GD": 4	, # 
		"LITUSE_ALPHA_TLS_LDM": 5	, # 
		"DT_ALPHA_PLTRO": (MacroRef("DT_LOPROC") + 0)	, # 
		"DT_ALPHA_NUM": 1	, # 
		"EF_PPC_EMB": 0x80000000	, #  PowerPC embedded flag 
		"EF_PPC_RELOCATABLE": 0x00010000	, #  PowerPC -mrelocatable flag
		"EF_PPC_RELOCATABLE_LIB": 0x00008000	, #  PowerPC -mrelocatable-lib
		"R_PPC_NONE": 0	, # 
		"R_PPC_ADDR32": 1	, #  32bit absolute address 
		"R_PPC_ADDR24": 2	, #  26bit address, 2 bits ignored.  
		"R_PPC_ADDR16": 3	, #  16bit absolute address 
		"R_PPC_ADDR16_LO": 4	, #  lower 16bit of absolute address 
		"R_PPC_ADDR16_HI": 5	, #  high 16bit of absolute address 
		"R_PPC_ADDR16_HA": 6	, #  adjusted high 16bit 
		"R_PPC_ADDR14": 7	, #  16bit address, 2 bits ignored 
		"R_PPC_ADDR14_BRTAKEN": 8	, # 
		"R_PPC_ADDR14_BRNTAKEN": 9	, # 
		"R_PPC_REL24": 10	, #  PC relative 26 bit 
		"R_PPC_REL14": 11	, #  PC relative 16 bit 
		"R_PPC_REL14_BRTAKEN": 12	, # 
		"R_PPC_REL14_BRNTAKEN": 13	, # 
		"R_PPC_GOT16": 14	, # 
		"R_PPC_GOT16_LO": 15	, # 
		"R_PPC_GOT16_HI": 16	, # 
		"R_PPC_GOT16_HA": 17	, # 
		"R_PPC_PLTREL24": 18	, # 
		"R_PPC_COPY": 19	, # 
		"R_PPC_GLOB_DAT": 20	, # 
		"R_PPC_JMP_SLOT": 21	, # 
		"R_PPC_RELATIVE": 22	, # 
		"R_PPC_LOCAL24PC": 23	, # 
		"R_PPC_UADDR32": 24	, # 
		"R_PPC_UADDR16": 25	, # 
		"R_PPC_REL32": 26	, # 
		"R_PPC_PLT32": 27	, # 
		"R_PPC_PLTREL32": 28	, # 
		"R_PPC_PLT16_LO": 29	, # 
		"R_PPC_PLT16_HI": 30	, # 
		"R_PPC_PLT16_HA": 31	, # 
		"R_PPC_SDAREL16": 32	, # 
		"R_PPC_SECTOFF": 33	, # 
		"R_PPC_SECTOFF_LO": 34	, # 
		"R_PPC_SECTOFF_HI": 35	, # 
		"R_PPC_SECTOFF_HA": 36	, # 
		"R_PPC_TLS": 67 , #  none	(sym+add)@tls 
		"R_PPC_DTPMOD32": 68 , #  word32	(sym+add)@dtpmod 
		"R_PPC_TPREL16": 69 , #  half16*	(sym+add)@tprel 
		"R_PPC_TPREL16_LO": 70 , #  half16	(sym+add)@tprel@l 
		"R_PPC_TPREL16_HI": 71 , #  half16	(sym+add)@tprel@h 
		"R_PPC_TPREL16_HA": 72 , #  half16	(sym+add)@tprel@ha 
		"R_PPC_TPREL32": 73 , #  word32	(sym+add)@tprel 
		"R_PPC_DTPREL16": 74 , #  half16*	(sym+add)@dtprel 
		"R_PPC_DTPREL16_LO": 75 , #  half16	(sym+add)@dtprel@l 
		"R_PPC_DTPREL16_HI": 76 , #  half16	(sym+add)@dtprel@h 
		"R_PPC_DTPREL16_HA": 77 , #  half16	(sym+add)@dtprel@ha 
		"R_PPC_DTPREL32": 78 , #  word32	(sym+add)@dtprel 
		"R_PPC_GOT_TLSGD16": 79 , #  half16*	(sym+add)@got@tlsgd 
		"R_PPC_GOT_TLSGD16_LO": 80 , #  half16	(sym+add)@got@tlsgd@l 
		"R_PPC_GOT_TLSGD16_HI": 81 , #  half16	(sym+add)@got@tlsgd@h 
		"R_PPC_GOT_TLSGD16_HA": 82 , #  half16	(sym+add)@got@tlsgd@ha 
		"R_PPC_GOT_TLSLD16": 83 , #  half16*	(sym+add)@got@tlsld 
		"R_PPC_GOT_TLSLD16_LO": 84 , #  half16	(sym+add)@got@tlsld@l 
		"R_PPC_GOT_TLSLD16_HI": 85 , #  half16	(sym+add)@got@tlsld@h 
		"R_PPC_GOT_TLSLD16_HA": 86 , #  half16	(sym+add)@got@tlsld@ha 
		"R_PPC_GOT_TPREL16": 87 , #  half16*	(sym+add)@got@tprel 
		"R_PPC_GOT_TPREL16_LO": 88 , #  half16	(sym+add)@got@tprel@l 
		"R_PPC_GOT_TPREL16_HI": 89 , #  half16	(sym+add)@got@tprel@h 
		"R_PPC_GOT_TPREL16_HA": 90 , #  half16	(sym+add)@got@tprel@ha 
		"R_PPC_GOT_DTPREL16": 91 , #  half16*	(sym+add)@got@dtprel 
		"R_PPC_GOT_DTPREL16_LO": 92 , #  half16*	(sym+add)@got@dtprel@l 
		"R_PPC_GOT_DTPREL16_HI": 93 , #  half16*	(sym+add)@got@dtprel@h 
		"R_PPC_GOT_DTPREL16_HA": 94 , #  half16*	(sym+add)@got@dtprel@ha 
		"R_PPC_EMB_NADDR32": 101	, # 
		"R_PPC_EMB_NADDR16": 102	, # 
		"R_PPC_EMB_NADDR16_LO": 103	, # 
		"R_PPC_EMB_NADDR16_HI": 104	, # 
		"R_PPC_EMB_NADDR16_HA": 105	, # 
		"R_PPC_EMB_SDAI16": 106	, # 
		"R_PPC_EMB_SDA2I16": 107	, # 
		"R_PPC_EMB_SDA2REL": 108	, # 
		"R_PPC_EMB_SDA21": 109	, #  16 bit offset in SDA 
		"R_PPC_EMB_MRKREF": 110	, # 
		"R_PPC_EMB_RELSEC16": 111	, # 
		"R_PPC_EMB_RELST_LO": 112	, # 
		"R_PPC_EMB_RELST_HI": 113	, # 
		"R_PPC_EMB_RELST_HA": 114	, # 
		"R_PPC_EMB_BIT_FLD": 115	, # 
		"R_PPC_EMB_RELSDA": 116	, #  16 bit relative offset in SDA 
		"R_PPC_DIAB_SDA21_LO": 180	, #  like EMB_SDA21, but lower 16 bit 
		"R_PPC_DIAB_SDA21_HI": 181	, #  like EMB_SDA21, but high 16 bit 
		"R_PPC_DIAB_SDA21_HA": 182	, #  like EMB_SDA21, adjusted high 16 
		"R_PPC_DIAB_RELSDA_LO": 183	, #  like EMB_RELSDA, but lower 16 bit 
		"R_PPC_DIAB_RELSDA_HI": 184	, #  like EMB_RELSDA, but high 16 bit 
		"R_PPC_DIAB_RELSDA_HA": 185	, #  like EMB_RELSDA, adjusted high 16 
		"R_PPC_IRELATIVE": 248	, # 
		"R_PPC_REL16": 249	, #  half16   (sym+add-.) 
		"R_PPC_REL16_LO": 250	, #  half16   (sym+add-.)@l 
		"R_PPC_REL16_HI": 251	, #  half16   (sym+add-.)@h 
		"R_PPC_REL16_HA": 252	, #  half16   (sym+add-.)@ha 
		"R_PPC_TOC16": 255	, # 
		"DT_PPC_GOT": (MacroRef("DT_LOPROC") + 0)	, # 
		"DT_PPC_NUM": 1	, # 
		"R_PPC64_NONE": MacroRef("R_PPC_NONE")	, # 
		"R_PPC64_ADDR32": MacroRef("R_PPC_ADDR32") , #  32bit absolute address 
		"R_PPC64_ADDR24": MacroRef("R_PPC_ADDR24") , #  26bit address, word aligned 
		"R_PPC64_ADDR16": MacroRef("R_PPC_ADDR16") , #  16bit absolute address 
		"R_PPC64_ADDR16_LO": MacroRef("R_PPC_ADDR16_LO")	, #  lower 16bits of address 
		"R_PPC64_ADDR16_HI": MacroRef("R_PPC_ADDR16_HI")	, #  high 16bits of address. 
		"R_PPC64_ADDR16_HA": MacroRef("R_PPC_ADDR16_HA") , #  adjusted high 16bits.  
		"R_PPC64_ADDR14": MacroRef("R_PPC_ADDR14") , #  16bit address, word aligned 
		"R_PPC64_ADDR14_BRTAKEN": MacroRef("R_PPC_ADDR14_BRTAKEN")	, # 
		"R_PPC64_ADDR14_BRNTAKEN": MacroRef("R_PPC_ADDR14_BRNTAKEN")	, # 
		"R_PPC64_REL24": MacroRef("R_PPC_REL24") , #  PC-rel. 26 bit, word aligned 
		"R_PPC64_REL14": MacroRef("R_PPC_REL14") , #  PC relative 16 bit 
		"R_PPC64_REL14_BRTAKEN": MacroRef("R_PPC_REL14_BRTAKEN")	, # 
		"R_PPC64_REL14_BRNTAKEN": MacroRef("R_PPC_REL14_BRNTAKEN")	, # 
		"R_PPC64_GOT16": MacroRef("R_PPC_GOT16")	, # 
		"R_PPC64_GOT16_LO": MacroRef("R_PPC_GOT16_LO")	, # 
		"R_PPC64_GOT16_HI": MacroRef("R_PPC_GOT16_HI")	, # 
		"R_PPC64_GOT16_HA": MacroRef("R_PPC_GOT16_HA")	, # 
		"R_PPC64_COPY": MacroRef("R_PPC_COPY")	, # 
		"R_PPC64_GLOB_DAT": MacroRef("R_PPC_GLOB_DAT")	, # 
		"R_PPC64_JMP_SLOT": MacroRef("R_PPC_JMP_SLOT")	, # 
		"R_PPC64_RELATIVE": MacroRef("R_PPC_RELATIVE")	, # 
		"R_PPC64_UADDR32": MacroRef("R_PPC_UADDR32")	, # 
		"R_PPC64_UADDR16": MacroRef("R_PPC_UADDR16")	, # 
		"R_PPC64_REL32": MacroRef("R_PPC_REL32")	, # 
		"R_PPC64_PLT32": MacroRef("R_PPC_PLT32")	, # 
		"R_PPC64_PLTREL32": MacroRef("R_PPC_PLTREL32")	, # 
		"R_PPC64_PLT16_LO": MacroRef("R_PPC_PLT16_LO")	, # 
		"R_PPC64_PLT16_HI": MacroRef("R_PPC_PLT16_HI")	, # 
		"R_PPC64_PLT16_HA": MacroRef("R_PPC_PLT16_HA")	, # 
		"R_PPC64_SECTOFF": MacroRef("R_PPC_SECTOFF")	, # 
		"R_PPC64_SECTOFF_LO": MacroRef("R_PPC_SECTOFF_LO")	, # 
		"R_PPC64_SECTOFF_HI": MacroRef("R_PPC_SECTOFF_HI")	, # 
		"R_PPC64_SECTOFF_HA": MacroRef("R_PPC_SECTOFF_HA")	, # 
		"R_PPC64_ADDR30": 37 , #  word30 (S + A - P) >> 2 
		"R_PPC64_ADDR64": 38 , #  doubleword64 S + A 
		"R_PPC64_ADDR16_HIGHER": 39 , #  half16 #higher(S + A) 
		"R_PPC64_ADDR16_HIGHERA": 40 , #  half16 #highera(S + A) 
		"R_PPC64_ADDR16_HIGHEST": 41 , #  half16 #highest(S + A) 
		"R_PPC64_ADDR16_HIGHESTA": 42 , #  half16 #highesta(S + A) 
		"R_PPC64_UADDR64": 43 , #  doubleword64 S + A 
		"R_PPC64_REL64": 44 , #  doubleword64 S + A - P 
		"R_PPC64_PLT64": 45 , #  doubleword64 L + A 
		"R_PPC64_PLTREL64": 46 , #  doubleword64 L + A - P 
		"R_PPC64_TOC16": 47 , #  half16* S + A - .TOC 
		"R_PPC64_TOC16_LO": 48 , #  half16 #lo(S + A - .TOC.) 
		"R_PPC64_TOC16_HI": 49 , #  half16 #hi(S + A - .TOC.) 
		"R_PPC64_TOC16_HA": 50 , #  half16 #ha(S + A - .TOC.) 
		"R_PPC64_TOC": 51 , #  doubleword64 .TOC 
		"R_PPC64_PLTGOT16": 52 , #  half16* M + A 
		"R_PPC64_PLTGOT16_LO": 53 , #  half16 #lo(M + A) 
		"R_PPC64_PLTGOT16_HI": 54 , #  half16 #hi(M + A) 
		"R_PPC64_PLTGOT16_HA": 55 , #  half16 #ha(M + A) 
		"R_PPC64_ADDR16_DS": 56 , #  half16ds* (S + A) >> 2 
		"R_PPC64_ADDR16_LO_DS": 57 , #  half16ds  #lo(S + A) >> 2 
		"R_PPC64_GOT16_DS": 58 , #  half16ds* (G + A) >> 2 
		"R_PPC64_GOT16_LO_DS": 59 , #  half16ds  #lo(G + A) >> 2 
		"R_PPC64_PLT16_LO_DS": 60 , #  half16ds  #lo(L + A) >> 2 
		"R_PPC64_SECTOFF_DS": 61 , #  half16ds* (R + A) >> 2 
		"R_PPC64_SECTOFF_LO_DS": 62 , #  half16ds  #lo(R + A) >> 2 
		"R_PPC64_TOC16_DS": 63 , #  half16ds* (S + A - .TOC.) >> 2 
		"R_PPC64_TOC16_LO_DS": 64 , #  half16ds  #lo(S + A - .TOC.) >> 2 
		"R_PPC64_PLTGOT16_DS": 65 , #  half16ds* (M + A) >> 2 
		"R_PPC64_PLTGOT16_LO_DS": 66 , #  half16ds  #lo(M + A) >> 2 
		"R_PPC64_TLS": 67 , #  none	(sym+add)@tls 
		"R_PPC64_DTPMOD64": 68 , #  doubleword64 (sym+add)@dtpmod 
		"R_PPC64_TPREL16": 69 , #  half16*	(sym+add)@tprel 
		"R_PPC64_TPREL16_LO": 70 , #  half16	(sym+add)@tprel@l 
		"R_PPC64_TPREL16_HI": 71 , #  half16	(sym+add)@tprel@h 
		"R_PPC64_TPREL16_HA": 72 , #  half16	(sym+add)@tprel@ha 
		"R_PPC64_TPREL64": 73 , #  doubleword64 (sym+add)@tprel 
		"R_PPC64_DTPREL16": 74 , #  half16*	(sym+add)@dtprel 
		"R_PPC64_DTPREL16_LO": 75 , #  half16	(sym+add)@dtprel@l 
		"R_PPC64_DTPREL16_HI": 76 , #  half16	(sym+add)@dtprel@h 
		"R_PPC64_DTPREL16_HA": 77 , #  half16	(sym+add)@dtprel@ha 
		"R_PPC64_DTPREL64": 78 , #  doubleword64 (sym+add)@dtprel 
		"R_PPC64_GOT_TLSGD16": 79 , #  half16*	(sym+add)@got@tlsgd 
		"R_PPC64_GOT_TLSGD16_LO": 80 , #  half16	(sym+add)@got@tlsgd@l 
		"R_PPC64_GOT_TLSGD16_HI": 81 , #  half16	(sym+add)@got@tlsgd@h 
		"R_PPC64_GOT_TLSGD16_HA": 82 , #  half16	(sym+add)@got@tlsgd@ha 
		"R_PPC64_GOT_TLSLD16": 83 , #  half16*	(sym+add)@got@tlsld 
		"R_PPC64_GOT_TLSLD16_LO": 84 , #  half16	(sym+add)@got@tlsld@l 
		"R_PPC64_GOT_TLSLD16_HI": 85 , #  half16	(sym+add)@got@tlsld@h 
		"R_PPC64_GOT_TLSLD16_HA": 86 , #  half16	(sym+add)@got@tlsld@ha 
		"R_PPC64_GOT_TPREL16_DS": 87 , #  half16ds*	(sym+add)@got@tprel 
		"R_PPC64_GOT_TPREL16_LO_DS": 88 , #  half16ds (sym+add)@got@tprel@l 
		"R_PPC64_GOT_TPREL16_HI": 89 , #  half16	(sym+add)@got@tprel@h 
		"R_PPC64_GOT_TPREL16_HA": 90 , #  half16	(sym+add)@got@tprel@ha 
		"R_PPC64_GOT_DTPREL16_DS": 91 , #  half16ds*	(sym+add)@got@dtprel 
		"R_PPC64_GOT_DTPREL16_LO_DS": 92 , #  half16ds (sym+add)@got@dtprel@l 
		"R_PPC64_GOT_DTPREL16_HI": 93 , #  half16	(sym+add)@got@dtprel@h 
		"R_PPC64_GOT_DTPREL16_HA": 94 , #  half16	(sym+add)@got@dtprel@ha 
		"R_PPC64_TPREL16_DS": 95 , #  half16ds*	(sym+add)@tprel 
		"R_PPC64_TPREL16_LO_DS": 96 , #  half16ds	(sym+add)@tprel@l 
		"R_PPC64_TPREL16_HIGHER": 97 , #  half16	(sym+add)@tprel@higher 
		"R_PPC64_TPREL16_HIGHERA": 98 , #  half16	(sym+add)@tprel@highera 
		"R_PPC64_TPREL16_HIGHEST": 99 , #  half16	(sym+add)@tprel@highest 
		"R_PPC64_TPREL16_HIGHESTA": 100 , #  half16	(sym+add)@tprel@highesta 
		"R_PPC64_DTPREL16_DS": 101 , #  half16ds* (sym+add)@dtprel 
		"R_PPC64_DTPREL16_LO_DS": 102 , #  half16ds	(sym+add)@dtprel@l 
		"R_PPC64_DTPREL16_HIGHER": 103 , #  half16	(sym+add)@dtprel@higher 
		"R_PPC64_DTPREL16_HIGHERA": 104 , #  half16	(sym+add)@dtprel@highera 
		"R_PPC64_DTPREL16_HIGHEST": 105 , #  half16	(sym+add)@dtprel@highest 
		"R_PPC64_DTPREL16_HIGHESTA": 106 , #  half16	(sym+add)@dtprel@highesta 
		"R_PPC64_JMP_IREL": 247	, # 
		"R_PPC64_IRELATIVE": 248	, # 
		"R_PPC64_REL16": 249	, #  half16   (sym+add-.) 
		"R_PPC64_REL16_LO": 250	, #  half16   (sym+add-.)@l 
		"R_PPC64_REL16_HI": 251	, #  half16   (sym+add-.)@h 
		"R_PPC64_REL16_HA": 252	, #  half16   (sym+add-.)@ha 
		"DT_PPC64_GLINK": (MacroRef("DT_LOPROC") + 0)	, # 
		"DT_PPC64_OPD": (MacroRef("DT_LOPROC") + 1)	, # 
		"DT_PPC64_OPDSZ": (MacroRef("DT_LOPROC") + 2)	, # 
		"DT_PPC64_NUM": 3	, # 
		"EF_ARM_RELEXEC": 0x01	, # 
		"EF_ARM_HASENTRY": 0x02	, # 
		"EF_ARM_INTERWORK": 0x04	, # 
		"EF_ARM_APCS_26": 0x08	, # 
		"EF_ARM_APCS_FLOAT": 0x10	, # 
		"EF_ARM_PIC": 0x20	, # 
		"EF_ARM_ALIGN8": 0x40 , #  8-bit structure alignment is in use 
		"EF_ARM_NEW_ABI": 0x80	, # 
		"EF_ARM_OLD_ABI": 0x100	, # 
		"EF_ARM_SOFT_FLOAT": 0x200	, # 
		"EF_ARM_VFP_FLOAT": 0x400	, # 
		"EF_ARM_MAVERICK_FLOAT": 0x800	, # 
		"EF_ARM_SYMSARESORTED": 0x04	, # 
		"EF_ARM_DYNSYMSUSESEGIDX": 0x08	, # 
		"EF_ARM_MAPSYMSFIRST": 0x10	, # 
		"EF_ARM_EABIMASK": 0XFF000000	, # 
		"EF_ARM_BE8": 0x00800000	, # 
		"EF_ARM_LE8": 0x00400000	, # 
		"EF_ARM_EABI_UNKNOWN": 0x00000000	, # 
		"EF_ARM_EABI_VER1": 0x01000000	, # 
		"EF_ARM_EABI_VER2": 0x02000000	, # 
		"EF_ARM_EABI_VER3": 0x03000000	, # 
		"EF_ARM_EABI_VER4": 0x04000000	, # 
		"EF_ARM_EABI_VER5": 0x05000000	, # 
		"STT_ARM_TFUNC": MacroRef("STT_LOPROC") , #  A Thumb function.  
		"STT_ARM_16BIT": MacroRef("STT_HIPROC") , #  A Thumb label.  
		"SHF_ARM_ENTRYSECT": 0x10000000 , #  Section contains an entry point 
		"SHF_ARM_COMDEF": 0x80000000 , #  Section may be multiply defined
		"PF_ARM_SB": 0x10000000 , #  Segment contains the location
		"PF_ARM_PI": 0x20000000 , #  Position-independent segment.  
		"PF_ARM_ABS": 0x40000000 , #  Absolute segment.  
		"PT_ARM_EXIDX": (MacroRef("PT_LOPROC") + 1)	, #  ARM unwind segment.  
		"SHT_ARM_EXIDX": (MacroRef("SHT_LOPROC") + 1) , #  ARM unwind section.  
		"SHT_ARM_PREEMPTMAP": (MacroRef("SHT_LOPROC") + 2) , #  Preemption details.  
		"SHT_ARM_ATTRIBUTES": (MacroRef("SHT_LOPROC") + 3) , #  ARM attributes section.  
		"R_ARM_NONE": 0	, #  No reloc 
		"R_ARM_PC24": 1	, #  PC relative 26 bit branch 
		"R_ARM_ABS32": 2	, #  Direct 32 bit  
		"R_ARM_REL32": 3	, #  PC relative 32 bit 
		"R_ARM_PC13": 4	, # 
		"R_ARM_ABS16": 5	, #  Direct 16 bit 
		"R_ARM_ABS12": 6	, #  Direct 12 bit 
		"R_ARM_THM_ABS5": 7	, # 
		"R_ARM_ABS8": 8	, #  Direct 8 bit 
		"R_ARM_SBREL32": 9	, # 
		"R_ARM_THM_PC22": 10	, # 
		"R_ARM_THM_PC8": 11	, # 
		"R_ARM_AMP_VCALL9": 12	, # 
		"R_ARM_SWI24": 13	, #  Obsolete static relocation.  
		"R_ARM_TLS_DESC": 13      , #  Dynamic relocation.  
		"R_ARM_THM_SWI8": 14	, # 
		"R_ARM_XPC25": 15	, # 
		"R_ARM_THM_XPC22": 16	, # 
		"R_ARM_TLS_DTPMOD32": 17	, #  ID of module containing symbol 
		"R_ARM_TLS_DTPOFF32": 18	, #  Offset in TLS block 
		"R_ARM_TLS_TPOFF32": 19	, #  Offset in static TLS block 
		"R_ARM_COPY": 20	, #  Copy symbol at runtime 
		"R_ARM_GLOB_DAT": 21	, #  Create GOT entry 
		"R_ARM_JUMP_SLOT": 22	, #  Create PLT entry 
		"R_ARM_RELATIVE": 23	, #  Adjust by program base 
		"R_ARM_GOTOFF": 24	, #  32 bit offset to GOT 
		"R_ARM_GOTPC": 25	, #  32 bit PC relative offset to GOT 
		"R_ARM_GOT32": 26	, #  32 bit GOT entry 
		"R_ARM_PLT32": 27	, #  32 bit PLT address 
		"R_ARM_ALU_PCREL_7_0": 32	, # 
		"R_ARM_ALU_PCREL_15_8": 33	, # 
		"R_ARM_ALU_PCREL_23_15": 34	, # 
		"R_ARM_LDR_SBREL_11_0": 35	, # 
		"R_ARM_ALU_SBREL_19_12": 36	, # 
		"R_ARM_ALU_SBREL_27_20": 37	, # 
		"R_ARM_TLS_GOTDESC": 90	, # 
		"R_ARM_TLS_CALL": 91	, # 
		"R_ARM_TLS_DESCSEQ": 92	, # 
		"R_ARM_THM_TLS_CALL": 93	, # 
		"R_ARM_GNU_VTENTRY": 100	, # 
		"R_ARM_GNU_VTINHERIT": 101	, # 
		"R_ARM_THM_PC11": 102	, #  thumb unconditional branch 
		"R_ARM_THM_PC9": 103	, #  thumb conditional branch 
		"R_ARM_TLS_GD32": 104	, #  PC-rel 32 bit for global dynamic
		"R_ARM_TLS_LDM32": 105	, #  PC-rel 32 bit for local dynamic
		"R_ARM_TLS_LDO32": 106	, #  32 bit offset relative to TLS
		"R_ARM_TLS_IE32": 107	, #  PC-rel 32 bit for GOT entry of
		"R_ARM_TLS_LE32": 108	, #  32 bit offset relative to static
		"R_ARM_THM_TLS_DESCSEQ": 129	, # 
		"R_ARM_IRELATIVE": 160	, # 
		"R_ARM_RXPC25": 249	, # 
		"R_ARM_RSBREL32": 250	, # 
		"R_ARM_THM_RPC22": 251	, # 
		"R_ARM_RREL32": 252	, # 
		"R_ARM_RABS22": 253	, # 
		"R_ARM_RPC24": 254	, # 
		"R_ARM_RBASE": 255	, # 
		"R_ARM_NUM": 256	, # 
		"EF_IA_64_MASKOS": 0x0000000f	, #  os-specific flags 
		"EF_IA_64_ABI64": 0x00000010	, #  64-bit ABI 
		"EF_IA_64_ARCH": 0xff000000	, #  arch. version mask 
		"PT_IA_64_ARCHEXT": (MacroRef("PT_LOPROC") + 0)	, #  arch extension bits 
		"PT_IA_64_UNWIND": (MacroRef("PT_LOPROC") + 1)	, #  ia64 unwind bits 
		"PT_IA_64_HP_OPT_ANOT": (MacroRef("PT_LOOS") + 0x12)	, # 
		"PT_IA_64_HP_HSL_ANOT": (MacroRef("PT_LOOS") + 0x13)	, # 
		"PT_IA_64_HP_STACK": (MacroRef("PT_LOOS") + 0x14)	, # 
		"PF_IA_64_NORECOV": 0x80000000	, #  spec insns w/o recovery 
		"SHT_IA_64_EXT": (MacroRef("SHT_LOPROC") + 0) , #  extension bits 
		"SHT_IA_64_UNWIND": (MacroRef("SHT_LOPROC") + 1) , #  unwind bits 
		"SHF_IA_64_SHORT": 0x10000000	, #  section near gp 
		"SHF_IA_64_NORECOV": 0x20000000	, #  spec insns w/o recovery 
		"DT_IA_64_PLT_RESERVE": (MacroRef("DT_LOPROC") + 0)	, # 
		"DT_IA_64_NUM": 1	, # 
		"R_IA64_NONE": 0x00	, #  none 
		"R_IA64_IMM14": 0x21	, #  symbol + addend, add imm14 
		"R_IA64_IMM22": 0x22	, #  symbol + addend, add imm22 
		"R_IA64_IMM64": 0x23	, #  symbol + addend, mov imm64 
		"R_IA64_DIR32MSB": 0x24	, #  symbol + addend, data4 MSB 
		"R_IA64_DIR32LSB": 0x25	, #  symbol + addend, data4 LSB 
		"R_IA64_DIR64MSB": 0x26	, #  symbol + addend, data8 MSB 
		"R_IA64_DIR64LSB": 0x27	, #  symbol + addend, data8 LSB 
		"R_IA64_GPREL22": 0x2a	, #  @gprel(sym + add), add imm22 
		"R_IA64_GPREL64I": 0x2b	, #  @gprel(sym + add), mov imm64 
		"R_IA64_GPREL32MSB": 0x2c	, #  @gprel(sym + add), data4 MSB 
		"R_IA64_GPREL32LSB": 0x2d	, #  @gprel(sym + add), data4 LSB 
		"R_IA64_GPREL64MSB": 0x2e	, #  @gprel(sym + add), data8 MSB 
		"R_IA64_GPREL64LSB": 0x2f	, #  @gprel(sym + add), data8 LSB 
		"R_IA64_LTOFF22": 0x32	, #  @ltoff(sym + add), add imm22 
		"R_IA64_LTOFF64I": 0x33	, #  @ltoff(sym + add), mov imm64 
		"R_IA64_PLTOFF22": 0x3a	, #  @pltoff(sym + add), add imm22 
		"R_IA64_PLTOFF64I": 0x3b	, #  @pltoff(sym + add), mov imm64 
		"R_IA64_PLTOFF64MSB": 0x3e	, #  @pltoff(sym + add), data8 MSB 
		"R_IA64_PLTOFF64LSB": 0x3f	, #  @pltoff(sym + add), data8 LSB 
		"R_IA64_FPTR64I": 0x43	, #  @fptr(sym + add), mov imm64 
		"R_IA64_FPTR32MSB": 0x44	, #  @fptr(sym + add), data4 MSB 
		"R_IA64_FPTR32LSB": 0x45	, #  @fptr(sym + add), data4 LSB 
		"R_IA64_FPTR64MSB": 0x46	, #  @fptr(sym + add), data8 MSB 
		"R_IA64_FPTR64LSB": 0x47	, #  @fptr(sym + add), data8 LSB 
		"R_IA64_PCREL60B": 0x48	, #  @pcrel(sym + add), brl 
		"R_IA64_PCREL21B": 0x49	, #  @pcrel(sym + add), ptb, call 
		"R_IA64_PCREL21M": 0x4a	, #  @pcrel(sym + add), chk.s 
		"R_IA64_PCREL21F": 0x4b	, #  @pcrel(sym + add), fchkf 
		"R_IA64_PCREL32MSB": 0x4c	, #  @pcrel(sym + add), data4 MSB 
		"R_IA64_PCREL32LSB": 0x4d	, #  @pcrel(sym + add), data4 LSB 
		"R_IA64_PCREL64MSB": 0x4e	, #  @pcrel(sym + add), data8 MSB 
		"R_IA64_PCREL64LSB": 0x4f	, #  @pcrel(sym + add), data8 LSB 
		"R_IA64_LTOFF_FPTR22": 0x52	, #  @ltoff(@fptr(s+a)), imm22 
		"R_IA64_LTOFF_FPTR64I": 0x53	, #  @ltoff(@fptr(s+a)), imm64 
		"R_IA64_LTOFF_FPTR32MSB": 0x54	, #  @ltoff(@fptr(s+a)), data4 MSB 
		"R_IA64_LTOFF_FPTR32LSB": 0x55	, #  @ltoff(@fptr(s+a)), data4 LSB 
		"R_IA64_LTOFF_FPTR64MSB": 0x56	, #  @ltoff(@fptr(s+a)), data8 MSB 
		"R_IA64_LTOFF_FPTR64LSB": 0x57	, #  @ltoff(@fptr(s+a)), data8 LSB 
		"R_IA64_SEGREL32MSB": 0x5c	, #  @segrel(sym + add), data4 MSB 
		"R_IA64_SEGREL32LSB": 0x5d	, #  @segrel(sym + add), data4 LSB 
		"R_IA64_SEGREL64MSB": 0x5e	, #  @segrel(sym + add), data8 MSB 
		"R_IA64_SEGREL64LSB": 0x5f	, #  @segrel(sym + add), data8 LSB 
		"R_IA64_SECREL32MSB": 0x64	, #  @secrel(sym + add), data4 MSB 
		"R_IA64_SECREL32LSB": 0x65	, #  @secrel(sym + add), data4 LSB 
		"R_IA64_SECREL64MSB": 0x66	, #  @secrel(sym + add), data8 MSB 
		"R_IA64_SECREL64LSB": 0x67	, #  @secrel(sym + add), data8 LSB 
		"R_IA64_REL32MSB": 0x6c	, #  data 4 + REL 
		"R_IA64_REL32LSB": 0x6d	, #  data 4 + REL 
		"R_IA64_REL64MSB": 0x6e	, #  data 8 + REL 
		"R_IA64_REL64LSB": 0x6f	, #  data 8 + REL 
		"R_IA64_LTV32MSB": 0x74	, #  symbol + addend, data4 MSB 
		"R_IA64_LTV32LSB": 0x75	, #  symbol + addend, data4 LSB 
		"R_IA64_LTV64MSB": 0x76	, #  symbol + addend, data8 MSB 
		"R_IA64_LTV64LSB": 0x77	, #  symbol + addend, data8 LSB 
		"R_IA64_PCREL21BI": 0x79	, #  @pcrel(sym + add), 21bit inst 
		"R_IA64_PCREL22": 0x7a	, #  @pcrel(sym + add), 22bit inst 
		"R_IA64_PCREL64I": 0x7b	, #  @pcrel(sym + add), 64bit inst 
		"R_IA64_IPLTMSB": 0x80	, #  dynamic reloc, imported PLT, MSB 
		"R_IA64_IPLTLSB": 0x81	, #  dynamic reloc, imported PLT, LSB 
		"R_IA64_COPY": 0x84	, #  copy relocation 
		"R_IA64_SUB": 0x85	, #  Addend and symbol difference 
		"R_IA64_LTOFF22X": 0x86	, #  LTOFF22, relaxable.  
		"R_IA64_LDXMOV": 0x87	, #  Use of LTOFF22X.  
		"R_IA64_TPREL14": 0x91	, #  @tprel(sym + add), imm14 
		"R_IA64_TPREL22": 0x92	, #  @tprel(sym + add), imm22 
		"R_IA64_TPREL64I": 0x93	, #  @tprel(sym + add), imm64 
		"R_IA64_TPREL64MSB": 0x96	, #  @tprel(sym + add), data8 MSB 
		"R_IA64_TPREL64LSB": 0x97	, #  @tprel(sym + add), data8 LSB 
		"R_IA64_LTOFF_TPREL22": 0x9a	, #  @ltoff(@tprel(s+a)), imm2 
		"R_IA64_DTPMOD64MSB": 0xa6	, #  @dtpmod(sym + add), data8 MSB 
		"R_IA64_DTPMOD64LSB": 0xa7	, #  @dtpmod(sym + add), data8 LSB 
		"R_IA64_LTOFF_DTPMOD22": 0xaa	, #  @ltoff(@dtpmod(sym + add)), imm22 
		"R_IA64_DTPREL14": 0xb1	, #  @dtprel(sym + add), imm14 
		"R_IA64_DTPREL22": 0xb2	, #  @dtprel(sym + add), imm22 
		"R_IA64_DTPREL64I": 0xb3	, #  @dtprel(sym + add), imm64 
		"R_IA64_DTPREL32MSB": 0xb4	, #  @dtprel(sym + add), data4 MSB 
		"R_IA64_DTPREL32LSB": 0xb5	, #  @dtprel(sym + add), data4 LSB 
		"R_IA64_DTPREL64MSB": 0xb6	, #  @dtprel(sym + add), data8 MSB 
		"R_IA64_DTPREL64LSB": 0xb7	, #  @dtprel(sym + add), data8 LSB 
		"R_IA64_LTOFF_DTPREL22": 0xba	, #  @ltoff(@dtprel(s+a)), imm22 
		"EF_SH_MACH_MASK": 0x1f	, # 
		"EF_SH_UNKNOWN": 0x0	, # 
		"EF_SH1": 0x1	, # 
		"EF_SH2": 0x2	, # 
		"EF_SH3": 0x3	, # 
		"EF_SH_DSP": 0x4	, # 
		"EF_SH3_DSP": 0x5	, # 
		"EF_SH4AL_DSP": 0x6	, # 
		"EF_SH3E": 0x8	, # 
		"EF_SH4": 0x9	, # 
		"EF_SH2E": 0xb	, # 
		"EF_SH4A": 0xc	, # 
		"EF_SH2A": 0xd	, # 
		"EF_SH4_NOFPU": 0x10	, # 
		"EF_SH4A_NOFPU": 0x11	, # 
		"EF_SH4_NOMMU_NOFPU": 0x12	, # 
		"EF_SH2A_NOFPU": 0x13	, # 
		"EF_SH3_NOMMU": 0x14	, # 
		"EF_SH2A_SH4_NOFPU": 0x15	, # 
		"EF_SH2A_SH3_NOFPU": 0x16	, # 
		"EF_SH2A_SH4": 0x17	, # 
		"EF_SH2A_SH3E": 0x18	, # 
		"R_SH_NONE": 0	, # 
		"R_SH_DIR32": 1	, # 
		"R_SH_REL32": 2	, # 
		"R_SH_DIR8WPN": 3	, # 
		"R_SH_IND12W": 4	, # 
		"R_SH_DIR8WPL": 5	, # 
		"R_SH_DIR8WPZ": 6	, # 
		"R_SH_DIR8BP": 7	, # 
		"R_SH_DIR8W": 8	, # 
		"R_SH_DIR8L": 9	, # 
		"R_SH_SWITCH16": 25	, # 
		"R_SH_SWITCH32": 26	, # 
		"R_SH_USES": 27	, # 
		"R_SH_COUNT": 28	, # 
		"R_SH_ALIGN": 29	, # 
		"R_SH_CODE": 30	, # 
		"R_SH_DATA": 31	, # 
		"R_SH_LABEL": 32	, # 
		"R_SH_SWITCH8": 33	, # 
		"R_SH_GNU_VTINHERIT": 34	, # 
		"R_SH_GNU_VTENTRY": 35	, # 
		"R_SH_TLS_GD_32": 144	, # 
		"R_SH_TLS_LD_32": 145	, # 
		"R_SH_TLS_LDO_32": 146	, # 
		"R_SH_TLS_IE_32": 147	, # 
		"R_SH_TLS_LE_32": 148	, # 
		"R_SH_TLS_DTPMOD32": 149	, # 
		"R_SH_TLS_DTPOFF32": 150	, # 
		"R_SH_TLS_TPOFF32": 151	, # 
		"R_SH_GOT32": 160	, # 
		"R_SH_PLT32": 161	, # 
		"R_SH_COPY": 162	, # 
		"R_SH_GLOB_DAT": 163	, # 
		"R_SH_JMP_SLOT": 164	, # 
		"R_SH_RELATIVE": 165	, # 
		"R_SH_GOTOFF": 166	, # 
		"R_SH_GOTPC": 167	, # 
		"R_SH_NUM": 256	, # 
		"EF_S390_HIGH_GPRS": 0x00000001  , #  High GPRs kernel facility needed.  
		"R_390_NONE": 0	, #  No reloc.  
		"R_390_8": 1	, #  Direct 8 bit.  
		"R_390_12": 2	, #  Direct 12 bit.  
		"R_390_16": 3	, #  Direct 16 bit.  
		"R_390_32": 4	, #  Direct 32 bit.  
		"R_390_PC32": 5	, #  PC relative 32 bit.	
		"R_390_GOT12": 6	, #  12 bit GOT offset.  
		"R_390_GOT32": 7	, #  32 bit GOT offset.  
		"R_390_PLT32": 8	, #  32 bit PC relative PLT address.  
		"R_390_COPY": 9	, #  Copy symbol at runtime.  
		"R_390_GLOB_DAT": 10	, #  Create GOT entry.  
		"R_390_JMP_SLOT": 11	, #  Create PLT entry.  
		"R_390_RELATIVE": 12	, #  Adjust by program base.  
		"R_390_GOTOFF32": 13	, #  32 bit offset to GOT.	 
		"R_390_GOTPC": 14	, #  32 bit PC relative offset to GOT.  
		"R_390_GOT16": 15	, #  16 bit GOT offset.  
		"R_390_PC16": 16	, #  PC relative 16 bit.	
		"R_390_PC16DBL": 17	, #  PC relative 16 bit shifted by 1.  
		"R_390_PLT16DBL": 18	, #  16 bit PC rel. PLT shifted by 1.  
		"R_390_PC32DBL": 19	, #  PC relative 32 bit shifted by 1.  
		"R_390_PLT32DBL": 20	, #  32 bit PC rel. PLT shifted by 1.  
		"R_390_GOTPCDBL": 21	, #  32 bit PC rel. GOT shifted by 1.  
		"R_390_64": 22	, #  Direct 64 bit.  
		"R_390_PC64": 23	, #  PC relative 64 bit.	
		"R_390_GOT64": 24	, #  64 bit GOT offset.  
		"R_390_PLT64": 25	, #  64 bit PC relative PLT address.  
		"R_390_GOTENT": 26	, #  32 bit PC rel. to GOT entry >> 1. 
		"R_390_GOTOFF16": 27	, #  16 bit offset to GOT. 
		"R_390_GOTOFF64": 28	, #  64 bit offset to GOT. 
		"R_390_GOTPLT12": 29	, #  12 bit offset to jump slot.	
		"R_390_GOTPLT16": 30	, #  16 bit offset to jump slot.	
		"R_390_GOTPLT32": 31	, #  32 bit offset to jump slot.	
		"R_390_GOTPLT64": 32	, #  64 bit offset to jump slot.	
		"R_390_GOTPLTENT": 33	, #  32 bit rel. offset to jump slot.  
		"R_390_PLTOFF16": 34	, #  16 bit offset from GOT to PLT. 
		"R_390_PLTOFF32": 35	, #  32 bit offset from GOT to PLT. 
		"R_390_PLTOFF64": 36	, #  16 bit offset from GOT to PLT. 
		"R_390_TLS_LOAD": 37	, #  Tag for load insn in TLS code.  
		"R_390_TLS_GDCALL": 38	, #  Tag for function call in general
		"R_390_TLS_LDCALL": 39	, #  Tag for function call in local
		"R_390_TLS_GD32": 40	, #  Direct 32 bit for general dynamic
		"R_390_TLS_GD64": 41	, #  Direct 64 bit for general dynamic
		"R_390_TLS_GOTIE12": 42	, #  12 bit GOT offset for static TLS
		"R_390_TLS_GOTIE32": 43	, #  32 bit GOT offset for static TLS
		"R_390_TLS_GOTIE64": 44	, #  64 bit GOT offset for static TLS
		"R_390_TLS_LDM32": 45	, #  Direct 32 bit for local dynamic
		"R_390_TLS_LDM64": 46	, #  Direct 64 bit for local dynamic
		"R_390_TLS_IE32": 47	, #  32 bit address of GOT entry for
		"R_390_TLS_IE64": 48	, #  64 bit address of GOT entry for
		"R_390_TLS_IEENT": 49	, #  32 bit rel. offset to GOT entry for
		"R_390_TLS_LE32": 50	, #  32 bit negated offset relative to
		"R_390_TLS_LE64": 51	, #  64 bit negated offset relative to
		"R_390_TLS_LDO32": 52	, #  32 bit offset relative to TLS
		"R_390_TLS_LDO64": 53	, #  64 bit offset relative to TLS
		"R_390_TLS_DTPMOD": 54	, #  ID of module containing symbol.  
		"R_390_TLS_DTPOFF": 55	, #  Offset in TLS block.	 
		"R_390_TLS_TPOFF": 56	, #  Negated offset in static TLS
		"R_390_20": 57	, #  Direct 20 bit.  
		"R_390_GOT20": 58	, #  20 bit GOT offset.  
		"R_390_GOTPLT20": 59	, #  20 bit offset to jump slot.  
		"R_390_TLS_GOTIE20": 60	, #  20 bit GOT offset for static TLS
		"R_390_NUM": 61	, # 
		"R_CRIS_NONE": 0	, # 
		"R_CRIS_8": 1	, # 
		"R_CRIS_16": 2	, # 
		"R_CRIS_32": 3	, # 
		"R_CRIS_8_PCREL": 4	, # 
		"R_CRIS_16_PCREL": 5	, # 
		"R_CRIS_32_PCREL": 6	, # 
		"R_CRIS_GNU_VTINHERIT": 7	, # 
		"R_CRIS_GNU_VTENTRY": 8	, # 
		"R_CRIS_COPY": 9	, # 
		"R_CRIS_GLOB_DAT": 10	, # 
		"R_CRIS_JUMP_SLOT": 11	, # 
		"R_CRIS_RELATIVE": 12	, # 
		"R_CRIS_16_GOT": 13	, # 
		"R_CRIS_32_GOT": 14	, # 
		"R_CRIS_16_GOTPLT": 15	, # 
		"R_CRIS_32_GOTPLT": 16	, # 
		"R_CRIS_32_GOTREL": 17	, # 
		"R_CRIS_32_PLT_GOTREL": 18	, # 
		"R_CRIS_32_PLT_PCREL": 19	, # 
		"R_CRIS_NUM": 20	, # 
		"R_X86_64_NONE": 0	, #  No reloc 
		"R_X86_64_64": 1	, #  Direct 64 bit  
		"R_X86_64_PC32": 2	, #  PC relative 32 bit signed 
		"R_X86_64_GOT32": 3	, #  32 bit GOT entry 
		"R_X86_64_PLT32": 4	, #  32 bit PLT address 
		"R_X86_64_COPY": 5	, #  Copy symbol at runtime 
		"R_X86_64_GLOB_DAT": 6	, #  Create GOT entry 
		"R_X86_64_JUMP_SLOT": 7	, #  Create PLT entry 
		"R_X86_64_RELATIVE": 8	, #  Adjust by program base 
		"R_X86_64_GOTPCREL": 9	, #  32 bit signed PC relative
		"R_X86_64_32": 10	, #  Direct 32 bit zero extended 
		"R_X86_64_32S": 11	, #  Direct 32 bit sign extended 
		"R_X86_64_16": 12	, #  Direct 16 bit zero extended 
		"R_X86_64_PC16": 13	, #  16 bit sign extended pc relative 
		"R_X86_64_8": 14	, #  Direct 8 bit sign extended  
		"R_X86_64_PC8": 15	, #  8 bit sign extended pc relative 
		"R_X86_64_DTPMOD64": 16	, #  ID of module containing symbol 
		"R_X86_64_DTPOFF64": 17	, #  Offset in module's TLS block 
		"R_X86_64_TPOFF64": 18	, #  Offset in initial TLS block 
		"R_X86_64_TLSGD": 19	, #  32 bit signed PC relative offset
		"R_X86_64_TLSLD": 20	, #  32 bit signed PC relative offset
		"R_X86_64_DTPOFF32": 21	, #  Offset in TLS block 
		"R_X86_64_GOTTPOFF": 22	, #  32 bit signed PC relative offset
		"R_X86_64_TPOFF32": 23	, #  Offset in initial TLS block 
		"R_X86_64_PC64": 24	, #  PC relative 64 bit 
		"R_X86_64_GOTOFF64": 25	, #  64 bit offset to GOT 
		"R_X86_64_GOTPC32": 26	, #  32 bit signed pc relative
		"R_X86_64_GOT64": 27	, #  64-bit GOT entry offset 
		"R_X86_64_GOTPCREL64": 28	, #  64-bit PC relative offset
		"R_X86_64_GOTPC64": 29	, #  64-bit PC relative offset to GOT 
		"R_X86_64_GOTPLT64": 30 	, #  like GOT64, says PLT entry needed 
		"R_X86_64_PLTOFF64": 31	, #  64-bit GOT relative offset
		"R_X86_64_SIZE32": 32	, #  Size of symbol plus 32-bit addend 
		"R_X86_64_SIZE64": 33	, #  Size of symbol plus 64-bit addend 
		"R_X86_64_GOTPC32_TLSDESC": 34	, #  GOT offset for TLS descriptor.  
		"R_X86_64_TLSDESC_CALL": 35	, #  Marker for call through TLS
		"R_X86_64_TLSDESC": 36	, #  TLS descriptor.  
		"R_X86_64_IRELATIVE": 37	, #  Adjust indirectly by program base 
		"R_X86_64_NUM": 38	, # 
		"R_MN10300_NONE": 0	, #  No reloc.  
		"R_MN10300_32": 1	, #  Direct 32 bit.  
		"R_MN10300_16": 2	, #  Direct 16 bit.  
		"R_MN10300_8": 3	, #  Direct 8 bit.  
		"R_MN10300_PCREL32": 4	, #  PC-relative 32-bit.  
		"R_MN10300_PCREL16": 5	, #  PC-relative 16-bit signed.  
		"R_MN10300_PCREL8": 6	, #  PC-relative 8-bit signed.  
		"R_MN10300_GNU_VTINHERIT": 7	, #  Ancient C++ vtable garbage... 
		"R_MN10300_GNU_VTENTRY": 8	, #  ... collection annotation.  
		"R_MN10300_24": 9	, #  Direct 24 bit.  
		"R_MN10300_GOTPC32": 10	, #  32-bit PCrel offset to GOT.  
		"R_MN10300_GOTPC16": 11	, #  16-bit PCrel offset to GOT.  
		"R_MN10300_GOTOFF32": 12	, #  32-bit offset from GOT.  
		"R_MN10300_GOTOFF24": 13	, #  24-bit offset from GOT.  
		"R_MN10300_GOTOFF16": 14	, #  16-bit offset from GOT.  
		"R_MN10300_PLT32": 15	, #  32-bit PCrel to PLT entry.  
		"R_MN10300_PLT16": 16	, #  16-bit PCrel to PLT entry.  
		"R_MN10300_GOT32": 17	, #  32-bit offset to GOT entry.  
		"R_MN10300_GOT24": 18	, #  24-bit offset to GOT entry.  
		"R_MN10300_GOT16": 19	, #  16-bit offset to GOT entry.  
		"R_MN10300_COPY": 20	, #  Copy symbol at runtime.  
		"R_MN10300_GLOB_DAT": 21	, #  Create GOT entry.  
		"R_MN10300_JMP_SLOT": 22	, #  Create PLT entry.  
		"R_MN10300_RELATIVE": 23	, #  Adjust by program base.  
		"R_MN10300_NUM": 24	, # 
		"R_M32R_NONE": 0	, #  No reloc. 
		"R_M32R_16": 1	, #  Direct 16 bit. 
		"R_M32R_32": 2	, #  Direct 32 bit. 
		"R_M32R_24": 3	, #  Direct 24 bit. 
		"R_M32R_10_PCREL": 4	, #  PC relative 10 bit shifted. 
		"R_M32R_18_PCREL": 5	, #  PC relative 18 bit shifted. 
		"R_M32R_26_PCREL": 6	, #  PC relative 26 bit shifted. 
		"R_M32R_HI16_ULO": 7	, #  High 16 bit with unsigned low. 
		"R_M32R_HI16_SLO": 8	, #  High 16 bit with signed low. 
		"R_M32R_LO16": 9	, #  Low 16 bit. 
		"R_M32R_SDA16": 10	, #  16 bit offset in SDA. 
		"R_M32R_GNU_VTINHERIT": 11	, # 
		"R_M32R_GNU_VTENTRY": 12	, # 
		"R_M32R_16_RELA": 33	, #  Direct 16 bit. 
		"R_M32R_32_RELA": 34	, #  Direct 32 bit. 
		"R_M32R_24_RELA": 35	, #  Direct 24 bit. 
		"R_M32R_10_PCREL_RELA": 36	, #  PC relative 10 bit shifted. 
		"R_M32R_18_PCREL_RELA": 37	, #  PC relative 18 bit shifted. 
		"R_M32R_26_PCREL_RELA": 38	, #  PC relative 26 bit shifted. 
		"R_M32R_HI16_ULO_RELA": 39	, #  High 16 bit with unsigned low 
		"R_M32R_HI16_SLO_RELA": 40	, #  High 16 bit with signed low 
		"R_M32R_LO16_RELA": 41	, #  Low 16 bit 
		"R_M32R_SDA16_RELA": 42	, #  16 bit offset in SDA 
		"R_M32R_RELA_GNU_VTINHERIT": 43	, # 
		"R_M32R_RELA_GNU_VTENTRY": 44	, # 
		"R_M32R_REL32": 45	, #  PC relative 32 bit.  
		"R_M32R_GOT24": 48	, #  24 bit GOT entry 
		"R_M32R_26_PLTREL": 49	, #  26 bit PC relative to PLT shifted 
		"R_M32R_COPY": 50	, #  Copy symbol at runtime 
		"R_M32R_GLOB_DAT": 51	, #  Create GOT entry 
		"R_M32R_JMP_SLOT": 52	, #  Create PLT entry 
		"R_M32R_RELATIVE": 53	, #  Adjust by program base 
		"R_M32R_GOTOFF": 54	, #  24 bit offset to GOT 
		"R_M32R_GOTPC24": 55	, #  24 bit PC relative offset to GOT 
		"R_M32R_GOT16_HI_ULO": 56	, #  High 16 bit GOT entry with unsigned
		"R_M32R_GOT16_HI_SLO": 57	, #  High 16 bit GOT entry with signed
		"R_M32R_GOT16_LO": 58	, #  Low 16 bit GOT entry 
		"R_M32R_GOTPC_HI_ULO": 59	, #  High 16 bit PC relative offset to
		"R_M32R_GOTPC_HI_SLO": 60	, #  High 16 bit PC relative offset to
		"R_M32R_GOTPC_LO": 61	, #  Low 16 bit PC relative offset to
		"R_M32R_GOTOFF_HI_ULO": 62	, #  High 16 bit offset to GOT
		"R_M32R_GOTOFF_HI_SLO": 63	, #  High 16 bit offset to GOT
		"R_M32R_GOTOFF_LO": 64	, #  Low 16 bit offset to GOT 
		"R_M32R_NUM": 256	, #  Keep this the last entry. 
		"SHF_WRITE": (1 << 0)	, #  Writable 
		"SHF_ALLOC": (1 << 1)	, #  Occupies memory during execution 
		"SHF_EXECINSTR": (1 << 2)	, #  Executable 
		"SHF_MERGE": (1 << 4)	, #  Might be merged 
		"SHF_STRINGS": (1 << 5)	, #  Contains nul-terminated strings 
		"SHF_INFO_LINK": (1 << 6)	, #  `sh_info' contains SHT index 
		"SHF_LINK_ORDER": (1 << 7)	, #  Preserve order after combining 
		"SHF_OS_NONCONFORMING": (1 << 8)	, #  Non-standard OS specific handling
		"SHF_GROUP": (1 << 9)	, #  Section is member of a group.  
		"SHF_TLS": (1 << 10)	, #  Section hold thread-local data.  
# libelf.h constants
# ELF_C
    "ELF_C_NULL": 0,
    "ELF_C_READ": 1,
    "ELF_C_WRITE": 2,
    "ELF_C_CLR": 3,
    "ELF_C_SET": 4,
    "ELF_C_FDDONE": 5,
    "ELF_C_FDREAD": 6,
    "ELF_C_RDWR": 7,
    "ELF_C_NUM": 8,
# ELF_K
    "ELF_K_NONE": 0,
    "ELF_K_AR": 1,
    "ELF_K_COFF": 2,
    "ELF_K_ELF": 3,
    "ELF_K_NUM": 4,
# ELF_T
    "ELF_T_BYTE": 00,
    "ELF_T_ADDR": 01,
    "ELF_T_DYN": 02,
    "ELF_T_EHDR": 03,
    "ELF_T_HALF": 04,
    "ELF_T_OFF": 05,
    "ELF_T_PHDR": 06,
    "ELF_T_RELA": 07,
    "ELF_T_REL": 8,
    "ELF_T_SHDR": 9,
    "ELF_T_SWORD": 10,
    "ELF_T_SYM": 11,
    "ELF_T_WORD": 12,
    "ELF_T_SXWORD": 13,
    "ELF_T_XWORD": 14,
    "ELF_T_VDEF": 15,
    "ELF_T_VNEED": 16,
    "ELF_T_NUM": 17,   
# ELF_F (ELF flags)
		"ELF_F_DIRTY": 0x1 , # 
		"ELF_F_LAYOUT": 0x4 , # 
		"ELF_F_LAYOUT_OVERLAP": 0x10000000 , # 
}

# Now lets generate constants for all 

g = globals()

for c in _consts:
  g[c] = _consts[c]

__all__ = _consts.keys()

# TODO: Move these to the macros module

#elf.h

# Macro functions 

#define ELF32_ST_VISIBILITY(o)	((o) & 0x03)
#define ELF64_ST_VISIBILITY(o)	ELF32_ST_VISIBILITY (o)

#define DT_VALTAGIDX(tag)	(DT_VALRNGHI - (tag))	# Reverse order! 

#define DT_ADDRTAGIDX(tag)	(DT_ADDRRNGHI - (tag))	# Reverse order! 

#define DT_VERSIONTAGIDX(tag)	(DT_VERNEEDNUM - (tag))	# Reverse order! 
#define DT_EXTRATAGIDX(tag)	((Elf32_Word)-((Elf32_Sword) (tag) <<1>>1)-1)

#define ELF32_M_SYM(info)	((info) >> 8)
#define ELF32_M_SIZE(info)	((unsigned char) (info))
#define ELF32_M_INFO(sym, size)	(((sym) << 8) + (unsigned char) (size))
#define ELF64_M_SYM(info)	ELF32_M_SYM (info)
#define ELF64_M_SIZE(info)	ELF32_M_SIZE (info)
#define ELF64_M_INFO(sym, size)	ELF32_M_INFO (sym, size)

#define EF_ARM_EABI_VERSION(flags)	((flags) & EF_ARM_EABIMASK)

#libelf.h
