#pragma once

#include "my_struct.h"

// --------------------------------------------
// |                  GUI                     |
// --------------------------------------------

bool init_SDL();
SDL_Texture *LoadTexture(const char *);
ImGuiIO &init_ImGui();
void display_message(const char *, const char *, const char *);
void renderImGui(ImGuiIO &);
void terminate(); // Frees media and shuts down
void draw_main_menu_screen();
void draw_aircraft_management_screen(PlaneList &, ImFont *&);
void popup_inputPlane(PlaneList &);
void draw_combo(const char *, const char *[], const int &, int &, const char *&);
void HelpMarker(const char *);
void add_plane_popup(PlaneList &, bool &, int &);
void show_action_buttons(PlaneList &, int &, bool &);
void show_delete_plane_popup(PlaneList &, int &);
void draw_flight_management_screen(FlightNodePTR &, PlaneList &, ImFont *&);
void add_flight_popup(FlightNodePTR &, PlaneList &, bool &);
void draw_combo(const char *, PlaneList &, int &, const char *&, int &);
void find_max_day_in_month(int *, int &, int &);

// --------------------------------------------
// |             Data Handling                |
// --------------------------------------------

int string_length(std::string &);
bool is_planeList_empty(PlaneList &);
bool is_planeList_full(PlaneList &);
bool insert_plane(PlaneList &, int &, char &, const char *&, int &, int &);
void save_plane(PlaneList &, char *);
// void save_aircraft(PlaneList&, char*);
void save_aircraft(PlaneList &, std::string &);
// void load_aircraft(PlaneList&, char*);
void edit_plane_popup(PlaneList &, bool &, int &);
void edit_plane(PlaneList &, int &, char[], const char *&, int &, int &, int &);
bool is_in_array(std::string &, const char *[], const int &);
void link_list_initialize(FlightNodePTR &);
void insert_flight(FlightNodePTR &, const char *&, char (&)[5], const char *&, PlaneList &, int &, int &, int &, int &, int &, int &);
FlightNodePTR new_flight (void);
int count_flights (FlightNodePTR );
void save_flight(FlightNodePTR &, std::string &);
void load_flight(FlightNodePTR &, std::string &);