#include "pebble.h"
#include "num2words.h"

#define BUFFER_SIZE 86

#ifdef CAPITAL
#define HEIGHT_CORRECTION 0
#elif defined HALF_CAPITAL
#define HEIGHT_CORRECTION 0
#else
#define HEIGHT_CORRECTION 5
#endif

static struct CommonWordsData {
  TextLayer *label;
  Window *window;
  char buffer[BUFFER_SIZE];
} s_data;

static PropertyAnimation *fade_animation;
static GRect frame;
static GFont font;
static Layer *root_layer;
static uint16_t layer_correct_position=0;

/* init the animation, also used to renew the lyer_current_position for a
 * correct fade animation*/
static void init_animation (void)
{
  /*create the "from" frame*/
  GRect frame_from;
  frame_from.origin.x=144;
  frame_from.origin.y=layer_correct_position;
  frame_from.size.h=frame.size.h;
  frame_from.size.w=frame.size.w;
  /*create the "to" frame*/
  GRect frame_to;
  frame_to.origin.x=0;
  frame_to.origin.y=layer_correct_position;
  frame_to.size.h=frame.size.h;
  frame_to.size.w=frame.size.w;
  Layer *layer = text_layer_get_layer(s_data.label);
  fade_animation=property_animation_create_layer_frame(layer, &frame_from, &frame_to);
  animation_set_duration((Animation*) fade_animation, 400);
  animation_set_curve((Animation*) fade_animation, AnimationCurveEaseIn);
  animation_set_delay((Animation*) fade_animation, 0);
}

/*this function resets the layer in a new position. If required, the layer is
 * destroyed before being reset. This is required, if it has not been created
 * already*/
static void reset_layer (int16_t x_pos, int16_t y_pos, int16_t width, int16_t height, bool destr)
{
  if (destr == true)
  {
    text_layer_destroy(s_data.label);
  }
  s_data.label = text_layer_create(GRect(x_pos, y_pos, width, height ));
  text_layer_set_background_color(s_data.label, GColorBlack);
  text_layer_set_text_color(s_data.label, GColorWhite);
  text_layer_set_font(s_data.label, font);
  text_layer_set_text_alignment(s_data.label, GTextAlignmentCenter);
  layer_add_child(root_layer, text_layer_get_layer(s_data.label));
}

// this function alligns the text vertically
static void align_vert ( void )
{
  //init pos variable
  //uint16_t new_pos = 0;
  //get the size of the text
  GSize current_size = text_layer_get_content_size (s_data.label);
  //calculate the new centered pos
  layer_correct_position = (((168 - current_size.h)/2) - HEIGHT_CORRECTION);
  //layer_correct_position = new_pos;
  //set the new size
  reset_layer(0, layer_correct_position, frame.size.w, (frame.size.h /*- (new_pos)*/),true);
}

static void update_time(struct tm* t) {
  fuzzy_time_to_words(t->tm_hour, t->tm_min, s_data.buffer, BUFFER_SIZE);
  text_layer_set_text(s_data.label, s_data.buffer);
  align_vert();
  text_layer_set_text(s_data.label, s_data.buffer);
  init_animation();
  animation_schedule((Animation*) fade_animation);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
}

static void do_init(void)
{
  s_data.window = window_create();
  const bool animated = true;
  window_stack_push(s_data.window, animated);

  window_set_background_color(s_data.window, GColorBlack);
  //GFont font = fonts_get_system_font(FONT_KEY_DROID_SERIF_28_BOLD);
  //GFont font = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
  //font = fonts_get_system_font(FONT_KEY_DROID_SERIF_28_BOLD);
  font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_CaviarDreamsBold_29));

  //Layer *root_layer = window_get_root_layer(s_data.window);
  root_layer = window_get_root_layer(s_data.window);
  //GRect frame = layer_get_frame(root_layer);
  frame = layer_get_frame(root_layer);

  reset_layer(0, 0, frame.size.w, frame.size.h,false);
/*
  s_data.label = text_layer_create(GRect(0, 0, frame.size.w, frame.size.h ));//- 20));
  text_layer_set_background_color(s_data.label, GColorBlack);
  text_layer_set_text_color(s_data.label, GColorWhite);
  text_layer_set_font(s_data.label, font);
  text_layer_set_text_alignment(s_data.label, GTextAlignmentCenter);
  layer_add_child(root_layer, text_layer_get_layer(s_data.label));
*/
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  /*init the animation*/
  init_animation();
  /*update the time for the first time*/
  update_time(t);

  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
}

static void do_deinit(void) {
  window_destroy(s_data.window);
  text_layer_destroy(s_data.label);
  property_animation_destroy(fade_animation);
}

int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}
