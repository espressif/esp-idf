def ELF32_R_SYM(i):
  if type(i) == str:
    assert(len(i) == 1) # Single char
    i = ord(i)
  return i >> 8

def ELF32_R_TYPE(i):
  if type(i) == str:
    assert(len(i) == 1) # Single char
    i = ord(i)
  return i % 256 # Lowest 8 bits

def ELF32_R_INFO(sym, typ):
  return (((sym) << 8) + typ % 256)

def ELF64_R_SYM(i):
  if type(i) == str:
    assert(len(i) == 1) # Single char
    i = ord(i)
  return i >> 32

def ELF64_R_TYPE(i):
  if type(i) == str:
    assert(len(i) == 1) # Single char
    i = ord(i)
  return i & 0xffffffffL

def ELF64_R_INFO(sym, typ):
  return ((sym << 32) + (typ & 0xffffffffL))

# symbol st_info

def ELF32_ST_BIND(val):
  if type(val) == str:
    assert(len(val) == 1) # Single char
    val = ord(val)
  return val >> 4

def ELF32_ST_TYPE(val):
  if type(val) == str:
    assert(len(val) == 1) # Single char
    val = ord(val)
  return val & 0xf

def ELF32_ST_INFO(bind, type):
  return (((bind) << 4) + ((type) & 0xf))

def ELF64_ST_BIND(val):
  return ELF32_ST_BIND(val)

def ELF64_ST_TYPE(val):
  return ELF32_ST_TYPE(val)

def ELF64_ST_INFO(bind, type):
  return ELF32_ST_INFO(bind, type)
