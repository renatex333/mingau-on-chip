/*******************************************************************************
 * Size: 20 px
 * Bpp: 1
 * Opts: 
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#ifndef PRIMASANS20
#define PRIMASANS20 1
#endif

#if PRIMASANS20

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0030 "0" */
    0x1e, 0xc, 0xc6, 0x1b, 0x3, 0xc0, 0xf0, 0x3c,
    0xf, 0x3, 0xc0, 0xf0, 0x3c, 0xd, 0x86, 0x33,
    0x7, 0x80,

    /* U+0031 "1" */
    0x7c, 0x7e, 0x23, 0x1, 0x80, 0xc0, 0x60, 0x30,
    0x18, 0xc, 0x6, 0x3, 0x1, 0x80, 0xc3, 0xfc,

    /* U+0032 "2" */
    0xfe, 0x61, 0x80, 0x60, 0x30, 0x18, 0x1c, 0xc,
    0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0x7, 0xfc,

    /* U+0033 "3" */
    0x7f, 0x10, 0x60, 0xc, 0x3, 0x0, 0xc0, 0x61,
    0xe0, 0x6, 0x0, 0xc0, 0x30, 0xc, 0x3, 0xc1,
    0xbf, 0xc0,

    /* U+0034 "4" */
    0x3, 0x80, 0xf0, 0x16, 0x6, 0xc1, 0x98, 0x23,
    0xc, 0x63, 0xc, 0x41, 0x9f, 0xfc, 0x6, 0x0,
    0xc0, 0x18, 0x3, 0x0,

    /* U+0035 "5" */
    0x7f, 0x98, 0x6, 0x1, 0x80, 0x60, 0x1f, 0x84,
    0x38, 0x7, 0x0, 0xc0, 0x30, 0xc, 0x7, 0xc3,
    0xbf, 0x80,

    /* U+0036 "6" */
    0x1f, 0x8c, 0x26, 0x1, 0x0, 0xc0, 0x37, 0xcf,
    0x1b, 0x83, 0xc0, 0xf0, 0x3c, 0xd, 0x83, 0x31,
    0x87, 0xc0,

    /* U+0037 "7" */
    0xff, 0x80, 0xc0, 0xc0, 0x60, 0x70, 0x30, 0x18,
    0x18, 0xc, 0x6, 0x6, 0x3, 0x3, 0x1, 0x80,

    /* U+0038 "8" */
    0x3f, 0x18, 0x6c, 0xf, 0x3, 0xc0, 0xd8, 0x63,
    0xe1, 0x86, 0xc0, 0xf0, 0x3c, 0xf, 0x3, 0x61,
    0x8f, 0xc0,

    /* U+0039 "9" */
    0x3e, 0x18, 0xcc, 0x1b, 0x3, 0xc0, 0xf0, 0x3c,
    0x1d, 0x8f, 0x3e, 0xc0, 0x30, 0x8, 0x6, 0x43,
    0x1f, 0x80,

    /* U+003A ":" */
    0xc0, 0x0, 0xc0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 102, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 204, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 19, .adv_w = 204, .box_w = 9, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 35, .adv_w = 204, .box_w = 9, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 51, .adv_w = 204, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 69, .adv_w = 204, .box_w = 11, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 89, .adv_w = 204, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 107, .adv_w = 204, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 125, .adv_w = 204, .box_w = 9, .box_h = 14, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 141, .adv_w = 204, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 159, .adv_w = 204, .box_w = 10, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 177, .adv_w = 108, .box_w = 2, .box_h = 9, .ofs_x = 2, .ofs_y = 0}
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
const lv_font_t primasans20 = {
#else
lv_font_t primasans20 = {
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
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if PRIMASANS20*/

