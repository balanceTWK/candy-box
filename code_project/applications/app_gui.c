#include "app_gui.h"

#define DRV_DEBUG
#define DBG_ENABLE
#define DBG_SECTION_NAME "app.gui"
#ifdef DRV_DEBUG
#define DBG_LEVEL         DBG_LOG
#else
#define DBG_LEVEL         DBG_INFO
#endif /* DRV_DEBUG */
#include <rtdbg.h>

extern lv_theme_t * lv_theme_zen_init(uint16_t hue, lv_font_t *font);

lv_obj_t *temperature_gauge, *slider, *bar;

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void create_tab1(lv_theme_t * th, lv_obj_t *parent)
{
    lv_page_set_scrl_layout(parent, LV_LAYOUT_PRETTY);

    static lv_style_t h_style;
    lv_style_copy(&h_style, &lv_style_transp);
    h_style.body.padding.inner = LV_DPI / 4;
    h_style.body.padding.hor = LV_DPI / 4;
    h_style.body.padding.ver = LV_DPI / 6;

    lv_obj_t *h = lv_cont_create(parent, NULL);
    lv_obj_set_style(h, &h_style);
    lv_obj_set_click(h, false);
    lv_cont_set_fit(h, true, true);
    lv_cont_set_layout(h, LV_LAYOUT_COL_M);

    lv_obj_t *btn = lv_btn_create(h, NULL);
    lv_btn_set_style(btn, LV_BTN_STYLE_REL, th->btn.rel);
    lv_btn_set_style(btn, LV_BTN_STYLE_PR, th->btn.pr);
    lv_btn_set_style(btn, LV_BTN_STYLE_TGL_REL, th->btn.tgl_rel);
    lv_btn_set_style(btn, LV_BTN_STYLE_TGL_PR, th->btn.tgl_pr);
    lv_btn_set_style(btn, LV_BTN_STYLE_INA, th->btn.ina);
    lv_btn_set_fit(btn, true, true);
    lv_btn_set_toggle(btn, true);
    lv_obj_t *btn_label = lv_label_create(btn, NULL);
    lv_label_set_text(btn_label, "Button");

    btn = lv_btn_create(h, btn);
    lv_btn_toggle(btn);
    btn_label = lv_label_create(btn, NULL);
    lv_label_set_text(btn_label, "Toggled");

    btn = lv_btn_create(h, btn);
    lv_btn_set_state(btn, LV_BTN_STATE_INA);
    btn_label = lv_label_create(btn, NULL);
    lv_label_set_text(btn_label, "Inactive");

    lv_obj_t *label = lv_label_create(h, NULL);
    lv_label_set_text(label, "Primary");
    lv_obj_set_style(label, th->label.prim);

    label = lv_label_create(h, NULL);
    lv_label_set_text(label, "Secondary");
    lv_obj_set_style(label, th->label.sec);

    label = lv_label_create(h, NULL);
    lv_label_set_text(label, "Hint");
    lv_obj_set_style(label, th->label.hint);

    static const char *btnm_str[] = {"1", "2", "3", SYMBOL_OK, SYMBOL_CLOSE, ""};
    lv_obj_t *btnm = lv_btnm_create(h, NULL);
    lv_obj_set_size(btnm,LV_HOR_RES / 4, 2 * LV_DPI / 3);
    lv_btnm_set_map(btnm, btnm_str);
    lv_btnm_set_toggle(btnm, true, 3);

    h = lv_cont_create(parent, h);

    lv_obj_t *sw_h = lv_cont_create(h, NULL);
    lv_cont_set_style(sw_h, &lv_style_transp);
    lv_cont_set_fit(sw_h, false, true);
    lv_obj_set_width(sw_h, LV_HOR_RES / 4);
    lv_cont_set_layout(sw_h, LV_LAYOUT_PRETTY);

    lv_obj_t *sw = lv_sw_create(sw_h, NULL);

    sw = lv_sw_create(sw_h, sw);
    lv_sw_on(sw);

    lv_obj_t *bar = lv_bar_create(h, NULL);
    lv_bar_set_value(bar, 70);

    lv_obj_t *slider = lv_slider_create(h, NULL);
    lv_bar_set_value(slider, 70);

    lv_obj_t *line = lv_line_create(h, NULL);
    static const lv_point_t line_p[] = {{0,0},{LV_HOR_RES / 5, 0}};
    lv_line_set_points(line, line_p, 2);
    lv_line_set_style(line, th->line.decor);

    lv_obj_t *ta = lv_ta_create(h, NULL);
    lv_obj_set_style(ta, th->ta.oneline);
    lv_ta_set_text(ta, "Some text");
    lv_ta_set_one_line(ta, true);

    lv_obj_t *cb = lv_cb_create(h, NULL);

    cb = lv_cb_create(h, cb);
    lv_btn_set_state(cb, LV_BTN_STATE_TGL_REL);


    lv_obj_t *ddlist = lv_ddlist_create(h, NULL);
    lv_ddlist_open(ddlist, false);
    lv_ddlist_set_selected(ddlist, 1);

    h = lv_cont_create(parent, h);

    lv_obj_t * list = lv_list_create(h, NULL);
    lv_obj_set_size(list, LV_HOR_RES / 4, LV_VER_RES / 2);
    
    lv_obj_t *list_btn;
    list_btn = lv_list_add(list, SYMBOL_GPS,  "GPS",  NULL);
    lv_btn_set_toggle(list_btn, true); 
    
    lv_btn_set_toggle(lv_list_add(list, SYMBOL_WIFI, "WiFi", NULL), true); 
    
    lv_list_add(list, SYMBOL_GPS, "GPS", NULL);
    lv_list_add(list, SYMBOL_AUDIO, "Audio", NULL);
    lv_list_add(list, SYMBOL_VIDEO, "Video", NULL);
    lv_list_add(list, SYMBOL_CALL, "Call", NULL);
    lv_list_add(list, SYMBOL_BELL, "Bell", NULL);
    lv_list_add(list, SYMBOL_FILE, "File", NULL);
    lv_list_add(list, SYMBOL_EDIT, "Edit", NULL);
    lv_list_add(list, SYMBOL_CUT,  "Cut",  NULL);
    lv_list_add(list, SYMBOL_COPY, "Copy", NULL);

    lv_obj_t *roller = lv_roller_create(h, NULL);
    lv_roller_set_options(roller, "Monday\nTuesday\nWednesday\nThursday\nFriday\nSaturday\nSunday");
    lv_roller_set_selected(roller, 1, false);
    lv_roller_set_visible_row_count(roller, 3);
}

