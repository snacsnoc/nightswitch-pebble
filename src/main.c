#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer;
static BitmapLayer *s_sun_image_layer, *s_moon_image_layer;
static GBitmap *s_sun_image_bitmap, *s_moon_image_bitmap;

//Set to white background with black text
static void set_whitebg() {
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorBlack);

    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text_color(s_date_layer, GColorBlack);

    window_set_background_color(s_main_window, GColorWhite);
    //Hide the moon layer, display the sun layer
    layer_set_hidden(bitmap_layer_get_layer(s_moon_image_layer), true);
    layer_set_hidden(bitmap_layer_get_layer(s_sun_image_layer), false);
}

//Set to black background with white text
static void set_blackbg() {
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorWhite);

    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text_color(s_date_layer, GColorWhite);

    window_set_background_color(s_main_window, GColorBlack);
    //Hide the sun layer, display the moon layer
    layer_set_hidden(bitmap_layer_get_layer(s_moon_image_layer), false);
    layer_set_hidden(bitmap_layer_get_layer(s_sun_image_layer), true);
}


static void update_time() {
    //Get a tm structure
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    // Create some buffers
    static char buffer[10];
    static char am_pm_buffer[] = "am";
    static char date_buffer[10];
    
    //Set morning and night times
    static char morning_time[] = "08:00";
    static char night_time[] = "23:00";
    static char night_time_twelve[] = "11:00";


    // Write the current hours and minutes into the buffer
    if(clock_is_24h_style() == true) {
        //Use 24h hour format
        strftime(buffer, sizeof(buffer), "%H:%M", tick_time);
    } else {
        //Use 12 hour format
        strftime(buffer, sizeof(buffer), "%I:%M", tick_time);
        //Here we get am or pm so we know if it is past noon/midnight 
        strftime(am_pm_buffer, sizeof(am_pm_buffer), "%p", tick_time);

    }
    
    strftime(date_buffer, sizeof(date_buffer), "%d %a", tick_time);
  
    //Display the time
    text_layer_set_text(s_time_layer, buffer);
    //Display the date
    text_layer_set_text(s_date_layer, date_buffer);

    //Change background/text colour based on night_time and morning_time
    if(clock_is_24h_style() == true) {
      
        //Current time and night_time are equal (watchface is active)
        if (strncmp(buffer, night_time, sizeof buffer) == 0 ) {
          set_blackbg();
        }
            //Current time greater than night_time (watchface not active)
        if (strncmp(buffer, night_time, sizeof buffer) > 0 ) {
          set_blackbg();
        } 
            //Current time is less than morning time (watchface not active)
        if (strncmp(buffer, morning_time, sizeof buffer) < 0 ) {
          set_blackbg();
        } 
            //Current time and morning_time are equal (watchface is active)
        if (strncmp(buffer, morning_time, sizeof buffer) == 0 ) {
          set_whitebg();
        }

    //If we are 12 hour
    } else {
        //Current time and night_time are equal (watchface is active) and is PM
        if (strncmp(buffer, night_time_twelve, sizeof buffer) == 0 && strncmp(am_pm_buffer, "pm", sizeof buffer) == 0) {
          set_blackbg();
        }
            //Current time greater than night_time (watchface not active) and is PM
        if (strncmp(buffer, night_time_twelve, sizeof buffer) > 0 && strncmp(am_pm_buffer, "pm", sizeof buffer) == 0) {
          set_blackbg();
        } 
      
        //Current time is less than morning time (watchface not active) and is AM
        if (strncmp(buffer, morning_time, sizeof buffer) < 0 && strncmp(am_pm_buffer, "am", sizeof buffer) == 0) {
          set_blackbg();
        }
                    //Current time is greater than morning time (watchface not active) and is AM
        if (strncmp(buffer, morning_time, sizeof buffer) > 0 && strncmp(am_pm_buffer, "am", sizeof buffer) == 0) {
          set_whitebg();
        } 
                        //Current time and morning_time are equal (watchface is active) and is AM
        if (strncmp(buffer, morning_time, sizeof buffer) == 0 && strncmp(am_pm_buffer, "am", sizeof buffer) == 0) {
          set_whitebg();
        } 
    }
}

static void main_window_load(Window *window) {
    //Create time TextLayer
    s_time_layer = text_layer_create(GRect(0, 20, 144, 50));
    text_layer_set_text(s_time_layer, "00:00");

    //Set font and center align
    //For list of fonts: https://gist.github.com/snacsnoc/5fa6d1329f2b1a96da2e
    text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    
    
    //Create date TextLayer
    s_date_layer = text_layer_create(GRect(0, 105, 155, 50));
    text_layer_set_text(s_date_layer, "Date");


    //Set font and center align
    text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

    //Create sun image layer
    s_sun_image_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SUN);
    s_sun_image_layer = bitmap_layer_create(GRect(58, 72, 32, 32));
    bitmap_layer_set_bitmap(s_sun_image_layer, s_sun_image_bitmap);
    
    //Create moon image layer
    s_moon_image_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MOON);
    s_moon_image_layer = bitmap_layer_create(GRect(58, 72, 32, 32));
    bitmap_layer_set_bitmap(s_moon_image_layer, s_moon_image_bitmap);

    //Add it as a child layer to the Window's root layer
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_moon_image_layer));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_sun_image_layer));
  
    //Make sure the time is displayed from the start
    update_time();
}

static void main_window_unload(Window *window) {
    //Destroy TextLayer
    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_date_layer);
    
    //Destroy BitmapLayer
    bitmap_layer_destroy(s_sun_image_layer);
    bitmap_layer_destroy(s_moon_image_layer);
    
    //Destroy the gBitmap
    gbitmap_destroy(s_sun_image_bitmap);
    gbitmap_destroy(s_moon_image_bitmap);

}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    update_time();
}

static void init() {
    // Create main Window element and assign to pointer
    s_main_window = window_create();

    // Set handlers to manage the elements inside the Window
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
         .unload = main_window_unload
    });

    //Show the Window on the watch
    window_stack_push(s_main_window, true);

    //Register with TickTimerService
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
    //Destroy Window
    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
