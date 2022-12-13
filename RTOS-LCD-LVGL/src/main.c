/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include <asf.h>
#include <string.h>
#include "ili9341.h"
#include "lvgl.h"
#include "touch/touch.h"
#include "logo.h"
#include "vel_icon.h"
#include "timer_icon.h"
#include "route_icon.h"


// Para simulação
#include "arm_math.h"
#define TASK_SIMULATOR_STACK_SIZE (4096 / sizeof(portSTACK_TYPE))
#define TASK_SIMULATOR_STACK_PRIORITY (tskIDLE_PRIORITY)

#define RAIO 0.508/2
#define VEL_MAX_KMH  5.0f
#define VEL_MIN_KMH  0.5f

/************************************************************************/
/* Defines										                                          */
/************************************************************************/
#define VEL_PIO PIOA
#define VEL_PIO_ID ID_PIOA
#define VEL_PIO_IDX 19
#define VEL_PIO_IDX_MASK (1u << VEL_PIO_IDX)

/************************************************************************/
/* Define Struct para RTC                                               */
/************************************************************************/
/**
 *  Informacoes para o RTC
 *  poderia ser extraida do __DATE__ e __TIME__
 *  ou ser atualizado pelo PC.
 */
typedef struct  {
  uint32_t year;
  uint32_t month;
  uint32_t day;
  uint32_t week;
  uint32_t hour;
  uint32_t minute;
  uint32_t second;
} calendar;

/************************************************************************/
/* LCD / LVGL                                                           */
/************************************************************************/

#define LV_HOR_RES_MAX          (240)
#define LV_VER_RES_MAX          (320)

LV_FONT_DECLARE(primasans10);
LV_FONT_DECLARE(primasansbold10);
LV_FONT_DECLARE(primasans20);
LV_FONT_DECLARE(primasansbold20);
LV_FONT_DECLARE(primasansbold30);
LV_FONT_DECLARE(primasans40);
LV_FONT_DECLARE(primasansbold40);


// declarar a tela como global e est�tica
static lv_obj_t * scr1;  // screen 1
static lv_obj_t * scr2;  // screen 2
static lv_obj_t * scr3;  // screen 2

/*A static or global variable to store the buffers*/
static lv_disp_draw_buf_t disp_buf;

/*Static or global buffer(s). The second buffer is optional*/
static lv_color_t buf_1[LV_HOR_RES_MAX * LV_VER_RES_MAX];
static lv_disp_drv_t disp_drv;          /*A variable to hold the drivers. Must be static or global.*/
static lv_indev_drv_t indev_drv;

lv_obj_t * labelHome;
lv_obj_t * labelRoute;
lv_obj_t * labelSettings;

lv_obj_t * labelVelInst;
lv_obj_t * labelVelMed;
lv_obj_t * labelDist;
lv_obj_t * labelAro;
lv_obj_t * labelTimer;


lv_obj_t * labelClock1;
lv_obj_t * labelClock2;
lv_obj_t * labelClock3;

lv_obj_t * seta_down;
lv_obj_t * seta_up;
lv_obj_t * seta_minus;

/************************************************************************/
/* VAR globais                                                          */
/************************************************************************/
// Raio da roda da bike em metros
// Multiplique o tamanho do aro (diâmetro em inches) por 0,0254 e divida por 2 para obter o raio em metro

volatile int aro = 20;

volatile int aceleracao_flag = 0;

/************************************************************************/
/* PROTOTYPES                                                           */
/************************************************************************/

void RTC_init(Rtc *rtc, uint32_t id_rtc, calendar t, uint32_t irq_type);
static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource);
float kmh_to_hz(float vel, float raio);
void vel_data_handler(void);

/************************************************************************/
/* RTOS                                                                 */
/************************************************************************/

// Semaphore RTC
SemaphoreHandle_t xSemaphoreRTC;
// Semaphore VEL
SemaphoreHandle_t xSemaphoreVEL;
// Queue da Screen
QueueHandle_t xQueueSCR;

#define TASK_LCD_STACK_SIZE                (1024*6/sizeof(portSTACK_TYPE))
#define TASK_LCD_STACK_PRIORITY            (tskIDLE_PRIORITY)

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,  signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
	for (;;) {	}
}

extern void vApplicationIdleHook(void) { }

extern void vApplicationTickHook(void) { }

extern void vApplicationMallocFailedHook(void) {
	configASSERT( ( volatile void * ) NULL );
}

/************************************************************************/
/* Handlers                                                             */
/************************************************************************/

/**
* \brief Interrupt handler for the RTC. Refresh the display.
*/
void RTC_Handler(void) {
	uint32_t ul_status = rtc_get_status(RTC);
	
	/* seccond tick */
	if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC) {
		// o c�digo para irq de segundo vem aqui
		xSemaphoreGiveFromISR(xSemaphoreRTC, 0);
	}

	rtc_clear_status(RTC, RTC_SCCR_SECCLR);
	rtc_clear_status(RTC, RTC_SCCR_ALRCLR);
	rtc_clear_status(RTC, RTC_SCCR_ACKCLR);
	rtc_clear_status(RTC, RTC_SCCR_TIMCLR);
	rtc_clear_status(RTC, RTC_SCCR_CALCLR);
	rtc_clear_status(RTC, RTC_SCCR_TDERRCLR);
}

