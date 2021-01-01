PROG=proj

SRCS = proj.c video_gr.c kbc.c keyboard.c mouse.c utils.c sprite.c timer.c game_engine.c game_handler.c rtc.c
# __LCOM_OPTIMIZED__ disables most of the LCF's overhead (and functionality!)
CPPFLAGS += -pedantic #-D __LCOM_OPTIMIZED__

DPADD += ${LIBLCF}
LDADD += -llcf

.include <minix.lcom.mk>

