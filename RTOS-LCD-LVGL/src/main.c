/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include <asf.h>
#include <string.h>
#include "ili9341.h"
#include "lvgl.h"
#include "touch/touch.h"
#include "logo2.h"

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

#define VEL_DATA_SIZE 50

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

static  lv_obj_t * labelBtn1;
static  lv_obj_t * labelBtn2;
static  lv_obj_t * labelBtn3;
static  lv_obj_t * labelBtn4;
static  lv_obj_t * labelBtn5;

lv_obj_t * meter;
lv_obj_t * labelFloor;
lv_obj_t * labelSetValue;
lv_obj_t * labelClock;

/************************************************************************/
/* VAR globais                                                          */
/************************************************************************/
// Raio da roda da bike em metros
// Multiplique o tamanho do aro (diâmetro em inches) por 0,0254 e divida por 2 para obter o raio em metro
//volatile double aro = 20.0;
volatile double raio = 20.0 * 0.0254 / 2;

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

// Semaphore RTC
SemaphoreHandle_t xSemaphoreRTC;
// Semaphore VEL
SemaphoreHandle_t xSemaphoreVEL;

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

static void event_handler1(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);

	if(code == LV_EVENT_CLICKED) {
		LV_LOG_USER("Clicked");
	}
	else if(code == LV_EVENT_VALUE_CHANGED) {
		LV_LOG_USER("Toggled");
	}
}

static void up_handler(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);
	char *c;
	int temp;
	if(code == LV_EVENT_CLICKED) {
		c = lv_label_get_text(labelSetValue);
		temp = atoi(c);
		lv_label_set_text_fmt(labelSetValue, "%02d", temp + 1);
	}
}

static void down_handler(lv_event_t * e) {
	lv_event_code_t code = lv_event_get_code(e);
	char *c;
	int temp;
	if(code == LV_EVENT_CLICKED) {
		c = lv_label_get_text(labelSetValue);
		temp = atoi(c);
		lv_label_set_text_fmt(labelSetValue, "%02d", temp - 1);
	}
}

static void set_value(void * indic, int32_t v)
{
	lv_meter_set_indicator_end_value(meter, indic, v);
}

/**
 * A meter with multiple arcs
 */
void meter_widget(lv_obj_t * screen)
{
	meter = lv_meter_create(screen);
	lv_obj_center(meter);
	lv_obj_set_size(meter, 160, 160);
	lv_obj_align(meter, LV_ALIGN_CENTER, 0, -45);

	/*Remove the circle from the middle*/
	lv_obj_remove_style(meter, NULL, LV_PART_INDICATOR);

	/*Add a scale first*/
	lv_meter_scale_t * scale = lv_meter_add_scale(meter);
	
	lv_meter_set_scale_ticks(meter, scale, 7, 3, 6, lv_palette_main(LV_PALETTE_GREY));
	lv_meter_set_scale_major_ticks(meter, scale, 1, 4, 30, lv_color_hex3(0xeee), 10);
	lv_meter_set_scale_range(meter, scale, 0, 60, 270, 90);

	/*Add a three arc indicator*/
	lv_meter_indicator_t * indic1 = lv_meter_add_arc(meter, scale, 20, lv_palette_main(LV_PALETTE_RED), 0);

    /*Create an animation to set the value*/
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, set_value);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_repeat_delay(&a, 100);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);

    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 2000);
    lv_anim_set_var(&a, indic1);
    lv_anim_start(&a);
}


