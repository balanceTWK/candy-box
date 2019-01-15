#include "littlevgl2rtt.h" 
#include "lvgl.h" 
#include <rtthread.h> 
#include <rtdevice.h>  

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

lv_obj_t *gauge1, *gauge2, *gauge3, *slider, *bar;

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

static void create_tab2(lv_theme_t * th, lv_obj_t *parent)
{
    lv_coord_t w = lv_page_get_scrl_width(parent);

    lv_obj_t *chart = lv_chart_create(parent, NULL);
    lv_obj_set_size(chart, w / 3, LV_VER_RES / 3);
    lv_obj_set_pos(chart, LV_DPI / 10, LV_DPI / 10);
    lv_chart_series_t * s1 = lv_chart_add_series(chart, LV_COLOR_RED);
    lv_chart_set_next(chart, s1, 30);
    lv_chart_set_next(chart, s1, 20);
    lv_chart_set_next(chart, s1, 10);
    lv_chart_set_next(chart, s1, 12);
    lv_chart_set_next(chart, s1, 20);
    lv_chart_set_next(chart, s1, 27);
    lv_chart_set_next(chart, s1, 35);
    lv_chart_set_next(chart, s1, 55);
    lv_chart_set_next(chart, s1, 70);
    lv_chart_set_next(chart, s1, 75);

    lv_obj_t *gauge = lv_gauge_create(parent, NULL);
    lv_gauge_set_value(gauge, 0, 40);
    lv_obj_set_size(gauge, w / 4, w / 4);
    lv_obj_align(gauge, chart, LV_ALIGN_OUT_BOTTOM_LEFT, 0, LV_DPI / 4);

    lv_obj_t *ta = lv_ta_create(parent, NULL);
    lv_obj_set_size(ta, w / 3, LV_VER_RES / 4);
    lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_RIGHT, -LV_DPI / 10, LV_DPI / 10);
    lv_ta_set_cursor_type(ta, LV_CURSOR_BLOCK);

    lv_obj_t *kb = lv_kb_create(parent, NULL);
    lv_obj_set_size(kb, 2 * w / 3, LV_VER_RES / 3);
    lv_obj_align(kb, ta, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, LV_DPI);
    lv_kb_set_ta(kb, ta);
}

static void create_tab3(lv_theme_t * th, lv_obj_t *parent)
{
    /*Create a Window*/
    lv_obj_t * win = lv_win_create(parent, NULL);
    lv_win_add_btn(win, SYMBOL_CLOSE, lv_win_close_action);
    lv_win_add_btn(win, SYMBOL_DOWN, NULL);
    lv_obj_set_size(win, LV_HOR_RES / 2, LV_VER_RES / 2);
    lv_obj_set_pos(win, LV_DPI / 20, LV_DPI / 20);
    lv_obj_set_top(win, true);

    /*Create a Page*/
    lv_obj_t * page = lv_page_create(parent, NULL);
    lv_obj_set_size(page, LV_HOR_RES / 3, LV_VER_RES / 2);
    lv_obj_set_top(page, true);
    lv_obj_align(page, win, LV_ALIGN_IN_TOP_RIGHT,  LV_DPI, LV_DPI);

    /*Create a Message box*/
    static const char * mbox_btn_map[] = {"\211", "\222Got it!", "\211", ""};
    lv_obj_t * mbox = lv_mbox_create(parent, NULL);
    lv_mbox_set_text(mbox, "Click on the window or the page to bring it to the foreground");
    lv_mbox_add_btns(mbox, mbox_btn_map, NULL);
    lv_obj_set_top(mbox, true);
}

////////////////////////////////////////////////////////////////////////////////


static lv_res_t slider_action(lv_obj_t *obj)
{
    rt_int16_t value = lv_slider_get_value(obj);
    LOG_D("slider_action value: %d",value);

    lv_bar_set_value_anim(bar, value, 1);
    lv_gauge_set_value(gauge1, 0, value);
    LOG_D("%x,%x,%x,%x",gauge1, gauge2, gauge3, slider, bar);
    return LV_RES_OK;
}

