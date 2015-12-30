#include <pebble.h>
#include "main.h"
/*
making a watchface around a TCAS display
use GPaths for targets
put altitudes next to targets for date and month
point targets at hour/minute
*/
static Window *window;
static Layer *bg_layer, *hands_layer;
static GPath *minute_target, *hour_target;

static void bg_create_proc(Layer *layer, GContext *ctx) {
	GRect bounds = layer_get_bounds(layer);
	GPoint center = grect_center_point(&bounds);

	graphics_context_set_stroke_width(ctx, 1);
	graphics_context_set_stroke_color(ctx, GColorBlue);
	graphics_context_set_text_color(ctx, GColorBlue);

	graphics_draw_circle(ctx, center, 64); // outer circle
	graphics_draw_circle(ctx, center, 33); // inner circle
	graphics_draw_text(ctx, "40", fonts_get_system_font(FONT_KEY_GOTHIC_09), GRect(0, 19, 144, 20), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
	
	graphics_draw_text(ctx, "20", fonts_get_system_font(FONT_KEY_GOTHIC_09), GRect(0, 50, 144, 20), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);


}

static void hands_update_proc(Layer *layer, GContext *ctx) {

	time_t temp = time(NULL);
	struct tm *t = localtime(&temp);

	static char date_buffer[] = "00";
	strftime(date_buffer, sizeof(date_buffer), "%d", t);

	static char month_buffer[] = "00";
	strftime(month_buffer, sizeof(month_buffer), "%m", t);

	GPoint center = GPoint(72, 84);

	int32_t minute_angle = TRIG_MAX_ANGLE * t->tm_min / 60;
	int minute_length = 66;
	GPoint minute_point = {
		.x = (int16_t)(sin_lookup(minute_angle) * (int32_t)minute_length / TRIG_MAX_RATIO) + center.x,
		.y = (int16_t)(-cos_lookup(minute_angle) * (int32_t)minute_length / TRIG_MAX_RATIO) + center.y,
	};

	graphics_context_set_fill_color(ctx, GColorRed);

	gpath_move_to(minute_target, minute_point);
	
	gpath_draw_filled(ctx, minute_target);
	
	graphics_context_set_text_color(ctx, GColorRed);

	graphics_draw_text(ctx, date_buffer, fonts_get_system_font(FONT_KEY_GOTHIC_09), GRect(minute_point.x - 7, minute_point.y + 4, 15, 20), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
	

	int32_t hour_angle = TRIG_MAX_ANGLE * t->tm_hour / 12;
	int hour_length = 33;
	GPoint hour_hand = {
		.x = (int16_t)(sin_lookup(hour_angle) * (int32_t)hour_length / TRIG_MAX_RATIO) + center.x,
		.y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)hour_length / TRIG_MAX_RATIO) + center.y,
	};
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "X: %d, Y: %d", (int)hour_hand.x, (int)hour_hand.y);

	graphics_context_set_fill_color(ctx, GColorYellow);

	gpath_move_to(hour_target, hour_hand);

	gpath_draw_filled(ctx, hour_target);

	graphics_context_set_text_color(ctx, GColorYellow);

	graphics_draw_text(ctx, month_buffer, fonts_get_system_font(FONT_KEY_GOTHIC_09), GRect(hour_hand.x - 7, hour_hand.y + 4, 15, 20), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	layer_mark_dirty(hands_layer);
}

static void main_window_load() {
	bg_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(bg_layer, bg_create_proc);
	layer_add_child(window_get_root_layer(window), bg_layer);

	hands_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(hands_layer, hands_update_proc);
	layer_add_child(window_get_root_layer(window), hands_layer);
}

static void main_window_unload() {
	layer_destroy(bg_layer);
	layer_destroy(hands_layer);
}

static void init() {
	window = window_create();
	
	window_set_background_color(window, GColorBlack);
	
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

	window_set_window_handlers(window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload,
	});

	window_stack_push(window, true);

	hour_target = gpath_create(&MINUTE_TARGET_POINTS);
	gpath_move_to(hour_target, GPoint(72, 84));

	minute_target = gpath_create(&HOUR_TARGET_POINTS);
	gpath_move_to(minute_target, GPoint(72, 84));
/*
	hour_target = gpath_create(&HOUR_TARGET_POINTS);
	gpath_move_to(hour_target, GPoint(72, 84));
*/
}

static void deinit() {
	gpath_destroy(minute_target);
	gpath_destroy(hour_target);
	window_destroy(window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
