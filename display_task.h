
#ifndef DISPLAY_TASK_H_
#define DISPLAY_TASK_H_

typedef struct menu_item{
	char text[17];
	struct menu_item* child;
	struct menu_item* parent;
	struct menu_item* next;
	uint8_t num_child;
	uint8_t is_value;
	uint8_t value;
	uint8_t is_switch;
	uint8_t switch_value;
}menu_item_t;

void display_task();
uint8_t get_text_length(char* text, uint8_t max_len);
void display_tuer_open(void);
void display_tuer_closed(void);
void display_menu_init(void);

extern menu_item_t settings_menu;
extern menu_item_t *current_enty;
extern uint8_t field_position;
extern uint8_t max_field_position;
extern menu_item_t settings_temperatures;
extern menu_item_t settings_cal_oil_temperature;
extern menu_item_t settings_cal_in_temperature;
extern menu_item_t settings_cal_gearbox_temperature;
extern menu_item_t settings_cal_ambient_temperature;
//menu_item_t settings_cal_water_temperature; //85;

extern menu_item_t settings_mfa_values;
extern menu_item_t settings_cal_voltage; // 171
extern menu_item_t settings_cal_speed; // 169
extern menu_item_t settings_cal_consumption;

extern menu_item_t settings_options;
extern menu_item_t settings_cal_k15_delay;
extern menu_item_t settings_cal_k58b;
extern menu_item_t settings_cal_k58b_off_val;
extern menu_item_t settings_cal_k58b_on_val;
extern menu_item_t settings_cal_can_mode;
extern menu_item_t settings_cal_startstop_enabled;
#endif