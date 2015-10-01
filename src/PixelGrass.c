#include <pebble.h>

#define KEY_TEMPERATURE 99

static Window *s_main_window;

static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_battery_layer;
static TextLayer *s_weather_layer;

static GFont s_time_font;

static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;
static BitmapLayer *s_charging_layer;
static GBitmap *s_charging_bitmap;

static int currentTemp;

static char* KEY_WEATHER_UNITS;
static bool KEY_VIBRATIONS;
static int KEY_START_HOUR;
static int KEY_END_HOUR;

enum MessageKeys {
  MK_KEY_WEATHER_UNITS = 6,
  MK_KEY_VIBRATIONS = 5,
  MK_KEY_START_HOUR = 7,
  MK_KEY_END_HOUR = 22
};

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char buffer[] = "00:00";

  if(clock_is_24h_style() == true) {
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  text_layer_set_text(s_time_layer, buffer);

  static char bufferDate[] = "MON 00";
  strftime(bufferDate, sizeof("MON 00"), "%a %d", tick_time);
  text_layer_set_text(s_date_layer, bufferDate);

  int currentHour = tick_time->tm_hour;
  if (tick_time->tm_min == 0) {
    if (KEY_VIBRATIONS) {
      if (currentHour >= KEY_START_HOUR && currentHour <= KEY_END_HOUR) {
        vibes_double_pulse();
      }
    }
  }
}

static void battery_handler(BatteryChargeState charge_state) {
  static char s_battery_buffer[16];

  if(charge_state.is_charging) {
    s_battery_buffer[0] = 0;
  } else {
    snprintf(s_battery_buffer, sizeof(s_battery_buffer), "%d%%", charge_state.charge_percent);
  }
  layer_set_hidden(bitmap_layer_get_layer(s_charging_layer), !charge_state.is_charging);
  text_layer_set_text(s_battery_layer, s_battery_buffer);
}

void on_animation_stopped(Animation *anim, bool finished, void *context) {
    property_animation_destroy((PropertyAnimation*) anim);
}

void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay) {

    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);

    animation_set_duration((Animation*) anim, duration);
    animation_set_delay((Animation*) anim, delay);

    AnimationHandlers handlers = {
        .stopped = (AnimationStoppedHandler) on_animation_stopped
    };
    animation_set_handlers((Animation*) anim, handlers, NULL);

    animation_schedule((Animation*) anim);
}

static void init_animations() {
  GRect timestart = GRect(0, -60, 144, 168);
	GRect timefinish = GRect(0, 30, 144, 168);

  int animlen = 800;

	animate_layer(text_layer_get_layer(s_time_layer), &timestart, &timefinish, animlen, 0);
}

static void main_window_load(Window *window) {

  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));

  s_time_layer = text_layer_create(GRect(0, -60, 144, 168));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");

  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));

  s_date_layer = text_layer_create(GRect(0, 71, 144, 168));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text(s_date_layer, "MON 00");

  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));

  s_charging_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHARGING_WHITE);
  s_charging_layer = bitmap_layer_create(GRect(127, 2, 10, 20));
  bitmap_layer_set_bitmap(s_charging_layer, s_charging_bitmap);
  bitmap_layer_set_compositing_mode(s_charging_layer, GCompOpOr);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_charging_layer));

  s_battery_layer = text_layer_create(GRect(0, -1, 141, 160));
  text_layer_set_background_color(s_battery_layer, GColorClear);
  text_layer_set_text_color(s_battery_layer, GColorWhite);
  text_layer_set_text(s_battery_layer, "--%");

  text_layer_set_font(s_battery_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_battery_layer, GTextAlignmentRight);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_battery_layer));
  battery_handler(battery_state_service_peek());

  s_weather_layer = text_layer_create(GRect(0, 90, 144, 90));
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_layer, "");

  text_layer_set_font(s_weather_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));

  update_time();
}

