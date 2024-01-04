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
void draw_aircraft_management_screen(PlaneList &, FlightNodePTR &, ImFont *&);
void popup_inputPlane(PlaneList &);
void draw_combo(const char *, const char *[], const int &, int &, const char *&);
void HelpMarker(const char *);
void add_plane_popup(PlaneList &, bool &, int &, int &);
void show_AM_action_buttons(PlaneList &, FlightNodePTR &, int &, int &, bool &, ImVec2 &);
void delete_plane_popup(PlaneList &, int &, int &, bool &);
void draw_flight_management_screen(FlightNodePTR &, PlaneList &, ImFont *&);
void add_flight_popup(FlightNodePTR &, PlaneList &, bool &, int &);
void draw_combo(const char *, PlaneList &, int &, const char *&, int &);
void find_max_day_in_month(int *, int &, int &);
void show_FM_action_buttons(FlightNodePTR &, PlaneList &, PassengerNodesPTR &, int &, bool &, int &, ImVec2 &);
void show_noti (const std::string &);
void confirm_change_status(FlightNodePTR &, int &, int &, int &);
void edit_flight_popup(FlightNodePTR &, int &, int &, bool &);
void book_ticket_popup(FlightNodePTR &, PlaneList &, PassengerNodesPTR &, int &, int &, bool &);
void input_passengerID_popup (PassengerNodesPTR &, FlightNodePTR &, int &, bool& , bool& , bool&, bool&);
void add_passenger_popup(PassengerNodesPTR &, FlightNodePTR &, int &, char (&)[13], bool &, bool &, bool &, bool&, bool&);
void confirm_id (char (&)[13], bool &, bool &);
void ticket_and_passenger_popup (FlightNodePTR &, PassengerNodesPTR &, int &, int &, bool &);
void confirm_cancel_ticket (FlightNodePTR &, int &, int &);
void ranking_table_popup(PlaneList &, FlightNodePTR &, bool &);

// --------------------------------------------
// |             Data Handling                |
// --------------------------------------------

bool is_decimal(char &);
bool is_alpha (char &);
int string_length(std::string &);
bool is_planeList_empty(PlaneList &);
bool is_planeList_full(PlaneList &);
bool insert_plane(PlaneList &, int &, char &, const char *&, int &, int &);
void save_plane(PlaneList &, char *);
// void save_aircraft(PlaneList&, char*);
void save_aircraft(PlaneList &, std::string &);
// void load_aircraft(PlaneList&, char*);
void edit_plane_popup(PlaneList &, FlightNodePTR &, bool &, int &);
bool edit_plane(PlaneList &, FlightNodePTR &, int &, char[], const char *&, int &, int &, int &);
bool is_in_array(std::string &, const char *[], const int &);
void link_list_initialize(FlightNodePTR &);
void insert_flight(FlightNodePTR &, const char *&, char (&)[5], const char *&, PlaneList &, int &, int &, int &, int &, int &, int &);
FlightNodePTR new_flight (void);
int count_flights (FlightNodePTR );
void save_flight(FlightNodePTR &, std::string &);
void load_flight(FlightNodePTR &, std::string &);
void init_row_arr();
void save_ticket (FlightNodePTR &, std::string &);
void load_ticket (FlightNodePTR &, std::string &);
void tree_initialize(PassengerNodesPTR &);
void insert_passenger(PassengerNodesPTR &, const std::string &, const std::string &, const std::string &, const bool &);
bool is_passengerList_empty(PassengerNodesPTR &, const std::string &);
void inorder_to_file(PassengerNodesPTR &, std::ofstream &);
void save_passenger(PassengerNodesPTR &, std::string &);
void load_passenger(PassengerNodesPTR &, std::string &);
void book_ticket (FlightNodePTR &, const std::string &D, int &);
PassengerNodesPTR search_passenger(PassengerNodesPTR &, const std::string &);