// Handler para irq das leituras de pulso no pino
void vel_data_handler(void) {
	xSemaphoreGiveFromISR(xSemaphoreVEL, 0);
}


/************************************************************************/
/* lvgl                                                                 */
/************************************************************************/

static void event_handlerHome(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);
	
	if(code == LV_EVENT_CLICKED) {
		int scrid = 0;
		xQueueSendFromISR(xQueueSCR, &scrid, 0);
	}
}

static void event_handlerRoute(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);
	

	if(code == LV_EVENT_CLICKED) {
		int scrid = 1;
		xQueueSendFromISR(xQueueSCR, &scrid, 0);
	}
}

static void event_handlerSettings(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);
	
	if(code == LV_EVENT_CLICKED) {
		int scrid = 2;
		xQueueSendFromISR(xQueueSCR, &scrid, 0);
	}
}

static void up_handler(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);
	
	if(code == LV_EVENT_CLICKED) {
		aro += 2;
		lv_label_set_text_fmt(labelAro, "%02d", aro);
	}
}

static void down_handler(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_CLICKED) {
		aro -= 2;
		lv_label_set_text_fmt(labelAro, "%02d", aro);
	}
}


void home(lv_obj_t * screen) {
	// Style (geral)
	static lv_style_t style;
	//All channels are 0-255
	lv_style_init(&style);
	lv_style_set_bg_color(&style, lv_color_white());
	lv_style_set_border_color(&style, lv_color_white());
	lv_style_set_shadow_width(&style, 0);
	
	labelClock1 = lv_label_create(screen);
	lv_obj_align(labelClock1, LV_ALIGN_TOP_LEFT, 10 , 5);
	lv_obj_set_style_text_font(labelClock1, &primasansbold20, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelClock1, lv_color_make(123,125,131), LV_STATE_DEFAULT);
	
	lv_obj_t * img_logo = lv_img_create(screen);
	lv_img_set_src(img_logo, &logo);
	lv_obj_align(img_logo, LV_ALIGN_TOP_RIGHT, 0, 0);
	
	lv_obj_t * img_vel_icon = lv_img_create(screen);
	lv_img_set_src(img_vel_icon, &vel_icon);
	lv_obj_align(img_vel_icon, LV_ALIGN_CENTER, -80, -70);
	
	lv_obj_t * vel_label =  lv_label_create(screen);
	lv_label_set_text(vel_label, "Velocidade instantânea");
	lv_obj_set_style_text_font(vel_label, &primasansbold10, LV_STATE_DEFAULT);
	lv_obj_align_to(vel_label, img_vel_icon, LV_ALIGN_RIGHT_MID, 140, -15);
	
	labelVelInst =  lv_label_create(screen);
	lv_obj_align_to(labelVelInst, img_vel_icon, LV_ALIGN_RIGHT_MID, 55, 20);
	lv_obj_set_style_text_font(labelVelInst, &primasansbold30, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelVelInst, lv_color_black(), LV_STATE_DEFAULT);
	lv_label_set_text_fmt(labelVelInst, "%.1f", 0.0);
	
	lv_obj_t * vel_unit_label =  lv_label_create(screen);
	lv_label_set_text(vel_unit_label, "KM/h");
	lv_obj_set_style_text_font(vel_unit_label, &primasansbold20, LV_STATE_DEFAULT);
	lv_obj_align_to(vel_unit_label, labelVelInst, LV_ALIGN_RIGHT_MID, 80, 0);
	
	seta_up = lv_img_create(screen);
	lv_img_set_src(seta_up, LV_SYMBOL_UP);
	lv_obj_align_to(seta_up, vel_label, LV_ALIGN_RIGHT_MID, 20, 20);
	lv_obj_add_style(seta_up, &style, 0);
	lv_obj_set_width(seta_up, 30);
	lv_obj_set_height(seta_up, 30);
	
	seta_down = lv_img_create(screen);
	lv_img_set_src(seta_down, LV_SYMBOL_DOWN);
	lv_obj_align_to(seta_down, vel_label, LV_ALIGN_RIGHT_MID, 20, 20);
	lv_obj_add_style(seta_down, &style, 0);
	lv_obj_set_width(seta_down, 30);
	lv_obj_set_height(seta_down, 30);
	
	seta_minus = lv_img_create(screen);
	lv_img_set_src(seta_minus, LV_SYMBOL_MINUS);
	lv_obj_align_to(seta_minus, vel_label, LV_ALIGN_RIGHT_MID, 20, 20);
	lv_obj_add_style(seta_minus, &style, 0);
	lv_obj_set_width(seta_minus, 30);
	lv_obj_set_height(seta_minus, 30);
	
	lv_obj_t * img_dist_icon = lv_img_create(screen);
	lv_img_set_src(img_dist_icon, &route_icon);
	lv_obj_align(img_dist_icon, LV_ALIGN_CENTER, -80, 10);
	
	lv_obj_t * dist_label =  lv_label_create(screen);
	lv_label_set_text(dist_label, "Distância percorrida");
	lv_obj_set_style_text_font(dist_label, &primasansbold10, LV_STATE_DEFAULT);
	lv_obj_align_to(dist_label, img_dist_icon, LV_ALIGN_RIGHT_MID, 120, -15);	
	
	labelDist =  lv_label_create(screen);
	lv_obj_align_to(labelDist, img_dist_icon, LV_ALIGN_RIGHT_MID, 45, 20);
	lv_obj_set_style_text_font(labelDist, &primasansbold30, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelDist, lv_color_black(), LV_STATE_DEFAULT);
	lv_label_set_text_fmt(labelDist, "%.1f", 0.0);
	
	lv_obj_t * dist_unit_label =  lv_label_create(screen);
	lv_label_set_text(dist_unit_label, "KM");
	lv_obj_set_style_text_font(dist_unit_label, &primasansbold20, LV_STATE_DEFAULT);
	lv_obj_align_to(dist_unit_label, labelDist, LV_ALIGN_RIGHT_MID, 60, 0);

	lv_obj_t * btnHome = lv_btn_create(screen);
	lv_obj_add_event_cb(btnHome, event_handlerHome, LV_EVENT_ALL, NULL);
	lv_obj_align(btnHome, LV_ALIGN_BOTTOM_LEFT, 5, -20);
	labelHome = lv_label_create(btnHome);
	lv_obj_set_style_text_font(labelHome, &primasansbold10, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelHome, lv_color_black(), LV_STATE_DEFAULT);
	lv_label_set_text(labelHome, "HOME");
	lv_obj_add_style(btnHome, &style, 0);
	lv_obj_set_width(btnHome, 60);  
	lv_obj_set_height(btnHome, 50);
	lv_obj_center(labelHome);

	lv_obj_t * btnRoute = lv_btn_create(screen);
	lv_obj_add_event_cb(btnRoute, event_handlerRoute, LV_EVENT_ALL, NULL);
	lv_obj_align(btnRoute, LV_ALIGN_BOTTOM_MID,  5, -20);
	labelRoute = lv_label_create(btnRoute);
	lv_obj_set_style_text_font(labelRoute, &primasansbold10, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelRoute, lv_color_black(), LV_STATE_DEFAULT);
	lv_label_set_text(labelRoute, "ROUTE");
	lv_obj_add_style(btnRoute, &style, 0);
	lv_obj_set_width(btnRoute, 60);
	lv_obj_set_height(btnRoute, 50);
	lv_obj_center(labelRoute);

	lv_obj_t * btnSettings = lv_btn_create(screen);
	lv_obj_add_event_cb(btnSettings, event_handlerSettings, LV_EVENT_ALL, NULL);
	lv_obj_align(btnSettings, LV_ALIGN_BOTTOM_RIGHT,  -5, -20);
	labelSettings = lv_label_create(btnSettings);
	lv_obj_set_style_text_font(labelSettings, &primasansbold10, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelSettings, lv_color_black(), LV_STATE_DEFAULT);
	lv_label_set_text(labelSettings, "SETTINGS");
	lv_obj_add_style(btnSettings, &style, 0);
	lv_obj_set_width(btnSettings, 60);  
	lv_obj_set_height(btnSettings, 50);
	lv_obj_center(labelSettings);
	
		
	//lv_obj_t * imgvel = lv_img_create(screen);
	//lv_img_set_src(imgvel, &velimg);
	//lv_obj_align(imgvel, LV_ALIGN_CENTER, 0, 0);
	
	//lv_obj_t * imgdist = lv_img_create(screen);
	//lv_img_set_src(imgdist, &distimg);
	//lv_obj_align(imgvel, LV_ALIGN_CENTER, 0, 0);
	
}