void home(lv_obj_t * screen) {
		// Style (geral)
		static lv_style_t style;
		//All channels are 0-255
		lv_style_init(&style);
		// lv_style_set_border_width(&style, 5);
		
		meter_widget(screen);




		
	// 	// Cria cada um dos bot�es
    // lv_obj_t * labelBtn1;
    // lv_obj_t * btn1 = lv_btn_create(scr1);
    // lv_obj_add_event_cb(btn1, event_handler1, LV_EVENT_ALL, NULL);
    // lv_obj_align(btn1, LV_ALIGN_BOTTOM_LEFT, 5, -5);
    // labelBtn1 = lv_label_create(btn1);
    // lv_label_set_text(labelBtn1, "[  " LV_SYMBOL_POWER);
	// 	lv_obj_add_style(btn1, &style, 0);
	// 	lv_obj_set_width(btn1, 60);  
	// 	lv_obj_set_height(btn1, 60);
    // lv_obj_center(labelBtn1);
		
	// 	lv_obj_t * labelBtn2;
	// 	lv_obj_t * btn2 = lv_btn_create(scr1);
	// 	lv_obj_add_event_cb(btn2, event_handler1, LV_EVENT_ALL, NULL);
	// 	lv_obj_align_to(btn2, btn1, LV_ALIGN_RIGHT_MID, 40, -22);
	// 	labelBtn2 = lv_label_create(btn2);
	// 	lv_label_set_text(labelBtn2, "|  M ");
	// 	lv_obj_add_style(btn2, &style, 0);
	// 	lv_obj_set_width(btn2, 60);
	// 	lv_obj_set_height(btn2, 60);
	// 	lv_obj_center(labelBtn2);
		
	// 	lv_obj_t * labelBtn3;
	// 	lv_obj_t * btn3 = lv_btn_create(scr1);
	// 	lv_obj_add_event_cb(btn3, event_handler1, LV_EVENT_ALL, NULL);
	// 	lv_obj_align_to(btn3, btn2, LV_ALIGN_RIGHT_MID, 40, -22);
	// 	labelBtn3 = lv_label_create(btn3);
	// 	lv_label_set_text(labelBtn3, "|  " LV_SYMBOL_SETTINGS "  ]");
	// 	lv_obj_add_style(btn3, &style, 0);
	// 	lv_obj_set_width(btn3, 60);
	// 	lv_obj_set_height(btn3, 60);
	// 	lv_obj_center(labelBtn3);
		
	// 	lv_obj_t * labelBtn4;
	// 	lv_obj_t * btn4 = lv_btn_create(scr1);
	// 	lv_obj_add_event_cb(btn4, down_handler, LV_EVENT_ALL, NULL);
	// 	lv_obj_align(btn4, LV_ALIGN_BOTTOM_RIGHT, -5, -5);
	// 	labelBtn4 = lv_label_create(btn4);
	// 	lv_label_set_text(labelBtn4, LV_SYMBOL_DOWN "  ]");
	// 	lv_obj_add_style(btn4, &style, 0);
	// 	lv_obj_set_width(btn4, 60);
	// 	lv_obj_set_height(btn4, 60);
	// 	lv_obj_center(labelBtn4);
		
	// 	lv_obj_t * labelBtn5;
	// 	lv_obj_t * btn5 = lv_btn_create(scr1);
	// 	lv_obj_add_event_cb(btn5, up_handler, LV_EVENT_ALL, NULL);
	// 	lv_obj_align_to(btn5, btn4, LV_ALIGN_LEFT_MID, -80, -22);
	// 	labelBtn5 = lv_label_create(btn5);
	// 	lv_label_set_text(labelBtn5, "[  " LV_SYMBOL_UP);
	// 	lv_obj_add_style(btn5, &style, 0);
	// 	lv_obj_set_width(btn5, 60);
	// 	lv_obj_set_height(btn5, 60);
	// 	lv_obj_center(labelBtn5);
		
	// 	// Cria labels do termostato
	// 	labelFloor = lv_label_create(scr1);
	// 	lv_obj_align(labelFloor, LV_ALIGN_LEFT_MID, 35 , -45);
	// 	lv_obj_set_style_text_font(labelFloor, &dseg70, LV_STATE_DEFAULT);
	// 	lv_obj_set_style_text_color(labelFloor, lv_color_white(), LV_STATE_DEFAULT);
	// 	lv_label_set_text_fmt(labelFloor, "%02d", 23);
		
	// 	labelSetValue = lv_label_create(scr1);
	// 	lv_obj_align(labelSetValue, LV_ALIGN_RIGHT_MID, -30 , -45);
	// 	lv_obj_set_style_text_font(labelSetValue, &dseg50, LV_STATE_DEFAULT);
	// 	lv_obj_set_style_text_color(labelSetValue, lv_color_white(), LV_STATE_DEFAULT);
	// 	lv_label_set_text_fmt(labelSetValue, "%02d", 22);
		
	// 	labelClock = lv_label_create(scr1);
	// 	lv_obj_align(labelClock, LV_ALIGN_TOP_RIGHT, -10 , 5);
	// 	lv_obj_set_style_text_font(labelClock, &dseg30, LV_STATE_DEFAULT);
	// 	lv_obj_set_style_text_color(labelClock, lv_color_white(), LV_STATE_DEFAULT);

}

