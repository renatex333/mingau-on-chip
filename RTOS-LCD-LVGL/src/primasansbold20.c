/*******************************************************************************
 * Size: 20 px
 * Bpp: 1
 * Opts: 
 ******************************************************************************/
#define LV_LVGL_H_INCLUDE_SIMPLE
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef PRIMASANSBOLD20
#define PRIMASANSBOLD20 1
#endif

#if PRIMASANSBOLD20

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0030 "0" */
    0x1f, 0x83, 0xfc, 0x79, 0xe7, 0xe, 0xf0, 0xff,
    0xf, 0xf0, 0xff, 0xf, 0xf0, 0xff, 0xf, 0x70,
    0xe7, 0x9e, 0x3f, 0xc1, 0xf8,

    /* U+0031 "1" */
    0x7e, 0x3f, 0x8f, 0xe0, 0x78, 0x1e, 0x7, 0x81,
    0xe0, 0x78, 0x1e, 0x7, 0x81, 0xe0, 0x78, 0xff,
    0xff, 0xf0,

    /* U+0032 "2" */
    0x7f, 0x1f, 0xfb, 0xf, 0x80, 0xf0, 0x1e, 0x3,
    0xc0, 0xf0, 0x3e, 0xf, 0x83, 0xc0, 0xf0, 0x3c,
    0xf, 0xff, 0xff, 0xc0,

    /* U+0033 "3" */
    0x7f, 0x8f, 0xf8, 0xf, 0x80, 0xf0, 0x3e, 0x3f,
    0x87, 0xc0, 0x1e, 0x1, 0xe0, 0x3c, 0x7, 0xe1,
    0xff, 0xfd, 0xfe, 0x0,

    /* U+0034 "4" */
    0x3, 0xe0, 0x3f, 0x1, 0xf8, 0x1f, 0xc1, 0xde,
    0xc, 0xf0, 0xe7, 0x8e, 0x3c, 0xe1, 0xe7, 0xff,
    0xff, 0xfe, 0x3, 0xc0, 0x1e, 0x0, 0xf0,

    /* U+0035 "5" */
    0x7f, 0xe7, 0xfe, 0x70, 0x7, 0x0, 0x7f, 0x87,
    0xfe, 0x41, 0xe0, 0xf, 0x0, 0xf0, 0xf, 0x80,
    0xfe, 0x1e, 0xff, 0xe7, 0xf8,

    /* U+0036 "6" */
    0xf, 0xe3, 0xfe, 0x78, 0x27, 0x0, 0xff, 0x8f,
    0xfe, 0xf9, 0xef, 0xf, 0xf0, 0xff, 0xf, 0x70,
    0xf7, 0x9e, 0x3f, 0xe1, 0xf8,

    /* U+0037 "7" */
    0xff, 0xff, 0xfc, 0x7, 0x81, 0xe0, 0x3c, 0xf,
    0x1, 0xe0, 0x38, 0xf, 0x1, 0xc0, 0x78, 0xe,
    0x3, 0xc0, 0x78, 0x0,

    /* U+0038 "8" */
    0x3f, 0x87, 0xfc, 0xf1, 0xef, 0x1e, 0xf1, 0xe7,
    0xfc, 0x3f, 0xc7, 0x9e, 0xf0, 0xff, 0xf, 0xf0,
    0xff, 0x9f, 0x7f, 0xe1, 0xf8,

    /* U+0039 "9" */
    0x1f, 0x87, 0xfc, 0x79, 0xef, 0xe, 0xf0, 0xff,
    0xf, 0xf9, 0xf7, 0xff, 0x1f, 0xf0, 0xf, 0x0,
    0xe4, 0x1e, 0x7f, 0xc7, 0xf0,

    /* U+003A ":" */
    0xff, 0xf0, 0x0, 0x0, 0xff, 0xf0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 111, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 223, .box_w = 12, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 22, .adv_w = 223, .box_w = 10, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 40, .adv_w = 223, .box_w = 11, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 60, .adv_w = 223, .box_w = 11, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 80, .adv_w = 223, .box_w = 13, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 103, .adv_w = 223, .box_w = 12, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 124, .adv_w = 223, .box_w = 12, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 145, .adv_w = 223, .box_w = 11, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 165, .adv_w = 223, .box_w = 12, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 186, .adv_w = 223, .box_w = 12, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 207, .adv_w = 128, .box_w = 4, .box_h = 11, .ofs_x = 2, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/



/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 1, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 48, .range_length = 11, .glyph_id_start = 2,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 2,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t primasansbold20 = {
#else
lv_font_t primasansbold20 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 14,          /*The maximum line height required by the font*/
    .base_line = 0,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 3,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if PRIMASANSBOLD20*/