void route(lv_obj_t * screen) {
	// Style (geral)
	static lv_style_t style;
	//All channels are 0-255
	lv_style_init(&style);
	lv_style_set_bg_color(&style, lv_color_white());
	lv_style_set_border_color(&style, lv_color_white());
	lv_style_set_shadow_width(&style, 0);
	
	labelClock2 = lv_label_create(screen);
	lv_obj_align(labelClock2, LV_ALIGN_TOP_LEFT, 10 , 5);
	lv_obj_set_style_text_font(labelClock2, &primasansbold20, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelClock2, lv_color_make(123,125,131), LV_STATE_DEFAULT);
	
	lv_obj_t * img_logo = lv_img_create(screen);
	lv_img_set_src(img_logo, &logo);
	lv_obj_align(img_logo, LV_ALIGN_TOP_RIGHT, 0, 0);
	
	lv_obj_t * btnHome = lv_btn_create(screen);
	lv_obj_add_event_cb(btnHome, event_handlerHome, LV_EVENT_ALL, NULL);
	lv_obj_align(btnHome, LV_ALIGN_BOTTOM_LEFT, 5, -20);
	
	labelHome = lv_label_create(btnHome);
	lv_obj_set_style_text_font(labelHome, &primasansbold10, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelHome, lv_color_black(), LV_STATE_DEFAULT);
	lv_label_set_text(labelHome, "HOME");
	lv_obj_add_style(btnHome, &style, 0);
	lv_obj_set_width(btnHome, 60);
	lv_obj_set_height(btnHome, 50);
	lv_obj_center(labelHome);

	lv_obj_t * btnRoute = lv_btn_create(screen);
	lv_obj_add_event_cb(btnRoute, event_handlerRoute, LV_EVENT_ALL, NULL);
	lv_obj_align(btnRoute, LV_ALIGN_BOTTOM_MID,  5, -20);
	
	labelRoute = lv_label_create(btnRoute);
	lv_obj_set_style_text_font(labelRoute, &primasansbold10, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelRoute, lv_color_black(), LV_STATE_DEFAULT);
	lv_label_set_text(labelRoute, "ROUTE");
	lv_obj_add_style(btnRoute, &style, 0);
	lv_obj_set_width(btnRoute, 60);
	lv_obj_set_height(btnRoute, 50);
	lv_obj_center(labelRoute);

	lv_obj_t * btnSettings = lv_btn_create(screen);
	lv_obj_add_event_cb(btnSettings, event_handlerSettings, LV_EVENT_ALL, NULL);
	lv_obj_align(btnSettings, LV_ALIGN_BOTTOM_RIGHT,  -5, -20);
	
	labelSettings = lv_label_create(btnSettings);
	lv_obj_set_style_text_font(labelSettings, &primasansbold10, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelSettings, lv_color_black(), LV_STATE_DEFAULT);
	lv_label_set_text(labelSettings, "SETTINGS");
	lv_obj_add_style(btnSettings, &style, 0);
	lv_obj_set_width(btnSettings, 60);
	lv_obj_set_height(btnSettings, 50);
	lv_obj_center(labelSettings);
	
	lv_obj_t * img_vel_icon = lv_img_create(screen);
	lv_img_set_src(img_vel_icon, &vel_icon);
	lv_obj_align(img_vel_icon, LV_ALIGN_CENTER, -80, -70);
	
	lv_obj_t * vel_label =  lv_label_create(screen);
	lv_label_set_text(vel_label, "Velocidade média");
	lv_obj_set_style_text_font(vel_label, &primasansbold10, LV_STATE_DEFAULT);
	lv_obj_align_to(vel_label, img_vel_icon, LV_ALIGN_RIGHT_MID, 120, -15);
	
	labelVelMed =  lv_label_create(screen);
	lv_obj_set_style_text_font(labelVelMed, &primasansbold30, LV_STATE_DEFAULT);
	lv_obj_align_to(labelVelMed, img_vel_icon, LV_ALIGN_RIGHT_MID, 20, 20);
	
	lv_obj_t * vel_unit_label =  lv_label_create(screen);
	lv_label_set_text(vel_unit_label, "KM/h");
	lv_obj_set_style_text_font(vel_unit_label, &primasansbold20, LV_STATE_DEFAULT);
	lv_obj_align_to(vel_unit_label, labelVelMed, LV_ALIGN_RIGHT_MID, 120, 0);

	lv_obj_t * img_timer_icon = lv_img_create(screen);
	lv_img_set_src(img_timer_icon, &timer_icon);
	lv_obj_align(img_timer_icon, LV_ALIGN_CENTER, -80, 10);
	
	lv_obj_t * timer_label =  lv_label_create(screen);
	lv_label_set_text(timer_label, "Tempo gasto");
	lv_obj_set_style_text_font(timer_label, &primasansbold10, LV_STATE_DEFAULT);
	lv_obj_align_to(timer_label, img_timer_icon, LV_ALIGN_RIGHT_MID, 85, -15);
	
	labelTimer =  lv_label_create(screen);
	lv_obj_set_style_text_font(labelTimer, &primasansbold30, LV_STATE_DEFAULT);
	lv_obj_align_to(labelTimer, img_timer_icon, LV_ALIGN_RIGHT_MID, 20, 20);

}

