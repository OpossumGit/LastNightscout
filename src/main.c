#include <pebble.h>

static Window *s_window;
static GFont s_res_gothic_18;
static GFont s_res_bitham_42_medium_numbers;
static GFont s_res_gothic_14;
static GFont s_res_gothic_24_bold;
static TextLayer *s_textlayer_day;
static TextLayer *s_textlayer_time;
static TextLayer *s_textlayer_date;
static TextLayer *s_textlayer_disconnected;

static Layer *s_battery_layer;
static int s_battery_level;

static TextLayer *s_sgv1_layer;
static TextLayer *s_sgv2_layer;
static TextLayer *s_sgv3_layer;
static TextLayer *s_time1_layer;
static TextLayer *s_time2_layer;
static TextLayer *s_time3_layer;

static char url[60] = "https://nightscout.azurewebsites.net";
static char units[5] = "mmol";

static void battery_callback(BatteryChargeState state) {
  // Record the new battery level
  s_battery_level = state.charge_percent;
  // Update meter
  layer_mark_dirty(s_battery_layer);
}

static void battery_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  // Find the width of the bar
  int width = (int)(float)(((float)s_battery_level / 100.0F) * 114.0F);

  // Draw the background
  graphics_context_set_fill_color(ctx, GColorDarkGray);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // Draw the bar
  graphics_context_set_fill_color(ctx, GColorYellow);
  graphics_fill_rect(ctx, GRect(0, 0, width, bounds.size.h), 0, GCornerNone);
}

static void bluetooth_callback(bool connected) {
  // Show icon if disconnected
  layer_set_hidden(text_layer_get_layer(s_textlayer_disconnected), connected);

  if(!connected) {
    // Issue a vibrating alert
    vibes_long_pulse();
  }
}

static void getNightScoutData() {
     APP_LOG(APP_LOG_LEVEL_INFO, "Sending request to js");
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair   
    dict_write_cstring(iter, MESSAGE_KEY_URL, url);
    dict_write_cstring(iter, MESSAGE_KEY_UNITS, units);
    //dict_write_end(iter);

    // Send the message!
    app_message_outbox_send();
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");
  
  // Store incoming information
  static char sgv1_buffer[20];
  static char time1_buffer[20];
  static char sgv2_buffer[20];
  static char time2_buffer[20];
  static char sgv3_buffer[20];
  static char time3_buffer[20];
  
  // Read tuples for data
  Tuple *sgv1_tuple = dict_find(iterator, MESSAGE_KEY_SGV1);
  Tuple *time1_tuple = dict_find(iterator, MESSAGE_KEY_TIME1);
  Tuple *sgv2_tuple = dict_find(iterator, MESSAGE_KEY_SGV2);
  Tuple *time2_tuple = dict_find(iterator, MESSAGE_KEY_TIME2);
  Tuple *sgv3_tuple = dict_find(iterator, MESSAGE_KEY_SGV3);
  Tuple *time3_tuple = dict_find(iterator, MESSAGE_KEY_TIME3);
  
  if (sgv1_tuple){
    snprintf(sgv1_buffer, sizeof(sgv1_buffer), "%s", sgv1_tuple->value->cstring);
  }
 text_layer_set_text(s_sgv1_layer, sgv1_buffer);
  
  if (sgv2_tuple){
    snprintf(sgv2_buffer, sizeof(sgv2_buffer), "%s", sgv2_tuple->value->cstring);
  }
  text_layer_set_text(s_sgv2_layer, sgv2_buffer);

  if (sgv3_tuple){
    snprintf(sgv3_buffer, sizeof(sgv3_buffer), "%s", sgv3_tuple->value->cstring);
  }
  text_layer_set_text(s_sgv3_layer, sgv3_buffer);
  
  if (time1_tuple){
    snprintf(time1_buffer, sizeof(time1_buffer), "%s", time1_tuple->value->cstring);
  }
  text_layer_set_text(s_time1_layer, time1_buffer);

  if (time2_tuple){
    snprintf(time2_buffer, sizeof(time2_buffer), "%s", time2_tuple->value->cstring);
  }
  text_layer_set_text(s_time2_layer, time2_buffer);
  
  if (time3_tuple){
    snprintf(time3_buffer, sizeof(time3_buffer), "%s", time3_tuple->value->cstring);
  }
  text_layer_set_text(s_time3_layer, time3_buffer);
  
  // read URL config
  Tuple *url_tuple = dict_find(iterator, MESSAGE_KEY_URL);
  if(url_tuple) {
    APP_LOG(APP_LOG_LEVEL_INFO, url_tuple->value->cstring);
    persist_write_string(0, url_tuple->value->cstring);
    strncpy(url,url_tuple->value->cstring,60);
  }

  // Read UNITS config
  Tuple *units_tuple = dict_find(iterator, MESSAGE_KEY_UNITS);
  if(units_tuple) {
    APP_LOG(APP_LOG_LEVEL_INFO, units_tuple->value->cstring);
    persist_write_string(1, units_tuple->value->cstring);
    strncpy(units,units_tuple->value->cstring,5);
    getNightScoutData();
  }
  
  // if javascript informs us that ip is unknown following is performed
  Tuple *readytuple = dict_find(iterator, MESSAGE_KEY_READY);
	if(readytuple) {
    getNightScoutData();
  }

}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


