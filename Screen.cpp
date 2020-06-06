#include "Screen.h"
/********************* Screen class *********************/
Screen::Screen(){
	// Initialize in derived classes
}
void Screen::update_screen_chars(char screen_chars[SCH_UI_LCD_ROWS][SCH_UI_LCD_COLS+1]){
	// Update text
	update_text();
	
	// Load text to screen_chars
	for(int i = 0; i < SCH_UI_LCD_ROWS; i++){
		// Copy text to screen_chars if possible
		int text_row = row_offset+i;
		if(text_row >= number_of_rows){
			// no need to print more
			screen_chars[i][0] = '\n';
		} else
			// Copy and paste
			strcpy(screen_chars[i], text[row_offset + i]);
	}
	
	// Load cursor
	if(show_cursor){
		screen_chars[cursor_row-row_offset][0] = CURSOR_ICON;
	}
}
void Screen::update_row_offset(){
	// update row offset if cursor is off screen
	if(row_offset > cursor_row)
		// row offset is too big
		row_offset = cursor_row;
	else if(row_offset + SCH_UI_LCD_ROWS - 1 < cursor_row)
		// row offset is too small
		row_offset = cursor_row - SCH_UI_LCD_ROWS + 1;
}
void Screen::limit_cursor(){
	if(cursor_row >= number_of_rows)
		cursor_row= number_of_rows - 1;
	if(cursor_row < cursor_row_min)
		cursor_row = cursor_row_min;
}
void Screen::increment_cursor(){
	cursor_row++;
	limit_cursor();
	update_row_offset();
}
void Screen::decrement_cursor(){
	cursor_row--;
	limit_cursor();
	update_row_offset();
}
void Screen::reset_cursor(){
	cursor_row = 0;
	update_row_offset();
}
int Screen::get_cursor(){
	return cursor_row;
}

/********************* VL screen *********************/
VL_Screen::VL_Screen(LoadRegulator::LR_state &LR_state_r, TempRegulator::TR_state &TR_state_r): 
	_LR_state(LR_state_r),
	_TR_state(TR_state_r)
{
	// Initialize data
	row_offset = 0;
	cursor_row = 0;
	cursor_row_min = 0;
	show_cursor = false;
	number_of_rows = VL_SIZE;
	
	strcpy(text[0], "00.00 V\n");
	strcpy(text[1], "00.00 A\n");
	strcpy(text[2], "000.00 W\n");
	strcpy(text[3], "OFF             000%");
	
}
void VL_Screen::update_text(){
	// Update voltage, current, power, mode of variable load
	// Update Duty cycle of fan
	// Update measured voltage
	dtostrf(_LR_state._measured_voltage, SET_UI_MEASURED_VOLT_WIDTH, SET_UI_MEASURED_VOLT_DECIMAL, text[0]);
	strcat(text[0], " V\n");
	// Update measured current
	dtostrf(_LR_state._measured_current, SET_UI_MEASURED_CURR_WIDTH, SET_UI_MEASURED_CURR_DECIMAL, text[1]);
	strcat(text[1], " A\n");
	// Update measured power
	float measured_power = _LR_state._measured_voltage * _LR_state._measured_current;
	dtostrf(measured_power, SET_UI_MEASURED_POW_WIDTH, SET_UI_MEASURED_POW_DECIMAL, text[2]);
	strcat(text[2], " W\n");
	// Update mode
	switch(_LR_state._op_mode){
		case(LoadRegulator::CC):
			strcpy(text[3], "CC ");
			break;
		case(LoadRegulator::CP):
			strcpy(text[3], "CP ");
			break;
		case(LoadRegulator::CR):
			strcpy(text[3], "CR ");
			break;
		case(LoadRegulator::CV):
			strcpy(text[3], "CV ");
			break;
		case(LoadRegulator::OFF):
			strcpy(text[3], "OFF");
			break;
	}
	// Update duty cycle
	strcat(text[3], "            ");
	itoa(_TR_state._duty_cycle, text[3]+15, 10);
	strcat(text[3], "%\n");
}
Screen::SCREEN_ID VL_Screen::handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn){
	// Push toggle output enable
	if(btn == Encoder::PUSH){
		// Toggle output enable, don't change screen
		return Screen::VL_SCREEN;
	}
	
	// Long push enters main menu
	if(btn == Encoder::LONG_PUSH){
		// Go to Main menu
		return Screen::MAIN_MENU_SCREEN;
	}
	
	// If no input, remain on current screen
	return Screen::VL_SCREEN;
}
/********************* Main Menu screen *********************/
Main_Menu_Screen::Main_Menu_Screen(){
	// Initialize data
	row_offset = 0;
	cursor_row = 1;
	cursor_row_min = 1;
	show_cursor = true;
	number_of_rows = MAIN_MENU_SIZE;
	
	strcpy(text[0], "Main menu\n");
	strcpy(text[1], " Set mode\n");
	strcpy(text[2], " Set target\n");
	strcpy(text[3], " Debugger\n");
	strcpy(text[4], " Fan Control\n");
	strcpy(text[5], " Info\n");
	
}
void Main_Menu_Screen::update_text(){
	// No need to update text
}
Screen::SCREEN_ID Main_Menu_Screen::handle_input(Encoder::Encoder_Dir dir, Encoder::Encoder_Button btn){
	// Push to go to submenu
	if(btn == Encoder::PUSH){
		// Change screen; WIP
// 		if(get_cursor() == 1)
// 			return Screen::MENU_SCREEN;
// 		else if(get_cursor() == 2)
// 			return Screen::TEST_SCREEN;
	}
	
	// Long push returns to VL screen
	if(btn == Encoder::LONG_PUSH){
		// Go to VL Screen
		return Screen::VL_SCREEN;
	}
	
	// dir increments or decrements cursor row
	if(dir == Encoder::CLOCKWISE)
		increment_cursor();
	else if(dir == Encoder::COUNTERCLOCKWISE)
		decrement_cursor();
	
	// don't change screens
	return Screen::MAIN_MENU_SCREEN;
}