void settings(lv_obj_t * screen) {	
	// Style (geral)
	static lv_style_t style;
	//All channels are 0-255
	lv_style_init(&style);
	lv_style_set_bg_color(&style, lv_color_white());
	lv_style_set_border_color(&style, lv_color_white());
	lv_style_set_shadow_width(&style, 0);
	
	labelClock3 = lv_label_create(screen);
	lv_obj_align(labelClock3, LV_ALIGN_TOP_LEFT, 10 , 5);
	lv_obj_set_style_text_font(labelClock3, &primasansbold20, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelClock3, lv_color_make(123,125,131), LV_STATE_DEFAULT);
	
	lv_obj_t * img_logo = lv_img_create(screen);
	lv_img_set_src(img_logo, &logo);
	lv_obj_align(img_logo, LV_ALIGN_TOP_RIGHT, 0, 0);
	
	lv_obj_t * aro_label =  lv_label_create(screen);
	lv_label_set_text(aro_label, "Mude seu aro:");
	lv_obj_set_style_text_font(aro_label, &primasansbold10, LV_STATE_DEFAULT);
	lv_obj_align(aro_label, LV_ALIGN_CENTER, 0, -30);
	labelAro =  lv_label_create(screen);
	lv_obj_align_to(labelAro, aro_label, LV_ALIGN_BOTTOM_MID, 10, 50);
	lv_obj_set_style_text_font(labelAro, &primasansbold30, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelAro, lv_color_black(), LV_STATE_DEFAULT);
	lv_label_set_text_fmt(labelAro, "%02d", aro);
	
	lv_obj_t * labelPls;
	lv_obj_t * btnpls = lv_btn_create(screen);
	lv_obj_add_event_cb(btnpls, up_handler, LV_EVENT_ALL, NULL);
	lv_obj_align_to(btnpls, aro_label, LV_ALIGN_RIGHT_MID, 20, 40);
	labelPls = lv_label_create(btnpls);
	lv_label_set_text(labelPls, LV_SYMBOL_UP);
	lv_obj_add_style(btnpls, &style, 0);
	lv_obj_set_style_text_color(labelPls,lv_color_black(), LV_STATE_DEFAULT);
	lv_obj_set_width(btnpls, 30);
	lv_obj_set_height(btnpls, 30);
	lv_obj_center(labelPls);
		
	
	lv_obj_t * labelMns;
	lv_obj_t * btnmns = lv_btn_create(screen);
	lv_obj_add_event_cb(btnmns, down_handler, LV_EVENT_ALL, NULL);
	lv_obj_align_to(btnmns, aro_label, LV_ALIGN_LEFT_MID, -20, 40);
	labelMns = lv_label_create(btnmns);
	lv_label_set_text(labelMns, LV_SYMBOL_DOWN);
	lv_obj_add_style(btnmns, &style, 0);
	lv_obj_set_style_text_color(labelMns,lv_color_black(), LV_STATE_DEFAULT);
	lv_obj_set_width(btnmns, 30);
	lv_obj_set_height(btnmns, 30);
	lv_obj_center(labelMns);
	
	
	lv_obj_t * btnHome = lv_btn_create(screen);
	lv_obj_add_event_cb(btnHome, event_handlerHome, LV_EVENT_ALL, NULL);
	lv_obj_align(btnHome, LV_ALIGN_BOTTOM_LEFT, 5, -20);
	labelHome = lv_label_create(btnHome);
	lv_obj_set_style_text_font(labelHome, &primasansbold10, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelHome, lv_color_black(), LV_STATE_DEFAULT);
	lv_label_set_text(labelHome, "HOME");
	lv_obj_add_style(btnHome, &style, 0);
	lv_obj_set_width(btnHome, 60);
	lv_obj_set_height(btnHome, 50);
	lv_obj_center(labelHome);

	lv_obj_t * btnRoute = lv_btn_create(screen);
	lv_obj_add_event_cb(btnRoute, event_handlerRoute, LV_EVENT_ALL, NULL);
	lv_obj_align(btnRoute, LV_ALIGN_BOTTOM_MID,  5, -20);
	labelRoute = lv_label_create(btnRoute);
	lv_obj_set_style_text_font(labelRoute, &primasansbold10, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelRoute, lv_color_black(), LV_STATE_DEFAULT);
	lv_label_set_text(labelRoute, "ROUTE");
	lv_obj_add_style(btnRoute, &style, 0);
	lv_obj_set_width(btnRoute, 60);
	lv_obj_set_height(btnRoute, 50);
	lv_obj_center(labelRoute);

	lv_obj_t * btnSettings = lv_btn_create(screen);
	lv_obj_add_event_cb(btnSettings, event_handlerSettings, LV_EVENT_ALL, NULL);
	lv_obj_align(btnSettings, LV_ALIGN_BOTTOM_RIGHT,  -5, -20);
	labelSettings = lv_label_create(btnSettings);
	lv_obj_set_style_text_font(labelSettings, &primasansbold10, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelSettings, lv_color_black(), LV_STATE_DEFAULT);
	lv_label_set_text(labelSettings, "SETTINGS");
	lv_obj_add_style(btnSettings, &style, 0);
	lv_obj_set_width(btnSettings, 60);
	lv_obj_set_height(btnSettings, 50);
	lv_obj_center(labelSettings);
	
}

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