static void create_tab4(lv_obj_t *parent)
{
    /* Create a default object*/
    gauge1 = lv_gauge_create(parent, NULL);
    
#if LV_COMPILER_NON_CONST_INIT_SUPPORTED
    static lv_color_t needle_colors[3] = {LV_COLOR_BLUE, LV_COLOR_PURPLE, LV_COLOR_TEAL};
#else
    static lv_color_t needle_colors[3] = { {{0}}, {{0}}, {{0}} };
#endif
    
    lv_obj_set_pos(gauge1, 10, 10);
    lv_gauge_set_value(gauge1, 0, 75);
    lv_gauge_set_range(gauge1, 0, 100);
    lv_gauge_set_needle_count(gauge1, 1, needle_colors);

    lv_obj_set_size(gauge1,300,300);

    /*Copy the previous gauge and set smaller size for it*/
    gauge2 = lv_gauge_create(parent, gauge1);
    lv_obj_set_size(gauge2, 2 * lv_obj_get_width(gauge1) / 3,  2 * lv_obj_get_height(gauge1) / 3);
    lv_obj_align(gauge2, gauge1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    /*Copy the first gauge add more needles and set new style*/
////////////////////////////
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

    gauge3 = lv_gauge_create(parent, gauge1);
    lv_obj_align(gauge3, gauge1, LV_ALIGN_OUT_RIGHT_MID, 20, 0);
    lv_obj_set_style(gauge3, &style3);
    lv_gauge_set_scale(gauge3, 270, 41, 5);
    lv_gauge_set_needle_count(gauge3, 3, needle_colors);
    lv_gauge_set_value(gauge3, 0, 20);
    lv_gauge_set_value(gauge3, 1, 40);
    lv_gauge_set_value(gauge3, 2, 60);
    
    bar = lv_bar_create(parent, NULL);
    lv_bar_set_range(bar, 0, 100);
    lv_bar_set_value(bar, 70);
    lv_obj_set_pos(bar, 700, 50);

    slider = lv_slider_create(parent, NULL);
    lv_slider_set_value(slider, 70);
    lv_slider_set_action(slider,slider_action);
    lv_obj_set_size(slider,300,50);
    lv_obj_set_pos(slider, 700, 300);

}
static void lv_gui_test(void *p)
{
    rt_int16_t value = 0;
    while(1)
    {
//        lv_bar_set_value_anim(bar, value, 1);
//        lv_bar_set_value(bar, value);
//        lv_gauge_set_value(gauge1, 0, value);
//        lv_slider_set_value(slider1, value);
        value++;
        if(value>=100)value=0;
        
        rt_thread_mdelay(100);
    }
}
////////////////////////////////////////////////////////////////////////////////

void lv_gui_create(lv_theme_t *th)
{
    lv_theme_set_current(th);
    lv_obj_t *scr = lv_cont_create(NULL, NULL);
    lv_scr_load(scr);
    lv_cont_set_style(scr, th->bg);

    lv_obj_t *tv = lv_tabview_create(scr, NULL);

    lv_obj_set_size(tv, LV_HOR_RES, LV_VER_RES);
    lv_obj_t *tab1 = lv_tabview_add_tab(tv, "Tab 1");
    lv_obj_t *tab2 = lv_tabview_add_tab(tv, "Tab 2");
    lv_obj_t *tab3 = lv_tabview_add_tab(tv, "Tab 3");
    lv_obj_t *tab4 = lv_tabview_add_tab(tv, "test tab4");

    create_tab1(th, tab1);
    create_tab2(th, tab2);
    create_tab3(th, tab3);

    create_tab4(tab4);
    
    /* littleGL demo gui thread */ 
    rt_thread_t thread = RT_NULL;
    thread = rt_thread_create("lv_test", lv_gui_test, RT_NULL, 30*1024, 8, 10); 
//    if(thread == RT_NULL)
//    {
//        return RT_ERROR;
//    }
    rt_thread_startup(thread);
}


static void lv_gui_run(void *p)
{
    rt_int16_t test_value = 0;
    lv_theme_t *th = lv_theme_zen_init(240, &lv_font_dejavu_30);
    lv_gui_create(th);
    while(1)
    {
        rt_thread_delay(RT_TICK_PER_SECOND/100);
        lv_task_handler(); 
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
INIT_APP_EXPORT(rt_lvgl_init); 
