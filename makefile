#CC=c:\progra~1\borland\cbuilder4\bin\bcc32
#CC=gcc -fPIC
#CPP=gcc -fPIC
#G++=gcc -fPIC
#C++=gcc -fPIC
#__CFLAGS=-fPIC -m32 -o32
#CPPFLAGS=-fPIC
PROC=ps3spu
ADDITIONAL_GOALS=config
DESCRIPTION=IBM/SONY CellBE SPE:ps3spu

!include ..\module.mak

config: $(C)ps3spu.cfg
$(C)ps3spu.cfg:    ps3spu.cfg
	$(CP) $? $@

# MAKEDEP dependency list ------------------
$(F)ana$(O)     : $(I)area.hpp $(I)auto.hpp $(I)bytes.hpp $(I)diskio.hpp    \
	          $(I)fpro.h $(I)funcs.hpp $(I)help.h $(I)ida.hpp           \
	          $(I)idp.hpp $(I)kernwin.hpp $(I)lines.hpp $(I)llong.hpp   \
	          $(I)loader.hpp $(I)nalt.hpp $(I)name.hpp $(I)netnode.hpp  \
	          $(I)offset.hpp $(I)pro.h $(I)queue.hpp $(I)segment.hpp    \
	          $(I)ua.hpp $(I)xref.hpp ../idaidp.hpp ana.cpp ps3spu.hpp     \
	          ins.hpp
$(F)emu$(O)     : $(I)area.hpp $(I)auto.hpp $(I)bytes.hpp $(I)diskio.hpp    \
	          $(I)fpro.h $(I)funcs.hpp $(I)help.h $(I)ida.hpp           \
	          $(I)idp.hpp $(I)kernwin.hpp $(I)lines.hpp $(I)llong.hpp   \
	          $(I)loader.hpp $(I)nalt.hpp $(I)name.hpp $(I)netnode.hpp  \
	          $(I)offset.hpp $(I)pro.h $(I)queue.hpp $(I)segment.hpp    \
	          $(I)ua.hpp $(I)xref.hpp ../idaidp.hpp emu.cpp ps3spu.hpp     \
	          ins.hpp
$(F)ins$(O)     : $(I)area.hpp $(I)auto.hpp $(I)bytes.hpp $(I)diskio.hpp    \
	          $(I)fpro.h $(I)funcs.hpp $(I)help.h $(I)ida.hpp           \
	          $(I)idp.hpp $(I)kernwin.hpp $(I)lines.hpp $(I)llong.hpp   \
	          $(I)loader.hpp $(I)nalt.hpp $(I)name.hpp $(I)netnode.hpp  \
	          $(I)offset.hpp $(I)pro.h $(I)queue.hpp $(I)segment.hpp    \
	          $(I)ua.hpp $(I)xref.hpp ../idaidp.hpp ps3spu.hpp ins.cpp     \
	          ins.hpp
$(F)out$(O)     : $(I)area.hpp $(I)auto.hpp $(I)bytes.hpp $(I)diskio.hpp    \
	          $(I)fpro.h $(I)funcs.hpp $(I)help.h $(I)ida.hpp           \
	          $(I)idp.hpp $(I)kernwin.hpp $(I)lines.hpp $(I)llong.hpp   \
	          $(I)loader.hpp $(I)nalt.hpp $(I)name.hpp $(I)netnode.hpp  \
	          $(I)offset.hpp $(I)pro.h $(I)queue.hpp $(I)segment.hpp    \
	          $(I)ua.hpp $(I)xref.hpp ../idaidp.hpp ps3spu.hpp ins.hpp     \
	          out.cpp
$(F)reg$(O)     : $(I)area.hpp $(I)auto.hpp $(I)bytes.hpp $(I)diskio.hpp    \
	          $(I)entry.hpp $(I)fpro.h $(I)funcs.hpp $(I)help.h         \
	          $(I)ida.hpp $(I)idp.hpp $(I)kernwin.hpp $(I)lines.hpp     \
	          $(I)llong.hpp $(I)loader.hpp $(I)nalt.hpp $(I)name.hpp    \
	          $(I)netnode.hpp $(I)offset.hpp $(I)pro.h $(I)queue.hpp    \
	          $(I)segment.hpp $(I)srarea.hpp $(I)ua.hpp $(I)xref.hpp    \
	          ../idaidp.hpp ../iocommon.cpp ps3spu.hpp ins.hpp reg.cpp