static void task_update(void *pvParameters) {
	// Cria as 3 telas
	scr1  = lv_obj_create(NULL);
	scr2  = lv_obj_create(NULL);
	scr3  = lv_obj_create(NULL);
	
	lv_obj_t * make_scrs[3];
	make_scrs[0] = scr1;
	make_scrs[1] = scr2;
	make_scrs[2] = scr3;
	
	lv_obj_set_style_bg_color(scr1, lv_color_white(), LV_PART_MAIN );
	lv_obj_set_style_bg_color(scr2, lv_color_white(), LV_PART_MAIN );
	lv_obj_set_style_bg_color(scr3, lv_color_white(), LV_PART_MAIN );
	
	home(scr1);
	route(scr2);
	settings(scr3);
	
	lv_obj_add_flag(seta_minus, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(seta_down, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(seta_up, LV_OBJ_FLAG_HIDDEN);

	int scr = 0;

	for (;;)  {
		if (xQueueReceive(xQueueSCR, &(scr), (TickType_t) 0)) {
			
		}
		lv_scr_load(make_scrs[scr]); // exibe tela
		vTaskDelay(500);
			
	}
}

static void task_lcd(void *pvParameters) {
	int px, py;
	for (;;)  {
		lv_tick_inc(50);
		lv_task_handler();
		vTaskDelay(50);
	}
}

static void task_rtc(void *pvParameters) {
	/** Configura RTC */
	calendar rtc_initial = {2022, 12, 13, 49, 15, 46, 0};
	/* configura alarme do RTC para cada 1 segundo */
	RTC_init(RTC, ID_RTC, rtc_initial, RTC_IER_SECEN);
	
	/* Leitura do valor atual do RTC */
	uint32_t current_hour, current_min, current_sec;
	rtc_get_time(RTC, &current_hour, &current_min, &current_sec);

	lv_label_set_text_fmt(labelClock1, "%02d:%02d:%02d", current_hour, current_min, current_sec);
	lv_label_set_text_fmt(labelClock2, "%02d:%02d:%02d", current_hour, current_min, current_sec);
	lv_label_set_text_fmt(labelClock3, "%02d:%02d:%02d", current_hour, current_min, current_sec);

	for (;;)  {
		if (xSemaphoreTake(xSemaphoreRTC, 5000 / portTICK_PERIOD_MS) == pdTRUE){
			rtc_get_time(RTC, &current_hour, &current_min, &current_sec);
			lv_label_set_text_fmt(labelClock1, "%02d:%02d:%02d", current_hour, current_min, current_sec);
			lv_label_set_text_fmt(labelClock2, "%02d:%02d:%02d", current_hour, current_min, current_sec);
			lv_label_set_text_fmt(labelClock3, "%02d:%02d:%02d", current_hour, current_min, current_sec);
		}	
	}
}

static void task_simulador(void *pvParameters) {

	pmc_enable_periph_clk(ID_PIOC);
	pio_set_output(PIOC, PIO_PC31, 1, 0, 0);

	float vel = VEL_MAX_KMH;
	float f;
	int ramp_up = 1;
	int t;

	while(1){
		pio_clear(PIOC, PIO_PC31);
		delay_ms(1);
		pio_set(PIOC, PIO_PC31);
		
		if (ramp_up) {
			vel += 0.5;
		} else {
			vel -= 0.5;
		}

		if (vel >= VEL_MAX_KMH)
		ramp_up = 0;
		else if (vel <= VEL_MIN_KMH)
		ramp_up = 1;
		
		f = kmh_to_hz(vel, RAIO);
		t = 965*(1.0/f); // UTILIZADO 965 como multiplicador ao invés de 1000
												 // para compensar o atraso gerado pelo Escalonador do freeRTOS
		delay_ms(t);
	}
}

static void task_vel(void *pvParameters) {
	pmc_enable_periph_clk(VEL_PIO_ID);
	pio_set_input(VEL_PIO, VEL_PIO_IDX_MASK, PIO_DEFAULT);
	pio_pull_up(VEL_PIO, VEL_PIO_IDX_MASK, 0);
	pio_handler_set(VEL_PIO,
									VEL_PIO_ID,
									VEL_PIO_IDX_MASK,
									PIO_IT_FALL_EDGE,
									vel_data_handler);
									
	// Config da interrupção do botão embutido
	pio_enable_interrupt(VEL_PIO, VEL_PIO_IDX_MASK);
	pio_get_interrupt_status(VEL_PIO);

	NVIC_EnableIRQ(VEL_PIO_ID);
	NVIC_SetPriority(VEL_PIO_ID, 4);
	
	/* Leitura do valor atual do RTC para contar o tempo gasto */
	uint32_t current_hour, current_min, current_sec, last_hour, last_min;
	rtc_get_time(RTC, &last_hour, &last_min, &current_sec);
				
	double vel_inst = 0.0;
	double vel_media = 0.0;
	double distancia = 0.0;
	long int n_pulsos = 0;
	double dt = 0.0;
	double tempo_total = 0.0;
	double vel_anterior = 0.0;
	double aceler = 0.0;
	uint32_t horas = 0;
	uint32_t minutos = 0;
	
	// Limpa semaforo antes de realmente começar a task
	xSemaphoreTake(xSemaphoreVEL, 1000 / portTICK_PERIOD_MS);
					
	while(1) {
		double raio = (double) aro * 0.0254 / 2.0;
		// Timer conta até 5 segundos
		RTT_init(1000, 5000, NULL);
		if (xSemaphoreTake(xSemaphoreVEL, 5000 / portTICK_PERIOD_MS) == pdTRUE){
			// Guarda tempo que demorou para receber o pulso na variável dt (em segundos)
			dt = rtt_read_timer_value(RTT) * portTICK_PERIOD_MS / 1000.0;
			tempo_total += dt;
			// Incrementa a quantidade de pulsos medidos
			n_pulsos++;
			// Cálculo da velocidade instantânea
			vel_inst = 3.6 * (raio * 2 * PI / dt);
			// Cálculo da aceleração (derivação discreta)
			aceler = 10 * (vel_inst - vel_anterior) / dt;
			vel_anterior = vel_inst;
			// Cálculo da distância percorrida total
			distancia = n_pulsos * 2 * PI * raio / 1000;
			// Cálculo da velocidade média
			vel_media = distancia / (tempo_total / 3600);
			// Obtém hora atual para fazer cálculo do tempo gasto no trajeto
			rtc_get_time(RTC, &current_hour, &current_min, &current_sec);
			if (current_min != last_min) {
				horas = current_hour - last_hour;
				minutos = current_min - last_min;
				last_hour = current_hour;
				last_min = current_min;
			}
			
			lv_label_set_text_fmt(labelVelInst, "%.1f", vel_inst);
			lv_label_set_text_fmt(labelDist, "%.1f", distancia/1000);
			lv_label_set_text_fmt(labelVelMed, "%.1f", vel_media);
			lv_label_set_text_fmt(labelTimer, "%02d:%02d", horas, minutos);
			
			if (aceler > 3.0) {
				aceleracao_flag = 1;
				
				lv_obj_clear_flag(seta_up, LV_OBJ_FLAG_HIDDEN);
				lv_obj_add_flag(seta_minus, LV_OBJ_FLAG_HIDDEN);
				lv_obj_add_flag(seta_down, LV_OBJ_FLAG_HIDDEN);
				
			} else if (aceler < -3.0) {
				aceleracao_flag = -1;
				
				lv_obj_clear_flag(seta_down, LV_OBJ_FLAG_HIDDEN);
				lv_obj_add_flag(seta_minus, LV_OBJ_FLAG_HIDDEN);
				lv_obj_add_flag(seta_up, LV_OBJ_FLAG_HIDDEN);
				
			} else {
				aceleracao_flag = 0;
				
				lv_obj_clear_flag(seta_minus, LV_OBJ_FLAG_HIDDEN);
				lv_obj_add_flag(seta_up, LV_OBJ_FLAG_HIDDEN);
				lv_obj_add_flag(seta_down, LV_OBJ_FLAG_HIDDEN);
				
			}
			
			//printf("Timer %f\n", dt);
			//printf("vel inst %f km/h\n", vel_inst);
			//printf("aceler %f\n", aceler);
			//printf("dist %f km \n", distancia);
			//printf("vel media %f km/h \n", vel_media);
		}
	}
}

/************************************************************************/
/* configs                                                              */
/************************************************************************/

static void configure_lcd(void) {
	/**LCD pin configure on SPI*/
	pio_configure_pin(LCD_SPI_MISO_PIO, LCD_SPI_MISO_FLAGS);
	pio_configure_pin(LCD_SPI_MOSI_PIO, LCD_SPI_MOSI_FLAGS);
	pio_configure_pin(LCD_SPI_SPCK_PIO, LCD_SPI_SPCK_FLAGS);
	pio_configure_pin(LCD_SPI_NPCS_PIO, LCD_SPI_NPCS_FLAGS);
	pio_configure_pin(LCD_SPI_RESET_PIO, LCD_SPI_RESET_FLAGS);
	pio_configure_pin(LCD_SPI_CDS_PIO, LCD_SPI_CDS_FLAGS);
	
	ili9341_init();
	ili9341_backlight_on();
}

static void configure_console(void) {
	const usart_serial_options_t uart_serial_options = {
		.baudrate = USART_SERIAL_EXAMPLE_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT,
	};

	/* Configure console UART. */
	stdio_serial_init(CONSOLE_UART, &uart_serial_options);

	/* Specify that stdout should not be buffered. */
	setbuf(stdout, NULL);
}

/************************************************************************/
/* port lvgl                                                            */
/************************************************************************/

void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p) {
	ili9341_set_top_left_limit(area->x1, area->y1);   ili9341_set_bottom_right_limit(area->x2, area->y2);
	ili9341_copy_pixels_to_screen(color_p,  (area->x2 + 1 - area->x1) * (area->y2 + 1 - area->y1));
	
	/* IMPORTANT!!!
	* Inform the graphics library that you are ready with the flushing*/
	lv_disp_flush_ready(disp_drv);
}

void my_input_read(lv_indev_drv_t * drv, lv_indev_data_t*data) {
	int px, py, pressed;
	
	if (readPoint(&px, &py))
		data->state = LV_INDEV_STATE_PRESSED;
	else
		data->state = LV_INDEV_STATE_RELEASED; 
	
	data->point.x = py;
	data->point.y = 320 - px;
}

void configure_lvgl(void) {
	lv_init();
	lv_disp_draw_buf_init(&disp_buf, buf_1, NULL, LV_HOR_RES_MAX * LV_VER_RES_MAX);
	
	lv_disp_drv_init(&disp_drv);            /*Basic initialization*/
	disp_drv.draw_buf = &disp_buf;          /*Set an initialized buffer*/
	disp_drv.flush_cb = my_flush_cb;        /*Set a flush callback to draw to the display*/
	disp_drv.hor_res = LV_HOR_RES_MAX;      /*Set the horizontal resolution in pixels*/
	disp_drv.ver_res = LV_VER_RES_MAX;      /*Set the vertical resolution in pixels*/

	lv_disp_t * disp;
	disp = lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/
	
	/* Init input on LVGL */
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = my_input_read;
	lv_indev_t * my_indev = lv_indev_drv_register(&indev_drv);
}

/**
* Configura o RTC para funcionar com interrupcao de alarme
*/
void RTC_init(Rtc *rtc, uint32_t id_rtc, calendar t, uint32_t irq_type) {
	/* Configura o PMC */
	pmc_enable_periph_clk(ID_RTC);

	/* Default RTC configuration, 24-hour mode */
	rtc_set_hour_mode(rtc, 0);

	/* Configura data e hora manualmente */
	rtc_set_date(rtc, t.year, t.month, t.day, t.week);
	rtc_set_time(rtc, t.hour, t.minute, t.second);

	/* Configure RTC interrupts */
	NVIC_DisableIRQ(id_rtc);
	NVIC_ClearPendingIRQ(id_rtc);
	NVIC_SetPriority(id_rtc, 4);
	NVIC_EnableIRQ(id_rtc);

	/* Ativa interrupcao via alarme */
	rtc_enable_interrupt(rtc,  irq_type);
}

static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource) {

	uint16_t pllPreScale = (int) (((float) 32768) / freqPrescale);
	
	rtt_sel_source(RTT, false);
	rtt_init(RTT, pllPreScale);
	
	if (rttIRQSource & RTT_MR_ALMIEN) {
		uint32_t ul_previous_time;
		ul_previous_time = rtt_read_timer_value(RTT);
		while (ul_previous_time == rtt_read_timer_value(RTT));
		rtt_write_alarm_time(RTT, IrqNPulses+ul_previous_time);
	}

	/* config NVIC */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 4);
	NVIC_EnableIRQ(RTT_IRQn);

	/* Enable RTT interrupt */
	if (rttIRQSource & (RTT_MR_RTTINCIEN | RTT_MR_ALMIEN))
	rtt_enable_interrupt(RTT, rttIRQSource);
	else
	rtt_disable_interrupt(RTT, RTT_MR_RTTINCIEN | RTT_MR_ALMIEN);
	
}



