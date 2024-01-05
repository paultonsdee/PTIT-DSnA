#include "Manipulation/my_dHandling.h"
#include "Manipulation/my_gui.h"

int main(int, char **)
{
	// Our state
	// bool show_another_window = false;
	// std::string filename = "aircraft.dat";
	PlaneList planeList;

	FlightNodePTR pFirstFlight = NULL;
	link_list_initialize(pFirstFlight);

	PassengerNodesPTR treeRoot;
	tree_initialize(treeRoot);

	if (!init_SDL())
	{
		std::cout << "Failed to initialize!\n";
		display_message("e", "Error", "Failed to initialize!");
	}
	else
	{
		init_row_arr();
		load_aircraft(planeList, aircraftFile);
		load_flight(pFirstFlight, flightFile);
		load_ticket(pFirstFlight, ticketFile);
		load_passenger(treeRoot, passengerFile);

		ImGuiIO &io = init_ImGui();

		io.Fonts->AddFontFromFileTTF("Media/Roboto-Medium.ttf", 16.0f);
		io.FontDefault = io.Fonts->Fonts.back();
		ImFont *special_font = io.Fonts->AddFontFromFileTTF("Media/PassionOne-Bold.ttf", 37.0f);
		noti_font = io.Fonts->AddFontFromFileTTF("Media/Roboto-Medium.ttf", 25.0f);
		popup_header_font = io.Fonts->AddFontFromFileTTF("Media/PassionOne-Bold.ttf", 20.0f);

		// show_popup_noti("Welcome to Flight Management System! Press CTRL + H to return to main menu!");

		bool running = true;
		SDL_Event ev;

		// Main loop
		while (running)
		{
			// Start the Dear ImGui frame
			ImGui_ImplSDLRenderer2_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

			while (SDL_PollEvent(&ev))
			{
				ImGui_ImplSDL2_ProcessEvent(&ev);
				if (ev.type == SDL_QUIT || (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_CLOSE && ev.window.windowID == SDL_GetWindowID(gWindow)))
					running = false;
				else if (ev.type == SDL_KEYUP)
				{
					switch (ev.key.keysym.sym)
					{
					case SDLK_ESCAPE:
						running = false;
						break;
					case SDLK_RETURN:
						break;
					case SDLK_SPACE:
						// SDL_Window* menuWindow = SDL_CreateWindow("Menu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
						break;
					case SDLK_LCTRL: // test case

						break;
					}
				}
				else if (ev.type == SDL_KEYDOWN)
				{
					const Uint8 *state = SDL_GetKeyboardState(NULL);
					if (state[SDL_SCANCODE_S] && (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]))
					{
						save_aircraft(planeList, aircraftFile);
						save_flight(pFirstFlight, flightFile);
						save_ticket(pFirstFlight, ticketFile);
						save_passenger(treeRoot, passengerFile);
						show_noti("save all data successfully!");
					}
					else if (state[SDL_SCANCODE_T] && (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]))
					{
						if (planeList.totalPlane == 0)
							std::cout << "Plane list is empty" << std::endl
									  << "----------" << std::endl;
						else
							std::cout << "Planelist:" << std::endl
									  << "----------" << std::endl;
						for (int i = 0; i < planeList.totalPlane; i++)
						{
							Plane *plane = planeList.nodes[i];
							std::cout << plane->planeID << " | ";
							std::cout << plane->planeType << " | ";
							std::cout << plane->seatNum << " | ";
							std::cout << plane->rowNum;
							std::cout << std::endl;
						}
						// std::cout << "----------" << std::endl;
						// if (count_flights(pFirstFlight) == 0)
						// 	std::cout << "Flight list is empty" << std::endl
						// 			  << "----------" << std::endl;
						// else
						// 	std::cout << "Flightlist:" << std::endl
						// 			  << "----------" << std::endl;
						// for (FlightNodePTR p = pFirstFlight; p != NULL; p = p->next)
						// {
						// 	std::cout << p->flight.flightNumber << " | ";
						// 	std::cout << p->flight.desAirport << " | ";
						// 	std::cout << p->flight.stt << " | ";
						// 	std::cout << p->flight.planeID << " | ";
						// 	std::cout << p->flight.totalTicket << " | ";
						// 	std::cout << p->flight.maxTicket;
						// 	std::cout << std::endl
						// 			  << "\t\t";
						// 	std::cout << p->flight.departureTime.day << "/";
						// 	std::cout << p->flight.departureTime.month << "/";
						// 	std::cout << p->flight.departureTime.year << " ";
						// 	std::cout << p->flight.departureTime.hour << ":";
						// 	std::cout << p->flight.departureTime.minute;
						// 	std::cout << std::endl;

						// 	for (int i = 0; i < p->flight.maxTicket; i++)
						// 	{
						// 		std::cout << p->flight.ticketList[i].ticketID << " | ";
						// 		std::cout << p->flight.ticketList[i].passengerID << " | ";
						// 		std::cout << p->flight.ticketList[i].inUse;
						// 		std::cout << "\t";
						// 	}
						// 	std::cout << std::endl;
						// }
						std::cout << "----------" << std::endl;
						// if (treeRoot == NULL)
						// 	std::cout << "Passenger list is empty" << std::endl
						// 			  << "----------" << std::endl;
						// else
						// 	std::cout << "Passengerlist:" << std::endl
						// 			  << "----------" << std::endl;
						show_noti("print all data to terminal successfully!");
					}
					else if (state[SDL_SCANCODE_H] && (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]))
					{
						current_screen = MAIN_MENU;
					}
					else if (state[SDL_SCANCODE_A] && (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]))
					{
						current_screen = AIRCRAFT_MANAGEMENT;
					}
					else if (state[SDL_SCANCODE_F] && (state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL]))
					{
						current_screen = FLIGHT_MANAGEMENT;
					}
				}
				else if (ev.type == SDL_MOUSEBUTTONDOWN)
				{
					// int mouseX, mouseY;
					// SDL_GetMouseState(&mouseX, &mouseY);
					// std::cout << mouseX << " " << mouseY << std::endl;
				}
				else if (ev.type == SDL_MOUSEBUTTONUP)
				{
				}
				SDL_UpdateWindowSurface(gWindow);
			}

			switch (current_screen)
			{
			case NONE:
				break;
			case MAIN_MENU:
				draw_main_menu_screen();
				break;
			case AIRCRAFT_MANAGEMENT:
				draw_aircraft_management_screen(planeList, pFirstFlight, special_font);
				break;
			case FLIGHT_MANAGEMENT:
				draw_flight_management_screen(pFirstFlight, planeList, treeRoot, special_font);
				break;
				// case DEMO_WINDOW:
				// 	ImGui::ShowDemoWindow(&open_state[DEMO_WINDOW]);
			case PASSENGER_MANAGEMENT:
				draw_passenger_management_screen(treeRoot, special_font);
				break;
			}

			if (showNoti)
			{
				ImVec2 center = ImGui::GetMainViewport()->GetCenter();
				ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				ImGui::SetNextWindowFocus();
				ImGui::PushFont(noti_font);
				ImGui::Begin("Notification", &showNoti, notification);
				ImGui::Text(notiMessage.c_str());
				if (SDL_GetTicks() - notiStartTime > timeDisplayNoti)
				{
					showNoti = false;
				}
				ImGui::End();
				ImGui::PopFont();
			}

			if (showPopupNoti)
			{
				draw_popup_noti();
				if (SDL_GetTicks() - popupNotiStartTime > timeDisplayNoti)
				{
					showPopupNoti = false;
					ImGui::CloseCurrentPopup();
				}
			}

			// Rendering
			renderImGui(io);
		}
	}
	terminate(planeList, pFirstFlight, treeRoot);

	return 0;
}