static lv_res_t slider_action(lv_obj_t *obj)
{
    rt_int16_t value = lv_slider_get_value(obj);
    LOG_D("slider_action value: %d",value);
    lv_bar_set_value_anim(bar, value, 1);
    lv_gauge_set_value(temperature_gauge, 0, value);
    return LV_RES_OK;
}

static void create_tab4(lv_obj_t *parent)
{

#if LV_COMPILER_NON_CONST_INIT_SUPPORTED
    static lv_color_t needle_colors[3] = {LV_COLOR_BLUE, LV_COLOR_PURPLE, LV_COLOR_TEAL};
#else
    static lv_color_t needle_colors[3] = { {{0}}, {{0}}, {{0}} };
#endif
    /*Create a styled gauge*/
    static lv_style_t style3;
    lv_style_copy(&style3, &lv_style_pretty);
    style3.body.main_color = LV_COLOR_GREEN;
    style3.body.grad_color = LV_COLOR_RED;
    style3.body.padding.hor = 6;
    style3.body.padding.inner = 10;
    style3.body.padding.ver = 8;
    style3.body.border.color = LV_COLOR_GRAY;
    style3.line.width = 2;

    temperature_gauge = lv_gauge_create(parent, NULL);
    lv_gauge_set_range(temperature_gauge, -20, 60);
    lv_obj_set_size(temperature_gauge,300,300);
    lv_gauge_set_scale(temperature_gauge, 270, 81, 9);
    lv_gauge_set_needle_count(temperature_gauge, 3, needle_colors);
    lv_gauge_set_value(temperature_gauge, 0, 20);
    lv_gauge_set_value(temperature_gauge, 1, 40);
    lv_gauge_set_value(temperature_gauge, 2, 60);

    bar = lv_bar_create(parent, NULL);
    lv_bar_set_range(bar, -20, 60);
    lv_bar_set_value(bar, 10);
    lv_obj_set_pos(bar, 700, 50);

    slider = lv_slider_create(parent, NULL);
    lv_slider_set_range(slider, -20, 60);
    lv_slider_set_value(slider, 70);
    lv_slider_set_action(slider,slider_action);
    lv_obj_set_size(slider,300,50);
    lv_obj_set_pos(slider, 700, 300);
}