void route(lv_obj_t * screen) {
	// Style (geral)
	static lv_style_t style;
	//All channels are 0-255
	lv_style_init(&style);
	
	labelClock = lv_label_create(screen);
	lv_obj_align(labelClock, LV_ALIGN_TOP_LEFT, 10 , 5);
	lv_obj_set_style_text_font(labelClock, &primasansbold20, LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(labelClock, lv_color_make(123,125,131), LV_STATE_DEFAULT);
	
	lv_obj_t * img = lv_img_create(screen);
	lv_img_set_src(img, &logo2);
	lv_obj_align(img, LV_ALIGN_TOP_RIGHT, 0, 0);




	
	// 	// Cria cada um dos bot�es
	// lv_obj_t * labelBtn1;
	// lv_obj_t * btn1 = lv_btn_create(scr1);
	// lv_obj_add_event_cb(btn1, event_handler1, LV_EVENT_ALL, NULL);
	// lv_obj_align(btn1, LV_ALIGN_BOTTOM_LEFT, 5, -5);
	// labelBtn1 = lv_label_create(btn1);
	// lv_label_set_text(labelBtn1, "[  " LV_SYMBOL_POWER);
	// 	lv_obj_add_style(btn1, &style, 0);
	// 	lv_obj_set_width(btn1, 60);
	// 	lv_obj_set_height(btn1, 60);
	// lv_obj_center(labelBtn1);
	
	// 	lv_obj_t * labelBtn2;
	// 	lv_obj_t * btn2 = lv_btn_create(scr1);
	// 	lv_obj_add_event_cb(btn2, event_handler1, LV_EVENT_ALL, NULL);
	// 	lv_obj_align_to(btn2, btn1, LV_ALIGN_RIGHT_MID, 40, -22);
	// 	labelBtn2 = lv_label_create(btn2);
	// 	lv_label_set_text(labelBtn2, "|  M ");
	// 	lv_obj_add_style(btn2, &style, 0);
	// 	lv_obj_set_width(btn2, 60);
	// 	lv_obj_set_height(btn2, 60);
	// 	lv_obj_center(labelBtn2);
	
	// 	lv_obj_t * labelBtn3;
	// 	lv_obj_t * btn3 = lv_btn_create(scr1);
	// 	lv_obj_add_event_cb(btn3, event_handler1, LV_EVENT_ALL, NULL);
	// 	lv_obj_align_to(btn3, btn2, LV_ALIGN_RIGHT_MID, 40, -22);
	// 	labelBtn3 = lv_label_create(btn3);
	// 	lv_label_set_text(labelBtn3, "|  " LV_SYMBOL_SETTINGS "  ]");
	// 	lv_obj_add_style(btn3, &style, 0);
	// 	lv_obj_set_width(btn3, 60);
	// 	lv_obj_set_height(btn3, 60);
	// 	lv_obj_center(labelBtn3);
	
	// 	lv_obj_t * labelBtn4;
	// 	lv_obj_t * btn4 = lv_btn_create(scr1);
	// 	lv_obj_add_event_cb(btn4, down_handler, LV_EVENT_ALL, NULL);
	// 	lv_obj_align(btn4, LV_ALIGN_BOTTOM_RIGHT, -5, -5);
	// 	labelBtn4 = lv_label_create(btn4);
	// 	lv_label_set_text(labelBtn4, LV_SYMBOL_DOWN "  ]");
	// 	lv_obj_add_style(btn4, &style, 0);
	// 	lv_obj_set_width(btn4, 60);
	// 	lv_obj_set_height(btn4, 60);
	// 	lv_obj_center(labelBtn4);
	
	// 	lv_obj_t * labelBtn5;
	// 	lv_obj_t * btn5 = lv_btn_create(scr1);
	// 	lv_obj_add_event_cb(btn5, up_handler, LV_EVENT_ALL, NULL);
	// 	lv_obj_align_to(btn5, btn4, LV_ALIGN_LEFT_MID, -80, -22);
	// 	labelBtn5 = lv_label_create(btn5);
	// 	lv_label_set_text(labelBtn5, "[  " LV_SYMBOL_UP);
	// 	lv_obj_add_style(btn5, &style, 0);
	// 	lv_obj_set_width(btn5, 60);
	// 	lv_obj_set_height(btn5, 60);
	// 	lv_obj_center(labelBtn5);
	
	// 	// Cria labels do termostato
	// 	labelFloor = lv_label_create(scr1);
	// 	lv_obj_align(labelFloor, LV_ALIGN_LEFT_MID, 35 , -45);
	// 	lv_obj_set_style_text_font(labelFloor, &dseg70, LV_STATE_DEFAULT);
	// 	lv_obj_set_style_text_color(labelFloor, lv_color_white(), LV_STATE_DEFAULT);
	// 	lv_label_set_text_fmt(labelFloor, "%02d", 23);
	
	// 	labelSetValue = lv_label_create(scr1);
	// 	lv_obj_align(labelSetValue, LV_ALIGN_RIGHT_MID, -30 , -45);
	// 	lv_obj_set_style_text_font(labelSetValue, &dseg50, LV_STATE_DEFAULT);
	// 	lv_obj_set_style_text_color(labelSetValue, lv_color_white(), LV_STATE_DEFAULT);
	// 	lv_label_set_text_fmt(labelSetValue, "%02d", 22);

}

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

static void task_update(void *pvParameters) {
	// Criando duas telas
	scr1  = lv_obj_create(NULL);
	scr2  = lv_obj_create(NULL);
	scr3  = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(scr1, lv_color_white(), LV_PART_MAIN );
	lv_obj_set_style_bg_color(scr2, lv_color_white(), LV_PART_MAIN );
	lv_obj_set_style_bg_color(scr3, lv_color_white(), LV_PART_MAIN );
	home(scr1);
	route(scr2);
	// settings(scr3);
	
	//lv_scr_load(scr1); // exibe tela 1
	//vTaskDelay(500);
	lv_scr_load(scr2); // exibe tela 2
	vTaskDelay(500);
	//lv_scr_load(scr3); // exibe tela 3
	//vTaskDelay(500);

	for (;;)  {
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
	calendar rtc_initial = {2022, 11, 18, 46, 15, 46, 1};
	/* configura alarme do RTC para cada 1 segundo */
	RTC_init(RTC, ID_RTC, rtc_initial, RTC_IER_SECEN);
	
	/* Leitura do valor atual do RTC */
	uint32_t current_hour, current_min, current_sec;
	rtc_get_time(RTC, &current_hour, &current_min, &current_sec);

	lv_label_set_text_fmt(labelClock, "%02d:%02d:%02d", current_hour, current_min, current_sec);

	for (;;)  {
		if (xSemaphoreTake(xSemaphoreRTC, 1500 / portTICK_PERIOD_MS) == pdTRUE){
			rtc_get_time(RTC, &current_hour, &current_min, &current_sec);
			lv_label_set_text_fmt(labelClock, "%02d:%02d:%02d", current_hour, current_min, current_sec);
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
									
	double vel_inst = 0.0;
	double vel_media = 0.0;
	double distancia = 0.0;
	long int n_pulsos = 0;
	double dt = 0.0;
	double tempo_total = 0.0;
	double vel_anterior = 0.0;
	double aceler = 0.0;
	
	// Limpa semaforo antes de realmente começar a task
	xSemaphoreTake(xSemaphoreVEL, 1000 / portTICK_PERIOD_MS);
										
	while(1) {
		// Timer conta até 5 segundos
		RTT_init(1000, 5000, NULL);
		if (xSemaphoreTake(xSemaphoreVEL, 5000 / portTICK_PERIOD_MS) == pdTRUE){
			dt = rtt_read_timer_value(RTT) * portTICK_PERIOD_MS / 1000.00;
			tempo_total += dt;
			n_pulsos++;
			vel_inst = 3.6 * (raio * 2 * PI / dt);
			aceler = 10 * (vel_inst - vel_anterior) / dt;
			vel_anterior = vel_inst;
			distancia = n_pulsos * 2 * PI * raio / 1000;
			vel_media = distancia / (tempo_total / 3600);
			
			if (aceler > 3.0) {
				aceleracao_flag = 1;
			} else if (aceler < -3.0) {
				aceleracao_flag = -1;
			} else {
				aceleracao_flag = 0;
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
