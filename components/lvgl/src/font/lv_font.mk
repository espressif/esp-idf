CSRCS += lv_font.c
CSRCS += lv_font_fmt_txt.c
CSRCS += lv_font_loader.c

CSRCS += lv_font_dejavu_16_persian_hebrew.c
CSRCS += lv_font_montserrat_8.c
CSRCS += lv_font_montserrat_10.c
CSRCS += lv_font_montserrat_12.c
CSRCS += lv_font_montserrat_12_subpx.c
CSRCS += lv_font_montserrat_14.c
CSRCS += lv_font_montserrat_16.c
CSRCS += lv_font_montserrat_18.c
CSRCS += lv_font_montserrat_20.c
CSRCS += lv_font_montserrat_22.c
CSRCS += lv_font_montserrat_24.c
CSRCS += lv_font_montserrat_26.c
CSRCS += lv_font_montserrat_28.c
CSRCS += lv_font_montserrat_28_compressed.c
CSRCS += lv_font_montserrat_30.c
CSRCS += lv_font_montserrat_32.c
CSRCS += lv_font_montserrat_34.c
CSRCS += lv_font_montserrat_36.c
CSRCS += lv_font_montserrat_38.c
CSRCS += lv_font_montserrat_40.c
CSRCS += lv_font_montserrat_42.c
CSRCS += lv_font_montserrat_44.c
CSRCS += lv_font_montserrat_46.c
CSRCS += lv_font_montserrat_48.c
CSRCS += lv_font_simsun_16_cjk.c
CSRCS += lv_font_unscii_8.c
CSRCS += lv_font_unscii_16.c

DEPPATH += --dep-path $(LVGL_DIR)/$(LVGL_DIR_NAME)/src/font
VPATH += :$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/font

CFLAGS += "-I$(LVGL_DIR)/$(LVGL_DIR_NAME)/src/font"