/************************************************************************/
/* Funcoes                                                              */
/************************************************************************/
/**
* aro 20" => 1" == 2,54 cm => 50,8 cm (diametro) => 0.508/2 = 0.254m (raio da roda)
* w = 2 pi f (m/s)
* v [km/h] = (w*r) / 3.6 = (2 pi f r) / 3.6
* f = v / (2 pi r 3.6)
* Exemplo : 5 km / h = 1.38 m/s
*           f = 0.87Hz
*           t = 1/f => 1/0.87 = 1,149s
*/
float kmh_to_hz(float vel, float raio) {
	float f = vel / (2*PI*raio*3.6);
	return(f);
}

/************************************************************************/
/* main                                                                 */
/************************************************************************/
int main(void) {
	/* board and sys init */
	board_init();
	sysclk_init();
	configure_console();

	/* LCd, touch and lvgl init*/
	configure_lcd();
	ili9341_set_orientation(ILI9341_FLIP_Y | ILI9341_SWITCH_XY);
	configure_touch();
	configure_lvgl();
	
	// Inicializa semaforo RTC
	xSemaphoreRTC = xSemaphoreCreateBinary();
	// Inicializa semaforo VEL
	xSemaphoreVEL = xSemaphoreCreateBinary();

	// Inicializa fila de scrs
	xQueueSCR = xQueueCreate(16, sizeof(int));
	if (xQueueSCR == NULL) {
		printf("Erro ao criar fila de screen \n");
	}
	
	/* Create task to control lcd */
	if (xTaskCreate(task_update, "TASK_UPDATE", 1024*2, NULL, TASK_LCD_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create update task\r\n");
	}
//
	///* Create task to control lcd */
	if (xTaskCreate(task_lcd, "TASK_LCD", TASK_LCD_STACK_SIZE, NULL, TASK_LCD_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create lcd task\r\n");
	}
	
	///* Create task to control clock rtc */
	if (xTaskCreate(task_rtc, "TASK_CLOCK_RTC", 1024*2, NULL, TASK_LCD_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create rtc clock task\r\n");
	}
	
	/* Cria task que simula giro da roda da bike */
	if (xTaskCreate(task_simulador, "SIMUL", 1024, NULL, TASK_SIMULATOR_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create simul task\r\n");
	}
	
	/* Cria task que pega dados do giro da roda da bike */
	if (xTaskCreate(task_vel, "VEL", 1024, NULL, TASK_SIMULATOR_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create vel task\r\n");
	}
	
	/* Start the scheduler. */
	vTaskStartScheduler();

	while(1){ }
}
