

#define EDGE_BUFFER 5


void display_text_adv(String string_to_display, int colour, const GFXfont* font, int x, int y) {
  if (font == NULL) display.setFont();
  else display.setFont(font);
  display.setTextColor(colour);
  display.setCursor(x, y);
  display.println(string_to_display);
}

void get_text_dimensions(String string_to_display, const GFXfont* font, uint16_t* w, uint16_t* h) {
  int16_t  x1, y1;
  int x, y = 0;
  display.setFont(font);
  display.getTextBounds(string_to_display, x, y, &x1, &y1, w, h);
}

void no_data_display() {
  uint16_t w, h;
  const GFXfont* font = f18b;
  String no_data = "No Data";
  display.fillScreen(GxEPD_WHITE);
  get_text_dimensions(no_data, font, &w, &h);
  display_text_adv(no_data, GxEPD_BLACK, font, (DISPLAY_WIDTH/2) - (w/2), (DISPLAY_HEIGHT/2));
}