void lv_gui_create(lv_theme_t *th)
{
    lv_theme_set_current(th);
    lv_obj_t *scr = lv_cont_create(NULL, NULL);
    lv_scr_load(scr);
    lv_cont_set_style(scr, th->bg);

    lv_obj_t *tv = lv_tabview_create(scr, NULL);

    lv_obj_set_size(tv, LV_HOR_RES, LV_VER_RES);
    
    lv_obj_t *tab4 = lv_tabview_add_tab(tv, "test tab4");
    
    lv_obj_t *tab1 = lv_tabview_add_tab(tv, "Tab 1");

    create_tab4(tab4);

    create_tab1(th, tab1);

}


static void lv_gui_run(void *p)
{
    lv_theme_t *th = lv_theme_zen_init(240, &lv_font_dejavu_30);
    lv_gui_create(th);
    while(1)
    {
        lv_task_handler(); 
        rt_thread_delay(RT_TICK_PER_SECOND/100);
    }
}

int rt_lvgl_init(void)
{
    rt_err_t ret = RT_EOK;
    rt_thread_t thread = RT_NULL; 

    /* init littlevGL */ 
    ret = littlevgl2rtt_init("lcd"); 
    if(ret != RT_EOK)
    {
        rt_kprintf("littlevgl2rtt_init return:%d\n",ret);
        return ret; 
    }

    /* littleGL demo gui thread */ 
    thread = rt_thread_create("lv_gui", lv_gui_run, RT_NULL, 30*1024, 8, 10); 
    if(thread == RT_NULL)
    {
        return RT_ERROR;
    }
    rt_thread_startup(thread);

    return RT_EOK; 
}

#if defined(RT_USING_DFS) && USE_LV_FILESYSTEM
#include <dfs_posix.h>

typedef  FILE * pc_file_t;

/**
 * Open a file from the PC
 * @param file_p pointer to a FILE* variable
 * @param fn name of the file.
 * @param mode element of 'fs_mode_t' enum or its 'OR' connection (e.g. FS_MODE_WR | FS_MODE_RD)
 * @return LV_FS_RES_OK: no error, the file is opened
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t pcfs_open(void * file_p, const char * fn, lv_fs_mode_t mode)
{
    errno = 0;

    const char * flags = "";

    if(mode == LV_FS_MODE_WR) flags = "wb";
    else if(mode == LV_FS_MODE_RD) flags = "rb";
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) flags = "a+";

    /*Make the path relative to the current directory (the projects root folder)*/
    char buf[256];
    rt_kprintf(buf, "./%s", fn);

    pc_file_t f = fopen(buf, flags);
    if((long int)f <= 0) return LV_FS_RES_UNKNOWN;
    else {
        fseek(f, 0, SEEK_SET);

        /* 'file_p' is pointer to a file descriptor and
         * we need to store our file descriptor here*/
        pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/
        *fp = f;
    }

    return LV_FS_RES_OK;
}


/**
 * Close an opened file
 * @param file_p pointer to a FILE* variable. (opened with lv_ufs_open)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
static lv_fs_res_t pcfs_close(void * file_p)
{
    pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/
    fclose(*fp);
    return LV_FS_RES_OK;
}

/**
 * Read data from an opened file
 * @param file_p pointer to a FILE variable.
 * @param buf pointer to a memory block where to store the read data
 * @param btr number of Bytes To Read
 * @param br the real number of read bytes (Byte Read)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
static lv_fs_res_t pcfs_read(void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/
    *br = fread(buf, 1, btr, *fp);
    return LV_FS_RES_OK;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param file_p pointer to a FILE* variable. (opened with lv_ufs_open )
 * @param pos the new position of read write pointer
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
static lv_fs_res_t pcfs_seek(void * file_p, uint32_t pos)
{
    pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/
    fseek(*fp, pos, SEEK_SET);
    return LV_FS_RES_OK;
}

/**
 * Give the position of the read write pointer
 * @param file_p pointer to a FILE* variable.
 * @param pos_p pointer to to store the result
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
static lv_fs_res_t pcfs_tell(void * file_p, uint32_t * pos_p)
{
    pc_file_t * fp = file_p;        /*Just avoid the confusing casings*/
    *pos_p = ftell(*fp);
    return LV_FS_RES_OK;
}

#endif
