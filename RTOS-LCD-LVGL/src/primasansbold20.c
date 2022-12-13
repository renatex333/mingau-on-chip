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

    /* U+002D "-" */
    0xff, 0xff, 0xc0,

    /* U+002E "." */
    0xff, 0xf0,

    /* U+002F "/" */
    0x6, 0xc, 0x38, 0x60, 0xc3, 0x86, 0xc, 0x38,
    0x60, 0xc3, 0x86, 0xc, 0x38, 0x60,

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
    0xff, 0xf0, 0x0, 0x0, 0xff, 0xf0,

    /* U+0048 "H" */
    0xf0, 0x7f, 0x83, 0xfc, 0x1f, 0xe0, 0xff, 0x7,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x7f, 0x83,
    0xfc, 0x1f, 0xe0, 0xff, 0x7, 0xf8, 0x3c,

    /* U+004B "K" */
    0xf0, 0x79, 0xe1, 0xe3, 0xc7, 0x87, 0x9e, 0xf,
    0x78, 0x1f, 0xe0, 0x3f, 0x80, 0x7f, 0x80, 0xff,
    0x81, 0xef, 0x83, 0xcf, 0x87, 0x8f, 0x8f, 0xf,
    0x9e, 0xf, 0x80,

    /* U+004D "M" */
    0xf8, 0x1f, 0xfc, 0x3f, 0xfc, 0x3f, 0xfc, 0x3f,
    0xfe, 0x7f, 0xf6, 0x6f, 0xf7, 0xef, 0xf7, 0xef,
    0xf3, 0xcf, 0xf3, 0xcf, 0xf1, 0x8f, 0xf0, 0xf,
    0xf0, 0xf, 0xf0, 0xf,

    /* U+005C "\\" */
    0xc1, 0xc1, 0x83, 0x7, 0x6, 0xc, 0x1c, 0x18,
    0x30, 0x70, 0x60, 0xc1, 0xc1, 0x83,

    /* U+0068 "h" */
    0xf0, 0x1e, 0x3, 0xc0, 0x78, 0xf, 0x79, 0xff,
    0xbe, 0x7f, 0x8f, 0xf1, 0xfe, 0x3f, 0xc7, 0xf8,
    0xff, 0x1f, 0xe3, 0xfc, 0x78,

    /* U+006B "k" */
    0xf0, 0x7, 0x80, 0x3c, 0x1, 0xe0, 0xf, 0xf,
    0x78, 0xf3, 0xce, 0x1e, 0xe0, 0xfe, 0x7, 0xf0,
    0x3f, 0xc1, 0xef, 0xf, 0x3c, 0x78, 0xf3, 0xc3,
    0xc0,

    /* U+006D "m" */
    0xf7, 0x8f, 0x3f, 0xf7, 0xef, 0x9f, 0x3f, 0xc7,
    0x8f, 0xf1, 0xe3, 0xfc, 0x78, 0xff, 0x1e, 0x3f,
    0xc7, 0x8f, 0xf1, 0xe3, 0xfc, 0x78, 0xff, 0x1e,
    0x3c
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 111, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 133, .box_w = 6, .box_h = 3, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 4, .adv_w = 122, .box_w = 4, .box_h = 3, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 6, .adv_w = 117, .box_w = 7, .box_h = 16, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 20, .adv_w = 223, .box_w = 12, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 41, .adv_w = 223, .box_w = 10, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 59, .adv_w = 223, .box_w = 11, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 79, .adv_w = 223, .box_w = 11, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 99, .adv_w = 223, .box_w = 13, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 122, .adv_w = 223, .box_w = 12, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 143, .adv_w = 223, .box_w = 12, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 164, .adv_w = 223, .box_w = 11, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 184, .adv_w = 223, .box_w = 12, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 205, .adv_w = 223, .box_w = 12, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 226, .adv_w = 128, .box_w = 4, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 232, .adv_w = 268, .box_w = 13, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 255, .adv_w = 248, .box_w = 15, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 282, .adv_w = 318, .box_w = 16, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 310, .adv_w = 117, .box_w = 7, .box_h = 16, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 324, .adv_w = 228, .box_w = 11, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 345, .adv_w = 213, .box_w = 13, .box_h = 15, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 370, .adv_w = 333, .box_w = 18, .box_h = 11, .ofs_x = 2, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0xd, 0xe, 0xf, 0x10, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x28,
    0x2b, 0x2d, 0x3c, 0x48, 0x4b, 0x4d
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 78, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 22, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Pair left and right glyphs for kerning*/
static const uint8_t kern_pair_glyph_ids[] =
{
    17, 2
};

/* Kerning between the respective left and right glyphs
 * 4.4 format which needs to scaled with `kern_scale`*/
static const int8_t kern_pair_values[] =
{
    -28
};

/*Collect the kern pair's data in one place*/
static const lv_font_fmt_txt_kern_pair_t kern_pairs =
{
    .glyph_ids = kern_pair_glyph_ids,
    .values = kern_pair_values,
    .pair_cnt = 1,
    .glyph_ids_size = 0
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
    .kern_dsc = &kern_pairs,
    .kern_scale = 16,
    .cmap_num = 1,
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
    .line_height = 17,          /*The maximum line height required by the font*/
    .base_line = 2,             /*Baseline measured from the bottom of the line*/
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