static void main_window_load(Window *window) {
  s_res_gothic_18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  s_res_bitham_42_medium_numbers = fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS);
  s_res_gothic_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  s_res_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  
  // s_textlayer_day
  s_textlayer_day = text_layer_create(GRect(20, 3, 100, 30));
  text_layer_set_background_color(s_textlayer_day, GColorClear);
  text_layer_set_text_color(s_textlayer_day, GColorYellow);
  text_layer_set_text(s_textlayer_day, "Wednesday");
  text_layer_set_text_alignment(s_textlayer_day, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_day, s_res_gothic_18);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_day);
  
  // s_textlayer_time
  s_textlayer_time = text_layer_create(GRect(10, 11, 122, 57));
  text_layer_set_background_color(s_textlayer_time, GColorClear);
  text_layer_set_text_color(s_textlayer_time, GColorWhite);
  text_layer_set_text(s_textlayer_time, "12:59");
  text_layer_set_text_alignment(s_textlayer_time, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_time, s_res_bitham_42_medium_numbers);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_time);
  
  // s_textlayer_date
  s_textlayer_date = text_layer_create(GRect(20, 51, 100, 20));
  text_layer_set_background_color(s_textlayer_date, GColorClear);
  text_layer_set_text_color(s_textlayer_date, GColorYellow);
  text_layer_set_text(s_textlayer_date, "11.12.2016");
  text_layer_set_text_alignment(s_textlayer_date, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_date, s_res_gothic_18);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_date);
  
  // Create battery meter Layer
  s_battery_layer = layer_create(GRect(14, 147, 115, 2));
  layer_set_update_proc(s_battery_layer, battery_update_proc);
  layer_add_child(window_get_root_layer(window), s_battery_layer);
  
  // Create the TextLayer to display BT status
  s_textlayer_disconnected = text_layer_create(GRect(20, 150, 100, 20));
  text_layer_set_background_color(s_textlayer_disconnected, GColorDarkCandyAppleRed);
  text_layer_set_text_color(s_textlayer_disconnected, GColorWhite);
  text_layer_set_text(s_textlayer_disconnected, "disconnected");
  text_layer_set_text_alignment(s_textlayer_disconnected, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_disconnected, s_res_gothic_18);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_disconnected); 
  // Show the correct state of the BT connection from the start
  bluetooth_callback(connection_service_peek_pebble_app_connection());
  
  // Create nightscout Layer
  s_sgv1_layer = text_layer_create( GRect(20, 70, 100, 25));
  text_layer_set_background_color(s_sgv1_layer, GColorClear);
  text_layer_set_text_color(s_sgv1_layer, GColorWhite);
  text_layer_set_text_alignment(s_sgv1_layer, GTextAlignmentLeft);
  text_layer_set_text(s_sgv1_layer, "Loading... \U0001F603");
  text_layer_set_font(s_sgv1_layer, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_sgv1_layer);
  
  s_sgv2_layer = text_layer_create( GRect(20, 90, 100, 25));
  text_layer_set_background_color(s_sgv2_layer, GColorClear);
  text_layer_set_text_color(s_sgv2_layer, GColorWhite);
  text_layer_set_text_alignment(s_sgv2_layer, GTextAlignmentLeft);
  text_layer_set_font(s_sgv2_layer, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_sgv2_layer);

  s_sgv3_layer = text_layer_create( GRect(20, 110, 100, 25));
  text_layer_set_background_color(s_sgv3_layer, GColorClear);
  text_layer_set_text_color(s_sgv3_layer, GColorWhite);
  text_layer_set_text_alignment(s_sgv3_layer, GTextAlignmentLeft);
  text_layer_set_font(s_sgv3_layer, s_res_gothic_24_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_sgv3_layer);
  
  s_time1_layer = text_layer_create( GRect(70, 76, 52, 25));
  text_layer_set_background_color(s_time1_layer, GColorClear);
  text_layer_set_text_color(s_time1_layer, GColorLightGray);
  text_layer_set_text_alignment(s_time1_layer, GTextAlignmentRight);
  text_layer_set_font(s_time1_layer, s_res_gothic_18);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_time1_layer);
  
  s_time2_layer = text_layer_create( GRect(70, 96, 52, 25));
  text_layer_set_background_color(s_time2_layer, GColorClear);
  text_layer_set_text_color(s_time2_layer, GColorLightGray);
  text_layer_set_text_alignment(s_time2_layer, GTextAlignmentRight);
  text_layer_set_font(s_time2_layer, s_res_gothic_18);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_time2_layer);
  
  s_time3_layer = text_layer_create( GRect(70, 116, 52, 25));
  text_layer_set_background_color(s_time3_layer, GColorClear);
  text_layer_set_text_color(s_time3_layer, GColorLightGray);
  text_layer_set_text_alignment(s_time3_layer, GTextAlignmentRight);
  text_layer_set_font(s_time3_layer, s_res_gothic_18);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_time3_layer);
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_textlayer_day);
  text_layer_destroy(s_textlayer_time);
  text_layer_destroy(s_textlayer_date);
  layer_destroy(s_battery_layer);
  text_layer_destroy(s_textlayer_disconnected);
  text_layer_destroy(s_sgv1_layer);
  text_layer_destroy(s_sgv2_layer);
  text_layer_destroy(s_sgv3_layer);
  text_layer_destroy(s_time1_layer);
  text_layer_destroy(s_time2_layer);
  text_layer_destroy(s_time3_layer);
  
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
	app_message_deregister_callbacks();

}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H:%M" : "%I:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_textlayer_time, s_buffer);
  
  // Copy date into buffer from tm structure
  static char date_buffer[16];
  strftime(date_buffer, sizeof(date_buffer), "%d.%m.%Y",tick_time);

  // Show the date
  text_layer_set_text(s_textlayer_date, date_buffer);
  
  // Copy day into buffer from tm structure
  static char day_buffer[16];
  strftime(day_buffer, sizeof(day_buffer), "%A", tick_time);

  // Show the day
  text_layer_set_text(s_textlayer_day, day_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  
  // Get nightscout update every 10 minutes
  if(tick_time->tm_min % 10 == 0) {
    getNightScoutData();

  }
}


static void init() {
  // Create main Window element and assign to pointer
  s_window = window_create();
  window_set_background_color(s_window, GColorOxfordBlue);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Register for battery level updates
  battery_state_service_subscribe(battery_callback);
  
  // Register for Bluetooth connection updates
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_callback
  });
  
  // Register javascript callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  // Open AppMessage
  const int inbox_size = 128;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);

  // Show the Window on the watch, with animated=true
  window_stack_push(s_window, true);
  
  update_time();
  // Ensure battery level is displayed from the start
  battery_callback(battery_state_service_peek());
  
  // read persisted cfg
  persist_read_string(0, url, 60);
  persist_read_string(1, units, 5);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}