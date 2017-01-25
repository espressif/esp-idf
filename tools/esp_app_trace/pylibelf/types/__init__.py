from ctypes import *
from ..constants import EI_NIDENT

# Obtained from /usr/lib/elf.h

# Type for a 16-bit quantity.  
Elf32_Half = c_uint16 
Elf64_Half = c_uint16 

# Types for signed and unsigned 32-bit quantities.  
Elf32_Word = c_uint32 
Elf32_Sword = c_int32 
Elf64_Word = c_uint32 
Elf64_Sword = c_int32 

# Types for signed and unsigned 64-bit quantities.  
Elf32_Xword = c_uint64 
Elf32_Sxword = c_int64 
Elf64_Xword = c_uint64 
Elf64_Sxword = c_int64 

# Type of addresses.  
Elf32_Addr = c_uint32 
Elf64_Addr = c_uint64 

# Type of file offsets.  
Elf32_Off = c_uint32 
Elf64_Off = c_uint64 

# Type for section indices, which are 16-bit quantities.  
Elf32_Section = c_uint16 
Elf64_Section = c_uint16 

# Type for version symbol information.  
Elf32_Versym = Elf32_Half 
Elf64_Versym = Elf64_Half 

# The ELF file header.  This appears at the start of every ELF file.  

Elf_IdentT = c_char * EI_NIDENT

Elf_Cmd = c_int

class _ElfStructure(Structure):
  def __str__(self):
    return self.__class__.__name__ + '(' + \
      ','.join([field[0] + '=' + str(getattr(self, field[0])) for field in self._fields_])  + ')'

class _ElfUnion(Union):
  def __str__(self):
    return self.__class__.__name__ + '(' + \
      ','.join([field[0] + '=' + str(getattr(self, field[0])) for field in self._fields_])  + ')'

# Libelf opaque handles
class Elf(_ElfStructure):
  _fields_ = [] 
class Elf_Scn(_ElfStructure):
  _fields_ = [] 

class Elf_Data(_ElfStructure):
  _fields_ = [
    ('d_buf', c_void_p),
    ('d_type', c_int),
    ('d_size', c_size_t),
    ('d_off', c_size_t),
    ('d_align', c_size_t),
    ('d_version', c_uint)
  ]

ElfP = POINTER(Elf)
Elf_ScnP = POINTER(Elf_Scn)
Elf_DataP = POINTER(Elf_Data)

class Elf32_Ehdr(_ElfStructure):
  _fields_ = [
    ('e_ident',   Elf_IdentT ), # Magic number and other info 
    ('e_type',   Elf32_Half ),     # Object file type 
    ('e_machine',   Elf32_Half ),    # Architecture 
    ('e_version',   Elf32_Word ),    # Object file version 
    ('e_entry',   Elf32_Addr ),    # Entry point virtual address 
    ('e_phoff',   Elf32_Off),    # Program header table file offset 
    ('e_shoff',   Elf32_Off),    # Section header table file offset 
    ('e_flags',   Elf32_Word ),    # Processor-specific flags 
    ('e_ehsize',   Elf32_Half ),   # ELF header size in bytes 
    ('e_phentsize',   Elf32_Half ),    # Program header table entry size 
    ('e_phnum',   Elf32_Half ),    # Program header table entry count 
    ('e_shentsize',   Elf32_Half ),    # Section header table entry size 
    ('e_shnum',   Elf32_Half ),    # Section header table entry count 
    ('e_shstrndx',   Elf32_Half ),   # Section header string table index 
  ]

class Elf64_Ehdr(_ElfStructure):
  _fields_ = [
    ('e_ident',   Elf_IdentT ), # Magic number and other info 
    ('e_type',   Elf64_Half ),     # Object file type 
    ('e_machine',   Elf64_Half ),    # Architecture 
    ('e_version',   Elf64_Word ),    # Object file version 
    ('e_entry',   Elf64_Addr ),    # Entry point virtual address 
    ('e_phoff',   Elf64_Off),    # Program header table file offset 
    ('e_shoff',   Elf64_Off),    # Section header table file offset 
    ('e_flags',   Elf64_Word ),    # Processor-specific flags 
    ('e_ehsize',   Elf64_Half ),   # ELF header size in bytes 
    ('e_phentsize',   Elf64_Half ),    # Program header table entry size 
    ('e_phnum',   Elf64_Half ),    # Program header table entry count 
    ('e_shentsize',   Elf64_Half ),    # Section header table entry size 
    ('e_shnum',   Elf64_Half ),    # Section header table entry count 
    ('e_shstrndx',   Elf64_Half ),   # Section header string table index 
 ] 

class Elf32_Shdr(_ElfStructure):
  _fields_ = [
		('sh_name', Elf32_Word), # Section name (string tbl index) 
		('sh_type', Elf32_Word), # Section type 
		('sh_flags', Elf32_Word), # Section flags 
		('sh_addr', Elf32_Addr), # Section virtual addr at execution 
		('sh_offset', Elf32_Off), # Section file offset 
		('sh_size', Elf32_Word), # Section size in bytes 
		('sh_link', Elf32_Word), # Link to another section 
		('sh_info', Elf32_Word), # Additional section information 
		('sh_addralign', Elf32_Word), # Section alignment 
		('sh_entsize', Elf32_Word), # Entry size if section holds table 
 ] 

