from types import *
from constants import *
from ctypes import *

lelf=CDLL("libelf.so.1")

__all__ = []

all_objs = []

class ElfError(Exception):
  def __init__(self, msg):
    self.msg = msg
    self.errno = elf_errno()
    self.elfmsg = elf_errmsg(self.errno)

  def __str__(self):
    return "ElfError(%d, %s): %s" % (self.errno, self.elfmsg, self.msg)

__all__.append("ElfError")

def nonNullDec(f):
  def decorated(*args):
    res = f(*args)
    try:
      a = res.contents
      all_objs.append(res)
    except ValueError: # NULL
      raise ElfError(f.__name__ + " returned NULL")
    return res
  return decorated

def nonNegDec(f):
  def decorated(*args):
    res = f(*args)
    if 0 > res:
      raise ElfError(f.__name__ + " returned %d" % (res,))
    return res
  return decorated

def badValDec(badVal):
  def decorator(f):
    def decorated(*args):
      res = f(*args)
      if res == badVal:
        raise ElfError(f.__name__ + " returned %s" % (str(res),))
      return res
    return decorated
  return decorator

def define(f, argtypes, restype, err_decorator = None):
  f.argtypes = argtypes
  f.restype = restype
  name = f.__name__
  __all__.append(name)

  if (err_decorator != None):
    f = err_decorator(f)

  globals()[name] = f

define(lelf.elf_version, [ c_int ], c_int )

if (elf_version(EV_CURRENT) == EV_NONE):
  raise Exception("Version mismatch")

off_t = c_size_t # TODO(dbounov): Figure out actual off_t type

define(lelf.elf_begin, [ c_int, Elf_Cmd, ElfP ], ElfP)
define(lelf.elf_getident, [ ElfP, POINTER(c_int) ], POINTER(Elf_IdentT), nonNullDec)
define(lelf.elf_end, [ ElfP ], c_int, nonNegDec )
define(lelf.elf_cntl, [ ElfP, c_int ], c_int, nonNegDec)
define(lelf.elf_errmsg, [ c_int ], c_char_p)
define(lelf.elf_errno, [ ], c_int)
define(lelf.elf_fill, [ c_int ], None)
define(lelf.elf_flagdata, [ Elf_DataP, c_int, c_uint ], c_uint)
define(lelf.elf_flagehdr, [ ElfP, c_int, c_uint ], c_uint)
define(lelf.elf_flagelf,  [ ElfP, c_int, c_uint ], c_uint)
define(lelf.elf_flagphdr, [ ElfP, c_int, c_uint ], c_uint)
define(lelf.elf_flagscn,  [ Elf_ScnP, c_int, c_uint ], c_uint)
define(lelf.elf_flagshdr, [ Elf_ScnP, c_int, c_uint ], c_uint)
define(lelf.elf_getarhdr, [ ElfP ], POINTER(Elf_Arhdr))
#define(lelf.elf_getarsym, [ ], )
define(lelf.elf_getbase, [ ElfP ], off_t, nonNegDec)
define(lelf.elf_getdata, [ Elf_ScnP, Elf_DataP ], Elf_DataP)
define(lelf.elf_getscn, [ ElfP, c_size_t ], Elf_ScnP, nonNullDec )
define(lelf.elf_getshnum, [ ElfP, POINTER(c_size_t) ], c_int, nonNegDec )
define(lelf.elf_getshstrndx,  [ ElfP, POINTER(c_size_t) ], c_int, nonNegDec )
define(lelf.elf_hash, [ c_char_p ], c_ulong)
define(lelf.elf_kind, [ ElfP ], c_int )
define(lelf.elf_memory, [ POINTER(c_char), c_size_t ], ElfP, nonNullDec)
define(lelf.elf_ndxscn, [ Elf_ScnP ], c_size_t, badValDec(SHN_UNDEF))
define(lelf.elf_newdata, [ Elf_ScnP ], Elf_DataP, nonNullDec) 
define(lelf.elf_newscn, [ ElfP ], Elf_ScnP, nonNullDec)
#define(lelf.elf_next, [ ], )
define(lelf.elf_nextscn, [ ElfP, Elf_ScnP ], Elf_ScnP)
#define(lelf.elf_rand, [ ], )
define(lelf.elf_rawdata, [ Elf_ScnP, Elf_DataP ], Elf_DataP)
#define(lelf.elf_rawfile, [ ], )
define(lelf.elf_strptr, [ ElfP, c_size_t, c_size_t ], c_char_p)
define(lelf.elf_update, [ ElfP, c_int], off_t, nonNegDec)