static void main_window_unload(Window *window) {

  gbitmap_destroy(s_background_bitmap);
  bitmap_layer_destroy(s_background_layer);

  fonts_unload_custom_font(s_time_font);
  text_layer_destroy(s_time_layer);

  text_layer_destroy(s_date_layer);

  text_layer_destroy(s_battery_layer);

  gbitmap_destroy(s_charging_bitmap);
  bitmap_layer_destroy(s_charging_layer);

  text_layer_destroy(s_weather_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();

  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    dict_write_uint8(iter, 0, 0);

    app_message_outbox_send();
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success");
}

static int getHourInt(char * hourString) {

  int hour = atoi(hourString);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "hour %d", hour);
  return hour;
}

static void updateTemperature() {
  static char temperature_buffer[8];
  static char weather_layer_buffer[32];

  if(strcmp(KEY_WEATHER_UNITS, "f") == 0) {
    int temperature = currentTemp;
    temperature = temperature*1.8+32;
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%dF", temperature);
  } else {
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", currentTemp);
  }

  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s", temperature_buffer);
  text_layer_set_text(s_weather_layer, weather_layer_buffer);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {

  Tuple *t = dict_read_first(iterator);

  while(t != NULL) {

    switch(t->key) {
      case KEY_TEMPERATURE:
        currentTemp = (int) t->value->int32;
        updateTemperature();
        break;
      case MK_KEY_WEATHER_UNITS:
        if (strcmp(t->value->cstring, "f") == 0) {
          KEY_WEATHER_UNITS = "f";
          persist_write_string(MK_KEY_WEATHER_UNITS, t->value->cstring);
        } else {
          KEY_WEATHER_UNITS = "c";
          persist_write_string(MK_KEY_WEATHER_UNITS, "c");
        }
        updateTemperature();
        break;
      case MK_KEY_VIBRATIONS:
        if (strcmp(t->value->cstring, "on") == 0) {
          KEY_VIBRATIONS = true;
          persist_write_bool(MK_KEY_VIBRATIONS, true);
        } else {
          KEY_VIBRATIONS = false;
          persist_write_bool(MK_KEY_VIBRATIONS, false);
        }
        break;
      case MK_KEY_START_HOUR:
        KEY_START_HOUR = getHourInt(t->value->cstring);
        persist_write_string(MK_KEY_START_HOUR, t->value->cstring);
        break;
      case MK_KEY_END_HOUR:
        KEY_END_HOUR = getHourInt(t->value->cstring);
        persist_write_string(MK_KEY_END_HOUR, t->value->cstring);
        break;
      default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized", (int)t->key);
        break;
    }

    t = dict_read_next(iterator);
  }
}

static void init() {

  char * weatherBuffer = "-";
  char * strBuffer = "--";

  if (persist_exists(MK_KEY_WEATHER_UNITS)) {
    persist_read_string(MK_KEY_WEATHER_UNITS, weatherBuffer, sizeof(weatherBuffer));
    KEY_WEATHER_UNITS = weatherBuffer;
  }
  else {
    KEY_WEATHER_UNITS = "c";
  }

  if (persist_exists(MK_KEY_VIBRATIONS)) {
    KEY_VIBRATIONS = persist_read_bool(MK_KEY_VIBRATIONS);
  }
  else {
    KEY_VIBRATIONS = false;
  }

  if (persist_exists(MK_KEY_START_HOUR)) {
    persist_read_string(MK_KEY_START_HOUR, strBuffer, sizeof(strBuffer));
    KEY_START_HOUR = getHourInt(strBuffer);
  }
  else {
    KEY_START_HOUR = 7;
  }

  if (persist_exists(MK_KEY_END_HOUR)) {
    persist_read_string(MK_KEY_END_HOUR, strBuffer, sizeof(strBuffer));
    KEY_END_HOUR = getHourInt(strBuffer);
  }
  else {
    KEY_END_HOUR = 22;
  }

  s_main_window = window_create();

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_stack_push(s_main_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  battery_state_service_subscribe(battery_handler);

  init_animations();
}

static void deinit() {

  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
