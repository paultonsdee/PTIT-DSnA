#include "Manipulation/my_dHandling.h"
#include "Manipulation/my_gui.h"

int main(int, char**)
{
	// Our state
	bool show_another_window = false;
	// std::string filename = "aircraft.dat";
	PlaneList planeList;
	
	FlightNodePTR pFirstFlight = NULL;
	link_list_initialize(pFirstFlight);

	if (!init_SDL())
	{
		std::cout << "Failed to initialize!\n";
		display_message("e", "Error", "Failed to initialize!");
	}
	else
	{
		load_aircraft(planeList, aircraftFile);
		load_flight(pFirstFlight, flightFile);

		ImGuiIO& io = init_ImGui();

		io.Fonts->AddFontFromFileTTF("Media/Roboto-Medium.ttf", 16.0f);
		io.FontDefault = io.Fonts->Fonts.back();
		ImFont* special_font = io.Fonts->AddFontFromFileTTF("Media/PassionOne-Bold.ttf", 37.0f);

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
						save_aircraft(planeList, aircraftFile);
						save_flight(pFirstFlight, flightFile);
						break;
					case SDLK_SPACE:
						current_screen = MAIN_MENU;
						// SDL_Window* menuWindow = SDL_CreateWindow("Menu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
						break;
					case SDLK_LCTRL: //test case
						if (planeList.totalPlane == 0)
							std::cout << "Plane list is empty" << std::endl << "----------" << std::endl;
						else
							std::cout << "Planelist:" << std::endl << "----------" << std::endl;
						for (int i = 0; i < planeList.totalPlane; i++)
						{
							Plane* plane = planeList.nodes[i];
							std::cout << plane->planeID << " | ";
							std::cout << plane->planeType << " | ";
							std::cout << plane->seatNum << " | ";
							std::cout << plane->rowNum;
							std::cout << std::endl;
						}
						std::cout << "----------" << std::endl;
						if (count_flights(pFirstFlight) == 0)
							std::cout << "Flight list is empty" << std::endl << "----------" << std::endl;
						else
							std::cout << "Flightlist:" << std::endl << "----------" << std::endl;
						for (FlightNodePTR p = pFirstFlight; p != NULL; p = p->next)
						{
							std::cout << p->flight.flightNumber << " | ";
							std::cout << p->flight.desAirport << " | ";
							std::cout << p->flight.stt << " | ";
							std::cout << p->flight.planeID << " | ";
							std::cout << p->flight.totalTicket << " | ";
							std::cout << p->flight.maxTicket;
							std::cout << std::endl << "\t\t";
							std::cout << p->flight.departureTime.day << "/";
							std::cout << p->flight.departureTime.month << "/";
							std::cout << p->flight.departureTime.year << " ";
							std::cout << p->flight.departureTime.hour << ":";
							std::cout << p->flight.departureTime.minute;
							std::cout << std::endl;
						}
						std::cout << "----------" << std::endl;
						break;
					}
				}
				else if (ev.type == SDL_MOUSEBUTTONDOWN)
				{
					// int mouseX, mouseY;
					// SDL_GetMouseState(&mouseX, &mouseY);
					// std::cout << mouseX << " " << mouseY << std::endl;

					// if (count_flights(pFirstFlight) != 0)
					// {
					// 	std::cout << pFirstFlight->flight.flightNumber << std::endl;
					// 	std::cout << pFirstFlight->flight.departureTime.day << std::endl;
					// 	std::cout << pFirstFlight->flight.departureTime.month << std::endl;
					// 	std::cout << pFirstFlight->flight.departureTime.year << std::endl;
					// 	std::cout << pFirstFlight->flight.departureTime.hour << std::endl;
					// 	std::cout << pFirstFlight->flight.departureTime.minute << std::endl;
					// 	std::cout << pFirstFlight->flight.desAirport << std::endl;
					// 	std::cout << pFirstFlight->flight.stt << std::endl;
					// 	std::cout << pFirstFlight->flight.planeID << std::endl;
					// 	std::cout << pFirstFlight->flight.totalTicket << std::endl;
					// 	std::cout << pFirstFlight->flight.maxTicket << std::endl;
					// }
					// if (count_flights(pFirstFlight) > 1)
					// {
					// 	std::cout << pFirstFlight->next->flight.flightNumber << std::endl;
					// 	std::cout << pFirstFlight->next->flight.departureTime.day << std::endl;
					// 	std::cout << pFirstFlight->next->flight.departureTime.month << std::endl;
					// 	std::cout << pFirstFlight->next->flight.departureTime.year << std::endl;
					// 	std::cout << pFirstFlight->next->flight.departureTime.hour << std::endl;
					// 	std::cout << pFirstFlight->next->flight.departureTime.minute << std::endl;
					// 	std::cout << pFirstFlight->next->flight.desAirport << std::endl;
					// 	std::cout << pFirstFlight->next->flight.stt << std::endl;
					// 	std::cout << pFirstFlight->next->flight.planeID << std::endl;
					// 	std::cout << pFirstFlight->next->flight.totalTicket << std::endl;
					// 	std::cout << pFirstFlight->next->flight.maxTicket << std::endl;
					// }

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
				draw_aircraft_management_screen(planeList, special_font);
				break;
			case FLIGHT_MANAGEMENT:
				draw_flight_management_screen(pFirstFlight, planeList, special_font);
				break;
			case DEMO_WINDOW:
				ImGui::ShowDemoWindow(&open_state[DEMO_WINDOW]);
				break;
			}

				// Rendering
			renderImGui(io);
		}
	}
	terminate();

	return 0;
}