define(lelf.elf32_checksum, [ ElfP ], c_long)
define(lelf.elf32_fsize, [ c_int, c_size_t, c_uint ], c_size_t, nonNegDec)
define(lelf.elf32_getehdr, [ ElfP ], POINTER(Elf32_Ehdr), nonNullDec)
define(lelf.elf32_getphdr, [ ElfP ], POINTER(Elf32_Phdr), nonNullDec)
define(lelf.elf32_getshdr, [ Elf_ScnP ], POINTER(Elf32_Shdr), nonNullDec)
define(lelf.elf32_newehdr, [ ElfP ], POINTER(Elf32_Ehdr), nonNullDec)
define(lelf.elf32_newphdr, [ ElfP, c_size_t ], POINTER(Elf32_Phdr), nonNullDec)
define(lelf.elf32_xlatetof, [ Elf_DataP, Elf_DataP, c_uint ], Elf_DataP, nonNullDec)
define(lelf.elf32_xlatetom, [ Elf_DataP, Elf_DataP, c_uint ], Elf_DataP, nonNullDec)


define(lelf.elf64_checksum, [ ElfP ], c_long )
define(lelf.elf64_fsize, [ c_int, c_size_t, c_uint ], c_size_t, nonNegDec)
define(lelf.elf64_getehdr,[ ElfP ], POINTER(Elf64_Ehdr), nonNullDec)
define(lelf.elf64_getphdr, [ ElfP ], POINTER(Elf64_Phdr), nonNullDec)
define(lelf.elf64_getshdr, [ Elf_ScnP ], POINTER(Elf64_Shdr), nonNullDec)
define(lelf.elf64_newehdr, [ ElfP ], POINTER(Elf64_Ehdr), nonNullDec)
define(lelf.elf64_newphdr, [ ElfP, c_size_t ], POINTER(Elf64_Phdr), nonNullDec)
define(lelf.elf64_xlatetof, [ Elf_DataP, Elf_DataP, c_uint ], Elf_DataP, nonNullDec)
define(lelf.elf64_xlatetom, [ Elf_DataP, Elf_DataP, c_uint ], Elf_DataP, nonNullDec)

# NOTE(dbounov): Ignoring gelf functions for now

#define(lelf.gelf_checksum, [ ], )
#define(lelf.gelf_fsize, [ ], )
#define(lelf.gelf_getcap, [ ], )
#define(lelf.gelf_getclass, [ ], )
#define(lelf.gelf_getdyn, [ ], )
#define(lelf.gelf_getehdr, [ ], )
#define(lelf.gelf_getmove, [ ], )
#define(lelf.gelf_getphdr, [ ], )
#define(lelf.gelf_getrel, [ ], )
#define(lelf.gelf_getrela, [ ], )
#define(lelf.gelf_getshdr, [ ], )
#define(lelf.gelf_getsym, [ ], )
#define(lelf.gelf_getsyminfo, [ ], )
#define(lelf.gelf_getsymshndx, [ ], )
#define(lelf.gelf_newehdr, [ ], )
#define(lelf.gelf_newphdr, [ ], )
#define(lelf.gelf_update_cap, [ ], )
#define(lelf.gelf_update_dyn, [ ], )
#define(lelf.gelf_update_ehdr, [ ], )
#define(lelf.gelf_update_move, [ ], )
#define(lelf.gelf_update_phdr, [ ], )
#define(lelf.gelf_update_rel, [ ], )
#define(lelf.gelf_update_rela, [ ], )
#define(lelf.gelf_update_shdr, [ ], )
#define(lelf.gelf_update_sym, [ ], )
#define(lelf.gelf_update_symshndx, [ ], )
#define(lelf.gelf_update_syminfo, [ ], )
#define(lelf.gelf_xlatetof, [ ], )
#define(lelf.gelf_xlatetom, [ ], )
#define(lelf.nlist, [ ], )
