from .. import *
from ..types import *
from ..constants import *
from ctypes import *
import os

def _class(elf):  return ord(elf_getident(elf, None).contents[EI_CLASS])

def is32(elf):   return _class(elf) == ELFCLASS32 
def is64(elf):   return _class(elf) == ELFCLASS64 

def select(elf, fname):
  if is32(elf):
    return globals()['elf32_' + fname]
  else:
    return globals()['elf64_' + fname]

def section_name(elfP, secP):
  shstrndx = c_size_t()
  r = elf_getshstrndx(elfP, byref(shstrndx))
  shdr = select(elfP, 'getshdr')(secP)
  return elf_strptr(elfP, shstrndx, shdr.contents.sh_name)

def section_type(elfP, secP):
  return select(elfP, 'getshdr')(secP).contents.sh_type

def section_link(elfP, secP):
  return select(elfP, 'getshdr')(secP).contents.sh_link

def section_hdr(elfP, secP):
  return select(elfP, 'getshdr')(secP).contents

def open_elf(fname):
  fd = os.open(fname, os.O_RDONLY)
  return elf_begin(fd, ELF_C_READ, None)

def sym_name(elf, scn, sym):
  return elf_strptr(elf, section_link(elf, scn), sym.st_name)