class Elf64_Shdr(_ElfStructure):
  _fields_ = [
		('sh_name', Elf64_Word), # Section name (string tbl index) 
		('sh_type', Elf64_Word), # Section type 
		('sh_flags', Elf64_Xword), # Section flags 
		('sh_addr', Elf64_Addr), # Section virtual addr at execution 
		('sh_offset', Elf64_Off), # Section file offset 
		('sh_size', Elf64_Xword), # Section size in bytes 
		('sh_link', Elf64_Word), # Link to another section 
		('sh_info', Elf64_Word), # Additional section information 
		('sh_addralign', Elf64_Xword), # Section alignment 
		('sh_entsize', Elf64_Xword), # Entry size if section holds table 
 ] 

class Elf32_Phdr(_ElfStructure):
  _fields_ = [
		('p_type', Elf32_Word), # Segment type 
		('p_offset', Elf32_Off), # Segment file offset 
		('p_vaddr', Elf32_Addr), # Segment virtual address 
		('p_paddr', Elf32_Addr), # Segment physical address 
		('p_filesz', Elf32_Word), # Segment size in file 
		('p_memsz', Elf32_Word), # Segment size in memory 
		('p_flags', Elf32_Word), # Segment flags 
		('p_align', Elf32_Word), # Segment alignment 
  ]

class Elf64_Phdr(_ElfStructure):
  _fields_ = [
		('p_type', Elf64_Word), # Segment type 
		('p_flags', Elf64_Word), # Segment flags 
		('p_offset', Elf64_Off), # Segment file offset 
		('p_vaddr', Elf64_Addr), # Segment virtual address 
		('p_paddr', Elf64_Addr), # Segment physical address 
		('p_filesz', Elf64_Xword), # Segment size in file 
		('p_memsz', Elf64_Xword), # Segment size in memory 
		('p_align', Elf64_Xword), # Segment alignment 
  ]

# /* Symbol table entry.  */
class Elf32_Sym(_ElfStructure):
  _fields_ = [
		('st_name', Elf32_Word), # Symbol name (string tbl index) 
		('st_value', Elf32_Addr), # Symbol value 
		('st_size', Elf32_Word), # Symbol size 
    ('st_info', c_char),    # Symbol type and binding 
    ('st_other', c_char),   # Symbol visibility 
		('st_shndx', Elf32_Section), # Section index 
  ]

class Elf64_Sym(_ElfStructure):
  _fields_ = [
		('st_name', Elf64_Word), # Symbol name (string tbl index) 
    ('st_info', c_char), # Symbol type and binding 
    ('st_other', c_char), # Symbol visibility 
		('st_shndx', Elf64_Section), # Section index 
		('st_value', Elf64_Addr), # Symbol value 
		('st_size', Elf64_Xword), # Symbol size 
  ]

#/* The syminfo section if available contains additional information about
#   every dynamic symbol.  */

class Elf32_Syminfo(_ElfStructure):
  _fields_ = [
		('si_boundto', Elf32_Half), # Direct bindings, symbol bound to 
		('si_flags', Elf32_Half), # Per symbol flags 
  ]

class Elf64_Syminfo(_ElfStructure):
  _fields_ = [
		('si_boundto', Elf64_Half), # Direct bindings, symbol bound to 
		('si_flags', Elf64_Half), # Per symbol flags 
  ]

# /* Relocation table entry without addend (in section of type SHT_REL).  */

class Elf32_Rel(_ElfStructure):
  _fields_ = [
		('r_offset', Elf32_Addr), # Address 
		('r_info', Elf32_Word), # Relocation type and symbol index 
  ]

class Elf64_Rel(_ElfStructure):
  _fields_ = [
		('r_offset', Elf64_Addr), # Address 
		('r_info', Elf64_Xword), # Relocation type and symbol index 
  ]

# # Relocation table entry with addend (in section of type SHT_RELA).  

class Elf32_Rela(_ElfStructure):
  _fields_ = [
		('r_offset', Elf32_Addr), # Address 
		('r_info', Elf32_Word), # Relocation type and symbol index 
		('r_addend', Elf32_Sword), # Addend 
  ]

class Elf64_Rela(_ElfStructure):
  _fields_ = [
		('r_offset', Elf64_Addr), # Address 
		('r_info', Elf64_Xword), # Relocation type and symbol index 
		('r_addend', Elf64_Sxword), # Addend 
  ]

time_t = c_int64
uid_t = c_int32
gid_t = c_int32
mode_t = c_int32
off_t = c_int64

class Elf_Arhdr(_ElfStructure):
  _fields_ = [
    ('ar_name', c_char_p), 
    ('ar_date', time_t), 
    ('ar_uid', uid_t), 
    ('ar_gid', gid_t), 
    ('ar_mode', mode_t), 
    ('ar_size', off_t), 
    ('ar_fmag', POINTER(c_char)), 
  ]

class _Elf64_DynUnion(_ElfUnion):
  _fields_ = [
    ('d_val', Elf64_Xword),
    ('d_ptr', Elf64_Addr),
  ]

class Elf64_Dyn(_ElfStructure):
  _fields_ = [
    ('d_tag', Elf64_Xword), 
    ('d_un', _Elf64_DynUnion),
  ]

# GNU Extensions
class Elf64_Verneed(_ElfStructure):
  _fields_ = [
    ('vn_version', Elf64_Half), 
    ('vn_cnt', Elf64_Half), 
    ('vn_file', Elf64_Word), 
    ('vn_aux', Elf64_Word), 
    ('vn_next', Elf64_Word), 
  ]

class Elf64_Vernaux(_ElfStructure):
  _fields_ = [
    ('vna_hash', Elf64_Word), 
    ('vna_flags', Elf64_Half), 
    ('vna_other', Elf64_Half), 
    ('vna_name', Elf64_Word), 
    ('vna_next', Elf64_Word), 
  ]
