#include "Manipulation/my_dHandling.h"
#include "Manipulation/my_gui.h"

int main(int, char**)
{
	// Our state
	bool show_another_window = false;
	// std::string filename = "aircraft.dat";
	PlaneList planeList;
	
	FlightListPTR pFlightList = NULL;

	if (!init_SDL())
	{
		std::cout << "Failed to initialize!\n";
		display_message("e", "Error", "Failed to initialize!");
	}
	else
	{
		load_aircraft(planeList, aircraftFile);

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
						break;
					case SDLK_SPACE:
						current_screen = MAIN_MENU;
						// SDL_Window* menuWindow = SDL_CreateWindow("Menu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
						break;
					case SDLK_LCTRL: //test case
						if (planeList.totalPlane == 0)
							std::cout << "Plane list is empty";
						for (int i = 0; i < planeList.totalPlane; i++)
						{
							Plane* plane = planeList.nodes[i];
							std::cout << plane->planeID << " | ";
							std::cout << plane->planeType << " | ";
							std::cout << plane->seatNum << " | ";
							std::cout << plane->rowNum;
							std::cout << std::endl;
						}
						break;
					}
				}
				else if (ev.type == SDL_MOUSEBUTTONDOWN)
				{
					int mouseX, mouseY;
					SDL_GetMouseState(&mouseX, &mouseY);
					std::cout << mouseX << " " << mouseY << std::endl;
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
				draw_flight_management_screen(pFlightList, planeList, special_font);
				break;
			case DEMO_WINDOW:
				ImGui::ShowDemoWindow(&open_state[DEMO_WINDOW]);
				break;
			}

			// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
			/*if (show_demo_window)
				ImGui::ShowDemoWindow(&show_demo_window);*/

				// ImGui::Button("clickme"); // Create a button called "Hello, world!" and display it.

				//{
				//	static float f = 0.0f;
				//	static int counter = 0;

				//	//ImGui::Begin("Please input here");                          // Create a window called "Hello, world!" and append into it.

				//	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
				//	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
				//	ImGui::Checkbox("Another Window", &show_another_window);

				//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				//	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

				//	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				//		counter++;
				//	ImGui::SameLine();
				//	ImGui::Text("counter = %d", counter);

				//	//ImGui::End();
				//}

				// Rendering
			renderImGui(io);
		}
	}
	terminate();

	return 0;
}