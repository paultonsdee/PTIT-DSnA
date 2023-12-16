#pragma once

#include "my_struct.h"


// --------------------------------------------
// |                  GUI                     |
// --------------------------------------------

bool init_SDL();
SDL_Texture* LoadTexture(const char*);
ImGuiIO& init_ImGui();
void display_message(const char*, const char*, const char*);
void renderImGui(ImGuiIO&);
void terminate(); // Frees media and shuts down
void draw_main_menu_screen();
void draw_aircraft_management_screen(PlaneList&, ImFont*&);
void popup_inputPlane(PlaneList&);
void draw_combo(const char*, const char* [], const int&, int&, const char*&);
void HelpMarker(const char*);
void add_plane_popup (PlaneList&, bool&, int &);
void show_action_buttons (PlaneList&, int&, bool&);
void show_delete_plane_popup(PlaneList&, int&);
void draw_flight_management_screen(FlightListPTR&, PlaneList&, ImFont*&);
void add_flight_popup(PlaneList&, bool&);

// --------------------------------------------
// |             Data Handling                |
// --------------------------------------------

int string_length(std::string&);
bool is_planeList_empty(PlaneList&);
bool is_planeList_full(PlaneList&);
bool insert_plane(PlaneList&, int&, char&, const char*&, int&, int&);
void save_plane(PlaneList&, char*);
// void save_aircraft(PlaneList&, char*);
void save_aircraft(PlaneList &, std::string&);
// void load_aircraft(PlaneList&, char*);
void edit_plane_popup(PlaneList&, bool&, int&);
void edit_plane(PlaneList&, int&, char [], const char*&, int&, int&, int&);
bool is_in_array(std::string&, const char*[], const int&);