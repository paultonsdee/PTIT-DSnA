#include "Declaration/my_gloVal.h"

SDL_Color BLACK = {0, 0, 0};

void display_message(const char *type, const char *label, const char *message)
{
	Uint32 flag;
	switch (type[0])
	{
	case 'n':
		flag = SDL_MESSAGEBOX_INFORMATION;
		break;
	case 'w':
		flag = SDL_MESSAGEBOX_WARNING;
		break;
	case 'e':
		flag = SDL_MESSAGEBOX_ERROR;
		break;
	}
	SDL_ShowSimpleMessageBox(flag, label, message, gWindow);
}

bool init_SDL()
{
	bool success = true;

	// Enable native IME on Windows
#ifdef SDL_HINT_IME_SHOW_UI
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0)
	{
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		display_message("e", "SDL Error", "SDL could not initialize!");
		success = false;
	}
	else
	{
		// Create window
		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
		gWindow = SDL_CreateWindow("Flight Management System", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, VIEWPORT_WIDTH, VIEWPORT_HEIGHT, window_flags);
		if (gWindow == NULL)
		{
			std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
			display_message("e", "SDL Error", "Window could not be created!");
			success = false;
		}
		else
		{
			// Get window surface
			gRenderTarget = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderTarget == NULL)
			{
				std::cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
				display_message("e", "SDL Error", "Renderer could not be created!");
				success = false;
			}
			else
			{
				gBackGround = LoadTexture("Media/bkgr.bmp");
				if (gBackGround == NULL)
				{
					std::cout << "Unable to load background image! SDL_Error: " << SDL_GetError << std::endl;
					display_message("e", "SDL Error", "Unable to load background image!");
					success = false;
				}
				else
				{
					SDL_RenderClear(gRenderTarget);
					SDL_RenderCopy(gRenderTarget, gBackGround, NULL, NULL);
					SDL_RenderPresent(gRenderTarget);
					SDL_Delay(257);
				}
			}
		}
	}

	return success;
}

ImGuiIO &init_ImGui()
{
	clear_color = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(gWindow, gRenderTarget);
	ImGui_ImplSDLRenderer2_Init(gRenderTarget);

	return io;
}

SDL_Texture *LoadTexture(const char *filePath)
{
	SDL_Texture *texture = nullptr;
	SDL_Surface *surface = SDL_LoadBMP(filePath);
	if (surface == NULL)
		std::cout << "Error loading BMP: " << SDL_GetError() << std::endl;
	else
	{
		texture = SDL_CreateTextureFromSurface(gRenderTarget, surface);
		if (texture == NULL)
			std::cout << "Error creating texture: " << SDL_GetError() << std::endl;
	}

	SDL_FreeSurface(surface);

	return texture;
}

void init_row_arr()
{
	for (int i = 0; i < 100; i++)
	{
		rows[i] = std::to_string(i + 1);
	}
}

void terminate()
{
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyWindow(gWindow);
	SDL_DestroyTexture(gBackGround);
	SDL_DestroyRenderer(gRenderTarget);
	gWindow = NULL;
	gBackGround = NULL;
	gRenderTarget = NULL;

	SDL_Quit(); // Quit SDL subsystems
}

void renderImGui(ImGuiIO &io)
{
	ImGui::Render();
	SDL_RenderSetScale(gRenderTarget, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
	SDL_SetRenderDrawColor(gRenderTarget, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
	SDL_RenderClear(gRenderTarget);
	SDL_RenderCopy(gRenderTarget, gBackGround, NULL, NULL);
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(gRenderTarget);
}

void draw_combo(const char *label, const char *choices[], const int &length, int &current_index, const char *&current)
{
	if (ImGui::BeginCombo(label, current))
	{
		for (int n = 0; n < length; n++)
		{
			const bool is_selected = (current_index == n);
			if (ImGui::Selectable(choices[n], is_selected))
			{
				current_index = n;
				current = choices[current_index];
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus(); // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
		}
		ImGui::EndCombo();
	}
}

void draw_combo(const char *label, PlaneList &planeList, int &current_index, const char *&current, int &airline)
{
	static int planeAirlines[MAXPLANE];
	for (int i = 0; i < MAXPLANE; i++)
	{
		planeAirlines[i] = -1;
	}

	for (int i = 0; i < planeList.totalPlane; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if (is_in_array(planeList.nodes[i]->planeType, aircraftTypes[j], j))
			{
				planeAirlines[i] = j;
				break;
			}
		}
	}

	static const char *tempTypesArray[MAXPLANE];
	for (int i = 0; i < planeList.totalPlane; i++)
	{
		tempTypesArray[i] = planeList.nodes[i]->planeID.c_str();
	}

	if (ImGui::BeginCombo(label, current))
	{
		for (int n = 0; n < planeList.totalPlane; n++)
		{
			if (planeAirlines[n] == airline)
			{
				const bool is_selected = (current_index == n);
				if (ImGui::Selectable(tempTypesArray[n], is_selected))
				{
					current_index = n;
					current = tempTypesArray[current_index];
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus(); // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
			}
		}
		ImGui::EndCombo();
	}
}

static void HelpMarker(const char *desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void show_noti(const std::string &message)
{
	showNoti = true;
	notiMessage = message;
	notiStartTime = SDL_GetTicks();
}

void show_popup_noti(const std::string &message)
{
	showPopupNoti = true;
	popupNotiMessage = message;
	popupNotiStartTime = SDL_GetTicks();
}

void draw_popup_noti()
{
	ImGui::OpenPopup("Popup Notification");
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::PushFont(noti_font);
	if (ImGui::BeginPopupModal("Popup Notification", NULL, popupModalFlags | ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::Text(popupNotiMessage.c_str());
		ImGui::EndPopup();
	}
	ImGui::PopFont();
}

bool is_in_array(std::string &planeType, const char *arr[], const int &aircraftTypesIndex)
{
	for (int i = 0; i < aircraftTypesLength[aircraftTypesIndex]; i++)
	{
		if (planeType == arr[i])
			return true;
	}
	return false;
}

void confirm_change_status(FlightNodePTR &pFirstFlight, int &newStatus, int &currentPage, int &row)
{
	FlightNodePTR p = pFirstFlight;

	for (int i = 0; i < 30 * currentPage + row; i++)
		p = p->next;
	if (newStatus != -1)
	{

		ImGui::OpenPopup("Confirm change flight status");

		// Always center this window when appearing
		ImVec2 center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

		if (ImGui::BeginPopupModal("Confirm change flight status", NULL, popupModalFlags))
		{
			ImGui::Separator();
			switch (newStatus)
			{
			case 0:
				ImGui::Text("Are you sure you want to cancel this flight?");
				break;
			case 1:
				ImGui::Text("Are you sure you want to make this flight available again?");
				break;
			case 3:
				ImGui::Text("Is this flight ready to start?");
				break;

			default:
				break;
			}
			ImGui::Separator();

			int windowWidth = ImGui::GetWindowWidth();
			int saveButtonX = (windowWidth - 2 * cmdButtonSize.x - ITEM_SPACING) / 2;
			ImGui::SetCursorPosX(saveButtonX);

			if (ImGui::Button("Yes", cmdButtonSize))
			{
				switch (newStatus)
				{
				case 0:
					p->flight.stt = newStatus;
					show_noti("This flight has been canceled!");
					break;
				case 1:
					p->flight.stt = newStatus;
					show_noti("This flight is now available!");
					if (p->flight.totalTicket == p->flight.maxTicket)
						p->flight.stt = 2;
					break;
				case 3:
					bool changeable = true;
					if (p->flight.maxTicket <= 10)
					{
						// do nothing
					}
					else if (p->flight.maxTicket <= 37)
					{
						if (p->flight.totalTicket < (0.7 * p->flight.maxTicket))
						{
							show_noti("The Passenger in this flight must be at least 70%% of the total seats to start the flight!");
							changeable = false;
						}
					}
					else if (p->flight.maxTicket <= 200)
					{
						if (p->flight.totalTicket < (0.8 * p->flight.maxTicket))
						{
							show_noti("The Passenger in this flight must be at least 80%% of the total seats to start the flight!");
							changeable = false;
						}
					}
					else if (p->flight.maxTicket > 200)
					{
						if (p->flight.totalTicket < (0.9 * p->flight.maxTicket))
						{
							show_noti("The Passenger in this flight must be at least 90%% of the total seats to start the flight!");
							changeable = false;
						}
					}
					if (changeable)
					{
						p->flight.stt = newStatus;
						show_noti("Fly successfully!");
						ImGui::CloseCurrentPopup();
					}
					break;
				}

				newStatus = -1;
				row = -1;

				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("No", cmdButtonSize))
			{
				newStatus = -1;
				row = -1;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}

void draw_main_menu_screen()
{
	open_state[MAIN_MENU] = true;
	ImVec2 viewportSize = ImGui::GetIO().DisplaySize;

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(viewportSize);

	ImGui::Begin("Main menu", &open_state[MAIN_MENU], fixed_full_screen);

	button AircraftManageButton("Aircraft Management", 200, 50);
	ImGui::SetCursorPos(ImVec2(AircraftManageButton.x_pos, AircraftManageButton.y_pos));
	if (ImGui::Button("Aircraft Management", ImVec2(AircraftManageButton.width, AircraftManageButton.height)))
	{
		current_screen = AIRCRAFT_MANAGEMENT;
		open_state[MAIN_MENU] = false;
	}
	button FlightManagementButton("Flight Management", AircraftManageButton, 25);
	ImGui::SetCursorPos(ImVec2(FlightManagementButton.x_pos, FlightManagementButton.y_pos));
	if (ImGui::Button("Flight Management", ImVec2(FlightManagementButton.width, FlightManagementButton.height)))
	{
		current_screen = FLIGHT_MANAGEMENT;
		open_state[MAIN_MENU] = false;
	}

	button PassengerManagementButton("Passenger Management", FlightManagementButton, 25);
	ImGui::SetCursorPos(ImVec2(PassengerManagementButton.x_pos, PassengerManagementButton.y_pos));
	if (ImGui::Button(PassengerManagementButton.name, ImVec2(PassengerManagementButton.width, PassengerManagementButton.height)))
	{
		current_screen = PASSENGER_MANAGEMENT;
		open_state[MAIN_MENU] = false;
	}

	button CloseWindowButton("Close Window", PassengerManagementButton, 25);
	ImGui::SetCursorPos(ImVec2(CloseWindowButton.x_pos, CloseWindowButton.y_pos));
	if (ImGui::Button("Close Window", ImVec2(CloseWindowButton.width, CloseWindowButton.height)))
	{
		current_screen = NONE;
		open_state[MAIN_MENU] = false;
	}

	ImGui::End();
}

void show_AM_action_buttons(PlaneList &planeList, FlightNodePTR &pFirstFlight, int &selected, int &currentPage, bool &isInTable, ImVec2 &tableCursorPos)
{
	ImVec2 viewportSize = ImGui::GetIO().DisplaySize;

	ImVec2 actionButtonsCursorPos = ImGui::GetCursorPos();
	button editButton("EDIT", actionButtonSize, actionButtonsCursorPos, 4);

	int widthSpacing = (viewportSize.x - 4 * actionButtonSize.x - 2 * editButton.x_pos) / 3;

	button addButton("ADD", editButton, widthSpacing, 4);
	ImGui::SetCursorPos(ImVec2(addButton.x_pos, addButton.y_pos));
	static bool show_add_plane_popup = false;
	if (ImGui::Button(addButton.name, actionButtonSize))
		show_add_plane_popup = true;
	if (show_add_plane_popup)
		add_plane_popup(planeList, show_add_plane_popup, selected, currentPage);

	button saveAndExitButton("SAVE & EXIT", addButton, widthSpacing, 4);
	ImGui::SetCursorPos(ImVec2(saveAndExitButton.x_pos, saveAndExitButton.y_pos));
	if (ImGui::Button(saveAndExitButton.name, actionButtonSize))
	{
		save_aircraft(planeList, aircraftFile);
		save_flight(pFirstFlight, flightFile);
		current_screen = MAIN_MENU;
		open_state[AIRCRAFT_MANAGEMENT] = false;
	}

	button deleteButton("DELETE", saveAndExitButton, widthSpacing, 4);

	ImVec2 controlButtonSize = ImGui::CalcTextSize("RANKING TABLE");
	controlButtonSize.x += 2 * ITEM_SPACING;
	controlButtonSize.y += 2 * ITEM_SPACING;
	button passengerAndTicketButton("RANKING TABLE", controlButtonSize.x, controlButtonSize.y, viewportSize.x - controlButtonSize.x - BASE_WIDTH, tableCursorPos.y - controlButtonSize.y - HEIGHT_SPACING * 2);
	ImGui::SetCursorPos(ImVec2(passengerAndTicketButton.x_pos, passengerAndTicketButton.y_pos));
	static bool show_ranking_table_popup = false;
	if (ImGui::Button(passengerAndTicketButton.name, controlButtonSize))
		show_ranking_table_popup = true;
	if (show_ranking_table_popup)
		ranking_table_popup(planeList, pFirstFlight, show_ranking_table_popup);

	static bool isInEditButton = false;
	if (ImGui::IsMouseHoveringRect(ImVec2(editButton.x_pos, editButton.y_pos), ImVec2(editButton.x_pos + editButton.width, editButton.y_pos + editButton.height)))
		isInEditButton = true;
	else
		isInEditButton = false;

	static bool isInDeleteButton = false;
	if (ImGui::IsMouseHoveringRect(ImVec2(deleteButton.x_pos, deleteButton.y_pos), ImVec2(deleteButton.x_pos + deleteButton.width, deleteButton.y_pos + deleteButton.height)))
		isInDeleteButton = true;
	else
		isInDeleteButton = false;

	if (!(isInTable) && !(isInEditButton) && !(isInDeleteButton))
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			selected = -1;

	if (selected != -1)
	{
		ImGui::SetCursorPos(ImVec2(editButton.x_pos, editButton.y_pos));
		static bool show_edit_plane_popup = false;
		if (ImGui::Button(editButton.name, actionButtonSize))
			show_edit_plane_popup = true;
		if (show_edit_plane_popup)
			edit_plane_popup(planeList, pFirstFlight, show_edit_plane_popup, selected);

		ImGui::SetCursorPos(ImVec2(deleteButton.x_pos, deleteButton.y_pos));
		static bool show_delete_plane_popup = false;
		if (ImGui::Button(deleteButton.name, actionButtonSize))
			show_delete_plane_popup = true;
		if (show_delete_plane_popup)
			delete_plane_popup(planeList, selected, currentPage, show_delete_plane_popup);
	}
}

void draw_aircraft_management_screen(PlaneList &planeList, FlightNodePTR &pFirstFlight, ImFont *&headerFont)
{
	open_state[AIRCRAFT_MANAGEMENT] = true;
	ImVec2 viewportSize = ImGui::GetIO().DisplaySize;

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(viewportSize);
	ImGui::Begin("Aircraft Management", &open_state[AIRCRAFT_MANAGEMENT], fixed_full_screen);

	ImGui::PushFont(headerFont);
	const char *header = "AIRCRAFT MANAGEMENT";
	ImVec2 headerSize = ImGui::CalcTextSize(header);
	ImGui::SetCursorPosX((viewportSize.x - headerSize.x) / 2);
	ImGui::Text(header);
	ImGui::PopFont();

	ImGui::Text("");
	ImGui::Text("");
	// ImGui::Text("From here you can control all the aircrafts!");
	// HelpMarker(
	// 	"Using TableSetupColumn() to alter resizing policy on a per-column basis.\n\n"
	// 	"When combining Fixed and Stretch columns, generally you only want one, maybe two trailing columns to use _WidthStretch.");
	// static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

	static ImGuiTableFlags flags = ImGuiTableFlags_RowBg |
								   ImGuiTableFlags_BordersOuter;
	//    ImGuiTableFlags_SizingStretchSame|
	//    ImGuiTableFlags_ScrollX
	//    ImGuiTableFlags_SizingFixedFit;

	// ImGuiTableFlags_NoBordersInBody, HighlightHoveredColumn, ImGuiTableFlags_ScrollY

	ImVec2 tableCursorPos = ImGui::GetCursorPos();

	static int selected_row = -1;
	static int currentPage = 0;
	int totalPages = planeList.totalPlane / 30;
	if (planeList.totalPlane % 30)
		totalPages++;

	if (ImGui::BeginTable("Planes", 3, flags))
	{
		ImGui::TableSetupColumn("Aircraft Registration");
		ImGui::TableSetupColumn("Aircraft Type");
		ImGui::TableSetupColumn("Total Seats");
		ImGui::TableHeadersRow();

		if (!is_planeList_empty(planeList))
		{
			ImGui::TableNextRow();
			// static bool* selected = new bool[planeList.totalPlane] {false};
			int countRows = 0;
			for (; countRows < 30 && ((currentPage * 30 + countRows) < planeList.totalPlane); countRows++)
			{
				int currentPlaneIndex = currentPage * 30 + countRows;
				ImGui::TableNextRow();

				for (int column = 0; column < 3; column++)
				{
					ImGui::TableSetColumnIndex(column);
					Plane *plane = planeList.nodes[currentPlaneIndex];
					switch (column)
					{
					case 0:
						if (ImGui::Selectable((plane->planeID).c_str(), selected_row == currentPlaneIndex, ImGuiSelectableFlags_SpanAllColumns))
							selected_row = currentPlaneIndex;
						break;
					case 1:
						ImGui::Text((plane->planeType).c_str());
						break;
					case 2:
						ImGui::Text("%d", (plane->rowNum * plane->seatNum));
						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::Text("%d x %d", plane->seatNum, plane->rowNum);
							ImGui::EndTooltip();
						}

						break;
					}
				}
			}
			if (countRows < 30)
			{
				for (; countRows < 30; countRows++)
				{
					ImGui::TableNextRow();
					for (int column = 0; column < 3; column++)
					{
						ImGui::TableSetColumnIndex(column);
						ImGui::Text("");
					}
				}
			}
		}

		ImGui::EndTable();
	}
	ImVec2 tableSize = ImGui::GetItemRectSize();

	ImVec2 currentMousePos = ImGui::GetMousePos();
	static bool isInTable = false;
	if (ImGui::IsMouseHoveringRect(tableCursorPos, ImVec2(tableCursorPos.x + tableSize.x, tableCursorPos.y + tableSize.y)))
	{
		isInTable = true;
	}
	else
	{
		isInTable = false;
	}

	ImVec2 pageNavButton = ImVec2(30, 30);

	std::string pageInfo = std::to_string(currentPage + 1) + " / " + std::to_string(totalPages);
	int cursorX = viewportSize.x / 2 - pageNavButton.x - ITEM_SPACING - ImGui::CalcTextSize(pageInfo.c_str()).x / 2;
	ImGui::SetCursorPosX(cursorX);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 17);

	if (ImGui::Button("<|", pageNavButton))
	{
		if (currentPage < totalPages)
			currentPage--;
		if (currentPage < 0)
			currentPage = 0;
	}
	ImGui::SameLine();
	ImGui::Text(pageInfo.c_str());
	ImGui::SameLine();
	if (ImGui::Button("|>", pageNavButton))
	{
		if (currentPage >= 0)
			currentPage++;
		if (currentPage >= totalPages)
			currentPage = totalPages - 1;
	}

	show_AM_action_buttons(planeList, pFirstFlight, selected_row, currentPage, isInTable, tableCursorPos);

	// ImGui::SetCursorPosY(viewportSize.y - ImGui::CalcTextSize("Home").y - HEIGHT_SPACING*2 - 10);
	// ImGui::Button("Home");

	ImGui::End();
}

void add_plane_popup(PlaneList &planeList, bool &show_add_plane_popup, int &currentAircraft, int &currentPage)
{
	ImGui::OpenPopup("Want to add a plane?");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Want to add a plane?", NULL, popupModalFlags))
	{

		// ImGui::Text("Dont forget to save data!\nThis operation cannot be undone!");
		ImGui::Separator();

		ImGui::Text("Airlines");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_AM);
		ImGui::Text(":  ");
		ImGui::SameLine();

		static int current_airline_index = 0;
		static const char *current_airline = airlines[current_airline_index];
		const int airline_length = combo_length(airlines);
		ImGui::PushItemWidth(BOX_WIDTH_AM);

		draw_combo("##aircraft_registration", airlines, airline_length, current_airline_index, current_airline);
		ImGui::PopItemWidth();

		ImGui::Spacing();

		struct TextFilters
		{
			// Return true to discard a character.
			static int emirates_filter(ImGuiInputTextCallbackData *data)
			{
				if (data->EventChar >= 'a' && data->EventChar <= 'z')
				{
					data->EventChar += 'A' - 'a';
					return false;
				}
				else if (data->EventChar >= 'A' && data->EventChar <= 'Z')
				{
					return false;
				}

				return true;
			}
		};

		static char aircraft_registration_buf[6] = "";

		ImGui::Text("Aircraft Registration");
		ImGui::SameLine();
		switch (current_airline_index)
		{
		case 0:
		case 1:
		case 2:
			HelpMarker("0000 -> 9999");
			ImGui::SameLine();
			ImGui::SetCursorPosX(COLON_POSX_AM);
			ImGui::Text(":  ");
			ImGui::SameLine();
			ImGui::Text("VN-");
			ImGui::SameLine();
			ImGui::PushItemWidth(BOX_WIDTH_AM - (BASE_WIDTH + DIGIT_WIDTH * 3));
			ImGui::InputText("##XXXX", aircraft_registration_buf, 5, ImGuiInputTextFlags_CharsDecimal);
			ImGui::PopItemWidth();
			break;
		case 3:
			HelpMarker("AAA -> ZZZ");
			ImGui::SameLine();
			ImGui::SetCursorPosX(COLON_POSX_AM);
			ImGui::Text(":  ");
			ImGui::SameLine();
			ImGui::Text("A6-");
			ImGui::SameLine();
			ImGui::PushItemWidth(BOX_WIDTH_AM - (BASE_WIDTH + DIGIT_WIDTH * 3));
			ImGui::InputText("##YYY", aircraft_registration_buf, 4, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::emirates_filter);
			ImGui::PopItemWidth();
			break;
		case 4:
			HelpMarker("00000 -> 99999");
			ImGui::SameLine();
			ImGui::SetCursorPosX(COLON_POSX_AM);
			ImGui::Text(":  ");
			ImGui::SameLine();
			ImGui::Text("B-");
			ImGui::SameLine();
			ImGui::PushItemWidth(BOX_WIDTH_AM - (BASE_WIDTH + DIGIT_WIDTH * 2));
			ImGui::InputText("##XXXXX", aircraft_registration_buf, 6, ImGuiInputTextFlags_CharsDecimal);
			ImGui::PopItemWidth();
			break;
		default:
			break;
		}

		ImGui::Spacing();

		ImGui::Text("Aircraft type");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_AM);
		ImGui::Text(":  ");
		ImGui::SameLine();

		static int current_aircraftType_index[5] = {0};
		static const char *current_aircraftType[5];
		for (int i = 0; i < 5; i++)
		{
			current_aircraftType[i] = aircraftTypes[i][current_aircraftType_index[i]];
		}

		ImGui::PushItemWidth(BOX_WIDTH_AM);
		draw_combo("##aircraft_type", aircraftTypes[current_airline_index], aircraftTypesLength[current_airline_index], current_aircraftType_index[current_airline_index], current_aircraftType[current_airline_index]);

		ImGui::PopItemWidth();

		ImGui::Spacing();

		ImGui::Text("Total seats");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_AM);
		ImGui::Text(":  ");
		ImGui::SameLine();
		static int total_seats = 2;
		ImGui::PushItemWidth(BOX_WIDTH_AM - (BASE_WIDTH + DIGIT_WIDTH * 2) - ITEM_SPACING);
		ImGui::SliderInt("##slider_seats", &total_seats, 2, 11);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::PushItemWidth(BASE_WIDTH + DIGIT_WIDTH * 2);
		if (ImGui::InputInt("##input_seats", &total_seats, 0, 0, ImGuiInputTextFlags_CharsDecimal))
		{
			if (total_seats < 2)
				total_seats = 2;
			else if (total_seats > 11)
				total_seats = 11;
		}
		ImGui::PopItemWidth();

		ImGui::Spacing();

		ImGui::Text("Total rows");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_AM);
		ImGui::Text(":  ");
		ImGui::SameLine();
		static int total_rows = 1;
		ImGui::PushItemWidth(BOX_WIDTH_AM - (BASE_WIDTH + DIGIT_WIDTH * 3) - ITEM_SPACING);
		ImGui::SliderInt("##slider_rows", &total_rows, 1, 100);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::PushItemWidth(BASE_WIDTH + DIGIT_WIDTH * 3);
		if (ImGui::InputInt("##input_rows", &total_rows, 0, 0, ImGuiInputTextFlags_CharsDecimal))
		{
			if (total_rows < 1)
				total_rows = 1;
			else if (total_rows > 100)
				total_rows = 100;
		}
		ImGui::PopItemWidth();

		ImGui::Separator();

		int windowWidth = ImGui::GetWindowWidth();
		int saveButtonX = (windowWidth - 2 * cmdButtonSize.x - ITEM_SPACING) / 2;
		ImGui::SetCursorPosX(saveButtonX);

		if (ImGui::Button("Save", cmdButtonSize))
		{
			bool rightFormat = true;
			if (current_airline_index == 0 || current_airline_index == 1 || current_airline_index == 2)
			{
				for (int i = 0; i < 4; i++)
					if (!is_decimal(aircraft_registration_buf[i]))
					{
						show_noti("Please enter in the right format!");
						rightFormat = false;
					}
			}
			else if (current_airline_index == 3)
			{
				for (int i = 0; i < 3; i++)
					if (!is_alpha(aircraft_registration_buf[i]))
					{
						show_noti("Please enter in the right format!");
						rightFormat = false;
					}
			}
			else if (current_airline_index == 4)
			{
				for (int i = 0; i < 5; i++)
					if (!is_decimal(aircraft_registration_buf[i]))
					{
						show_noti("Please enter in the right format!");
						rightFormat = false;
					}
			}

			if (rightFormat)
			{
				if (insert_plane(planeList, current_airline_index, aircraft_registration_buf, current_aircraftType[current_airline_index], total_seats, total_rows))
				{
					for (int i = 0; i < 6; i++)
					{
						aircraft_registration_buf[i] = '\0';
					}
					currentAircraft = planeList.totalPlane - 1;
					currentPage = (planeList.totalPlane - 1) / 30;

					show_add_plane_popup = false;
				}
			}

			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", cmdButtonSize))
		{
			for (int i = 0; i < 6; i++)
			{
				aircraft_registration_buf[i] = '\0';
			}
			show_add_plane_popup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void edit_plane_popup(PlaneList &planeList, FlightNodePTR &pFirstFlight, bool &show_add_plane_popup, int &selectedPlane)
{
	ImGui::OpenPopup("Want to edit a plane?");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Want to edit a plane?", NULL, popupModalFlags))
	{
		// ImGui::Text("Dont forget to save data!\nThis operation cannot be undone!");
		ImGui::Separator();

		ImGui::Text("Airlines");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_AM);
		ImGui::Text(":  ");
		ImGui::SameLine();
		static bool isFirstTime = true;
		static int current_airline_index = 0;
		static int total_seats;
		static int total_rows;
		if (isFirstTime)
		{
			switch (planeList.nodes[selectedPlane]->planeID[0])
			{
			case 'A':
				current_airline_index = 3;
				break;
			case 'B':
				current_airline_index = 4;
				break;
			default:
				if (is_in_array(planeList.nodes[selectedPlane]->planeType, aircraftTypes[0], 0))
					current_airline_index = 0;
				else if (is_in_array(planeList.nodes[selectedPlane]->planeType, aircraftTypes[1], 1))
					current_airline_index = 1;
				else
					current_airline_index = 2;
				break;
			}
			total_seats = planeList.nodes[selectedPlane]->seatNum;
			total_rows = planeList.nodes[selectedPlane]->rowNum;
			isFirstTime = false;
		}

		static const char *current_airline = airlines[current_airline_index];
		const int airline_length = combo_length(airlines);
		ImGui::PushItemWidth(BOX_WIDTH_AM);
		draw_combo("##aircraft_registration", airlines, airline_length, current_airline_index, current_airline);
		ImGui::PopItemWidth();

		ImGui::Spacing();

		struct TextFilters
		{
			// Return true to discard a character.
			static int emirates_filter(ImGuiInputTextCallbackData *data)
			{
				if (data->EventChar >= 'a' && data->EventChar <= 'z')
				{
					data->EventChar += 'A' - 'a';
					return false;
				}
				else if (data->EventChar >= 'A' && data->EventChar <= 'Z')
				{
					return false;
				}

				return true;
			}
		};

		static char aircraft_registration_buf[6] = "";
		static int temp_i = 0;
		static bool startFlag = false;
		static int startIndex = 0;
		while (planeList.nodes[selectedPlane]->planeID[temp_i] != '\0')
		{
			if (planeList.nodes[selectedPlane]->planeID[temp_i] == '-')
			{
				startFlag = true;
				temp_i++;
				continue;
			}
			if (startFlag)
			{
				aircraft_registration_buf[startIndex] = planeList.nodes[selectedPlane]->planeID[temp_i];
				startIndex++;
			}
			temp_i++;
		}

		ImGui::Text("Aircraft Registration");
		ImGui::SameLine();
		switch (current_airline_index)
		{
		case 0:
		case 1:
		case 2:
			HelpMarker("0000 -> 9999");
			ImGui::SameLine();
			ImGui::SetCursorPosX(COLON_POSX_AM);
			ImGui::Text(":  ");
			ImGui::SameLine();
			ImGui::Text("VN-");
			ImGui::SameLine();
			ImGui::PushItemWidth(BOX_WIDTH_AM - (BASE_WIDTH + DIGIT_WIDTH * 3));
			ImGui::InputText("##XXXX", aircraft_registration_buf, 5, ImGuiInputTextFlags_CharsDecimal);
			ImGui::PopItemWidth();
			break;
		case 3:
			HelpMarker("AAA -> ZZZ");
			ImGui::SameLine();
			ImGui::SetCursorPosX(COLON_POSX_AM);
			ImGui::Text(":  ");
			ImGui::SameLine();
			ImGui::Text("A6-");
			ImGui::SameLine();
			ImGui::PushItemWidth(BOX_WIDTH_AM - (BASE_WIDTH + DIGIT_WIDTH * 3));
			ImGui::InputText("##YYY", aircraft_registration_buf, 4, ImGuiInputTextFlags_CallbackCharFilter, TextFilters::emirates_filter);
			ImGui::PopItemWidth();
			break;
		case 4:
			HelpMarker("00000 -> 99999");
			ImGui::SameLine();
			ImGui::SetCursorPosX(COLON_POSX_AM);
			ImGui::Text(":  ");
			ImGui::SameLine();
			ImGui::Text("B-");
			ImGui::SameLine();
			ImGui::PushItemWidth(BOX_WIDTH_AM - (BASE_WIDTH + DIGIT_WIDTH * 2));
			ImGui::InputText("##XXXXX", aircraft_registration_buf, 6, ImGuiInputTextFlags_CharsDecimal);
			ImGui::PopItemWidth();
			break;
		default:
			break;
		}

		ImGui::Spacing();

		ImGui::Text("Aircraft type");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_AM);
		ImGui::Text(":  ");
		ImGui::SameLine();

		static int current_aircraftType_index[5] = {0};
		static const char *current_aircraftType[5];
		for (int i = 0; i < 5; i++)
		{
			current_aircraftType[i] = aircraftTypes[i][current_aircraftType_index[i]];
		}
		static bool successLoadAircraftType = false;
		if (!successLoadAircraftType)
		{
			for (int i = 0; i < aircraftTypesLength[current_airline_index]; i++)
			{
				if (planeList.nodes[selectedPlane]->planeType == aircraftTypes[current_airline_index][i])
				{
					current_aircraftType_index[current_airline_index] = i;
					break;
				}
			}
			current_aircraftType[current_airline_index] = aircraftTypes[current_airline_index][current_aircraftType_index[current_airline_index]];
			successLoadAircraftType = true;
		}

		ImGui::PushItemWidth(BOX_WIDTH_AM);
		draw_combo("##aircraft_type", aircraftTypes[current_airline_index], aircraftTypesLength[current_airline_index], current_aircraftType_index[current_airline_index], current_aircraftType[current_airline_index]);

		ImGui::PopItemWidth();

		ImGui::Spacing();

		ImGui::Text("Total seats");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_AM);
		ImGui::Text(":  ");
		ImGui::SameLine();

		ImGui::PushItemWidth(BOX_WIDTH_AM - (BASE_WIDTH + DIGIT_WIDTH * 2) - ITEM_SPACING);
		ImGui::SliderInt("##slider_seats", &total_seats, 2, 11);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::PushItemWidth(BASE_WIDTH + DIGIT_WIDTH * 2);
		if (ImGui::InputInt("##input_seats", &total_seats, 0, 0, ImGuiInputTextFlags_CharsDecimal))
		{
			if (total_seats < 2)
				total_seats = 2;
			else if (total_seats > 11)
				total_seats = 11;
		}
		ImGui::PopItemWidth();

		ImGui::Spacing();

		ImGui::Text("Total rows");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_AM);
		ImGui::Text(":  ");
		ImGui::SameLine();

		ImGui::PushItemWidth(BOX_WIDTH_AM - (BASE_WIDTH + DIGIT_WIDTH * 3) - ITEM_SPACING);
		ImGui::SliderInt("##slider_rows", &total_rows, 1, 100);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::PushItemWidth(BASE_WIDTH + DIGIT_WIDTH * 3);
		if (ImGui::InputInt("##input_rows", &total_rows, 0, 0, ImGuiInputTextFlags_CharsDecimal))
		{
			if (total_rows < 1)
				total_rows = 1;
			else if (total_rows > 100)
				total_rows = 100;
		}
		ImGui::PopItemWidth();

		ImGui::Separator();

		int windowWidth = ImGui::GetWindowWidth();
		int saveButtonX = (windowWidth - 2 * cmdButtonSize.x - ITEM_SPACING) / 2;
		ImGui::SetCursorPosX(saveButtonX);

		if (ImGui::Button("Save", cmdButtonSize))
		{
			bool rightFormat = true;
			if (current_airline_index == 0 || current_airline_index == 1 || current_airline_index == 2)
			{
				for (int i = 0; i < 4; i++)
					if (!is_decimal(aircraft_registration_buf[i]))
					{
						show_noti("Please enter in the right format!");
						rightFormat = false;
					}
			}
			else if (current_airline_index == 3)
			{
				for (int i = 0; i < 3; i++)
					if (!is_alpha(aircraft_registration_buf[i]))
					{
						show_noti("Please enter in the right format!");
						rightFormat = false;
					}
			}
			else if (current_airline_index == 4)
			{
				for (int i = 0; i < 5; i++)
					if (!is_decimal(aircraft_registration_buf[i]))
					{
						show_noti("Please enter in the right format!");
						rightFormat = false;
					}
			}

			if (rightFormat)
				if (edit_plane(planeList, pFirstFlight, current_airline_index, aircraft_registration_buf, current_aircraftType[current_airline_index], total_seats, total_rows, selectedPlane))
				{
					for (int i = 0; i < 6; i++)
					{
						aircraft_registration_buf[i] = '\0';
					}
					temp_i = 0;
					startFlag = false;
					startIndex = 0;
					successLoadAircraftType = false;
					isFirstTime = true;
					show_add_plane_popup = false;
				}

			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", cmdButtonSize))
		{
			// reset all static variables
			for (int i = 0; i < 6; i++)
			{
				aircraft_registration_buf[i] = '\0';
			}
			temp_i = 0;
			startFlag = false;
			startIndex = 0;
			successLoadAircraftType = false;
			isFirstTime = true;
			show_add_plane_popup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void delete_plane_popup(PlaneList &planeList, int &selectedPlane, int &currentPage, bool &show_delete_plane_popup)
{
	ImGui::OpenPopup("Want to delete a plane?");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Want to delete a plane?", NULL, popupModalFlags))
	{
		ImGui::Separator();

		ImGui::Text("Are you sure you want to delete this plane?");

		ImGui::Separator();

		int windowWidth = ImGui::GetWindowWidth();
		int saveButtonX = (windowWidth - 2 * cmdButtonSize.x - ITEM_SPACING) / 2;
		ImGui::SetCursorPosX(saveButtonX);

		if (ImGui::Button("Yes", cmdButtonSize))
		{
			delete_plane(planeList, selectedPlane);
			selectedPlane = -1;
			if (currentPage > 0 && planeList.totalPlane % 30 == 0)
				currentPage--;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("No", cmdButtonSize))
		{
			selectedPlane = -1;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void draw_flight_management_screen(FlightNodePTR &pFirstFlight, PlaneList &planeList, PassengerNodesPTR &root, ImFont *&headerFont)
{
	open_state[FLIGHT_MANAGEMENT] = true;
	ImVec2 viewportSize = ImGui::GetIO().DisplaySize;

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(viewportSize);
	ImGui::Begin("Flight Management", &open_state[FLIGHT_MANAGEMENT], fixed_full_screen);

	ImGui::PushFont(headerFont);
	const char *header = "FLIGHT MANAGEMENT";
	ImVec2 headerSize = ImGui::CalcTextSize(header);
	ImGui::SetCursorPosX((viewportSize.x - headerSize.x) / 2);
	ImGui::Text(header);
	ImGui::PopFont();

	ImGui::Text("");
	// ImGui::Text("From here you can control all the flights!");
	// HelpMarker(
	// 	"Using TableSetupColumn() to alter resizing policy on a per-column basis.\n\n"
	// 	"When combining Fixed and Stretch columns, generally you only want one, maybe two trailing columns to use _WidthStretch.");
	// static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;
	ImGui::Text("Filters:");

	static bool showDayFilter = false;
	static bool showMonthFilter = false;
	static bool showYearFilter = false;
	static bool showDestinationAirportFilter = false;
	static bool showAvailableTicketsFilter = false;

	ImGui::SameLine();
	ImGui::Text("Day:");
	ImGui::SameLine();
	ImGui::Checkbox("##Day", &showDayFilter);
	ImGui::SameLine();
	ImGui::Text("Month:");
	ImGui::SameLine();
	ImGui::Checkbox("##Month", &showMonthFilter);
	ImGui::SameLine();
	ImGui::Text("Year:");
	ImGui::SameLine();
	ImGui::Checkbox("##Year", &showYearFilter);
	ImGui::SameLine();
	ImGui::Text("Destination Airport:");
	ImGui::SameLine();
	ImGui::Checkbox("##Destination Airport", &showDestinationAirportFilter);
	ImGui::SameLine();
	ImGui::Text("Only book available flights:");
	ImGui::SameLine();
	ImGui::Checkbox("##Available Tickets", &showAvailableTicketsFilter);

	if (showAvailableTicketsFilter && !(showDayFilter || showMonthFilter || showYearFilter || showDestinationAirportFilter))
	{
		ImGui::SameLine();
		HelpMarker("You can only filter showing bookable flights when combining with at least one other filter!");
	}

	ImGui::Spacing();

	ImGui::Text("Departure Date: ");
	ImGui::SameLine();
	int box2digit = BASE_WIDTH + DIGIT_WIDTH * 2;
	int box4digit = BASE_WIDTH + DIGIT_WIDTH * 4;
	ImGui::PushItemWidth(box2digit);
	static int dayFilter = 1;
	static int monthFilter = 1;
	static int yearFilter = 2024;
	int maxDayInMonth[12];
	const int minDay = 1;
	const int minMonth = 1;
	const int maxMonth = 12;
	const int minYear = 2024;
	const int maxYear = 2099;
	find_max_day_in_month(maxDayInMonth, monthFilter, yearFilter);

	if (ImGui::InputInt("##day", &dayFilter, 0, 0, ImGuiInputTextFlags_CharsDecimal))
	{

		if (dayFilter < minDay)
			dayFilter = minDay;
		else if (dayFilter > maxDayInMonth[monthFilter - 1])
			dayFilter = maxDayInMonth[monthFilter - 1];
	}
	ImGui::SameLine();
	ImGui::Text("/");
	ImGui::SameLine();
	if (ImGui::InputInt("##month", &monthFilter, 0, 0, ImGuiInputTextFlags_CharsDecimal))
	{
		if (monthFilter < minMonth)
			monthFilter = minMonth;
		else if (monthFilter > maxMonth)
			monthFilter = maxMonth;
	}

	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::Text("/");
	ImGui::SameLine();
	ImGui::PushItemWidth(box4digit);
	if (ImGui::InputInt("##year", &yearFilter, 0, 0, ImGuiInputTextFlags_CharsDecimal))
	{
		if (yearFilter < minYear)
			yearFilter = minYear;
		else if (yearFilter > maxYear)
			yearFilter = maxYear;
	}
	ImGui::PopItemWidth();

	ImGui::SameLine();

	ImGui::Text("Departure Time: ");
	ImGui::SameLine();
	static int current_destinationAirport_index = 0;
	static const char *current_destinationAirport = airports[current_destinationAirport_index];
	const int Airports_length = combo_length(airports);
	ImGui::PushItemWidth(BOX_WIDTH_FM);
	draw_combo("##destination_airport", airports, Airports_length, current_destinationAirport_index, current_destinationAirport);
	ImGui::PopItemWidth();

	static ImGuiTableFlags flags = ImGuiTableFlags_RowBg |
								   ImGuiTableFlags_BordersOuter;
	//    ImGuiTableFlags_SizingStretchSame|
	//    ImGuiTableFlags_ScrollX
	//    ImGuiTableFlags_SizingFixedFit;

	// Always center this window when appearing
	ImVec2 tableCursorPos = ImGui::GetCursorPos();

	static int selected_row = -1;
	static int currentPage = 0;
	static int newStatus = -1;

	int totalPages = count_flights(pFirstFlight) / 30;
	if (count_flights(pFirstFlight) % 30)
		totalPages += 1;

	if (ImGui::BeginTable("Flights", 7, flags))
	{
		ImGui::TableSetupColumn("Flight Number");
		ImGui::TableSetupColumn("Departure Date");
		ImGui::TableSetupColumn("Departure Time");
		ImGui::TableSetupColumn("Destination Airport");
		ImGui::TableSetupColumn("Flights Status");
		ImGui::TableSetupColumn("Aircraft Used");
		ImGui::TableSetupColumn("Seats Available");
		ImGui::TableHeadersRow();

		FlightNodePTR p = pFirstFlight;
		ImGui::TableNextRow();

		if (!(showDayFilter || showMonthFilter || showYearFilter || showDestinationAirportFilter))
			for (int i = 0; i < 30 * currentPage; i++)
				p = p->next;
		int countRows = 0;
		while (countRows < 30 && p != NULL)
		{
			if (showDayFilter && p->flight.departureTime.day != dayFilter)
			{
				p = p->next;
				continue;
			}
			if (showMonthFilter && p->flight.departureTime.month != monthFilter)
			{
				p = p->next;
				continue;
			}
			if (showYearFilter && p->flight.departureTime.year != yearFilter)
			{
				p = p->next;
				continue;
			}
			if (showDestinationAirportFilter && p->flight.desAirport != current_destinationAirport)
			{
				p = p->next;
				continue;
			}
			if (showAvailableTicketsFilter && (showDayFilter || showMonthFilter || showYearFilter || showDestinationAirportFilter))
				if (p->flight.stt != 1)
				{
					p = p->next;
					continue;
				}

			ImGui::TableNextRow();
			for (int column = 0; column < 7; column++)
			{
				ImGui::TableSetColumnIndex(column);

				std::string id = "##" + std::to_string(FLIGHT_ID * countRows + column);

				switch (column)
				{
				case 0:
					if (ImGui::Selectable((p->flight.flightNumber).c_str(), selected_row == countRows, ImGuiSelectableFlags_SpanAllColumns))
						selected_row = countRows;
					break;
				case 1:
					if (p->flight.departureTime.day < 10)
						ImGui::Text("0%d", p->flight.departureTime.day);
					else
						ImGui::Text("%d", p->flight.departureTime.day);
					ImGui::SameLine(0, 0);
					ImGui::Text(" / ");
					ImGui::SameLine(0, 0);
					if (p->flight.departureTime.month < 10)
						ImGui::Text("0%d", p->flight.departureTime.month);
					else
						ImGui::Text("%d", p->flight.departureTime.month);
					ImGui::SameLine(0, 0);
					ImGui::Text(" / ");
					ImGui::SameLine(0, 0);
					ImGui::Text("%d", p->flight.departureTime.year);
					break;
				case 2:
					if (p->flight.departureTime.hour < 10)
						ImGui::Text("0%d", p->flight.departureTime.hour);
					else
						ImGui::Text("%d", p->flight.departureTime.hour);
					ImGui::SameLine(0, 0);
					ImGui::Text(" : ");
					ImGui::SameLine(0, 0);
					if (p->flight.departureTime.minute < 10)
						ImGui::Text("0%d", p->flight.departureTime.minute);
					else
						ImGui::Text("%d", p->flight.departureTime.minute);
					break;
				case 3:
					ImGui::Text((p->flight.desAirport).c_str());
					break;
				case 4:
					switch (p->flight.stt)
					{
					case 0:
						ImGui::Text("Canceled");
						if (ImGui::BeginPopupContextItem(id.c_str()))
						{
							if (ImGui::Selectable("Undo Canceled"))
								newStatus = 1;
							ImGui::EndPopup();
						}
						break;
					case 1:
						if (p->flight.totalTicket == p->flight.maxTicket)
							p->flight.stt = 2;

						ImGui::Text("Tickets Available");
						if (ImGui::BeginPopupContextItem(id.c_str()))
						{
							if (ImGui::Selectable("Canceled"))
								newStatus = 0;
							if (ImGui::Selectable("Completed"))
								newStatus = 3;
							ImGui::EndPopup();
						}
						break;
					case 2:
						if (p->flight.totalTicket < p->flight.maxTicket)
							p->flight.stt = 1;
						ImGui::Text("Sold Out");
						if (ImGui::BeginPopupContextItem(id.c_str()))
						{
							if (ImGui::Selectable("Canceled"))
								newStatus = 0;
							if (ImGui::Selectable("Completed"))
								newStatus = 3;
							ImGui::EndPopup();
						}
						break;
					case 3:
						ImGui::Text("Completed");
						break;
					}

					if (ImGui::IsItemHovered())
					{
						if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
						{
							selected_row = countRows;
							// ImGui::PushID(FLIGHT_ID * countRows + column);
							// ImGui::OpenPopup("Change Status");
							// ImGui::PopID();
						}
					}

					break;
				case 5:
					ImGui::Text((p->flight.planeID).c_str());
					break;
				case 6:
					ImGui::Text("%d/%d", p->flight.totalTicket, p->flight.maxTicket);
					break;
				}
			}
			p = p->next;
			countRows++;
		}
		if (countRows < 30)
		{
			for (int row = countRows; row < 30; row++)
			{
				ImGui::TableNextRow();
				for (int column = 0; column < 7; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::Text("");
				}
			}
		}

		ImGui::EndTable();
	}

	confirm_change_status(pFirstFlight, newStatus, currentPage, selected_row);

	ImVec2 tableSize = ImGui::GetItemRectSize();

	ImVec2 currentMousePos = ImGui::GetMousePos();
	static bool isInTable = false;

	if (ImGui::IsMouseHoveringRect(tableCursorPos, ImVec2(tableCursorPos.x + tableSize.x, tableCursorPos.y + tableSize.y)) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		isInTable = true;
	}
	else
	{
		isInTable = false;
	}

	static ImVec2 FM_action_ButtonsCursorPos;

	if (!(showDayFilter || showMonthFilter || showYearFilter || showDestinationAirportFilter))
	{
		ImVec2 pageNavButton = ImVec2(30, 30);

		std::string pageInfo = std::to_string(currentPage + 1) + " / " + std::to_string(totalPages);
		int cursorX = viewportSize.x / 2 - pageNavButton.x - ITEM_SPACING - ImGui::CalcTextSize(pageInfo.c_str()).x / 2;
		ImGui::SetCursorPosX(cursorX);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 17);

		if (ImGui::Button("<|", pageNavButton))
		{
			if (currentPage < totalPages)
				currentPage--;
			if (currentPage < 0)
				currentPage = 0;
		}
		ImGui::SameLine();
		ImGui::Text(pageInfo.c_str());
		ImGui::SameLine();
		if (ImGui::Button("|>", pageNavButton))
		{
			if (currentPage >= 0)
				currentPage++;
			if (currentPage >= totalPages)
				currentPage = totalPages - 1;
		}

		FM_action_ButtonsCursorPos = ImGui::GetCursorPos();
	}

	ImGui::SetCursorPos(FM_action_ButtonsCursorPos);

	show_FM_action_buttons(pFirstFlight, planeList, root, selected_row, isInTable, currentPage, tableCursorPos);

	ImGui::End();
}

void add_flight_popup(FlightNodePTR &pFirstFlight, PlaneList &planeList, bool &show_add_flight_popup, int &selectedFlight, int &currentPage)
{
	ImGui::OpenPopup("Want to add a new flight?");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Want to add a new flight?", NULL, popupModalFlags))
	{
		ImGui::Separator();

		ImGui::Text("Flight Number");
		ImGui::SameLine();
		HelpMarker("VNxxxx: Vietnam Airlines\nVJxxxx: Vietjet Air\nQHxxxx: Bamboo Airways\nEKxxxx: Emirates Airlines\nJXxxxx: Starlux Airlines\nxxxx: 0000 -> 9999");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_FM);
		ImGui::Text(":  ");
		ImGui::SameLine();
		static int current_flightNumber_index = 0;
		static const char *current_flightNumber = flightNumbers[current_flightNumber_index];
		const int flightNumber_length = combo_length(flightNumbers);
		int dropdownBox2Digit = (BASE_WIDTH_DROPDOWN + DIGIT_WIDTH * 2) + DROPDOWN_WIDTH;
		ImGui::PushItemWidth(dropdownBox2Digit);
		draw_combo("##flight_number", flightNumbers, flightNumber_length, current_flightNumber_index, current_flightNumber);
		ImGui::PopItemWidth();

		struct TextFilters
		{
			// Return true to discard a character.
			static int emirates_filter(ImGuiInputTextCallbackData *data)
			{
				if (data->EventChar >= 'a' && data->EventChar <= 'z')
				{
					data->EventChar += 'A' - 'a';
					return false;
				}
				else if (data->EventChar >= 'A' && data->EventChar <= 'Z')
				{
					return false;
				}

				return true;
			}
		};
		ImGui::SameLine();
		static char flight_number[5] = "";
		ImGui::PushItemWidth(BOX_WIDTH_FM - dropdownBox2Digit - ITEM_SPACING);
		ImGui::InputText("##XXXX", flight_number, 5, ImGuiInputTextFlags_CharsDecimal);

		ImGui::Spacing();

		ImGui::Text("Departure Date");
		ImGui::SameLine();
		HelpMarker("DD/MM/YYYY");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_FM);
		ImGui::Text(":  ");
		ImGui::SameLine();
		int box2digit = BASE_WIDTH + DIGIT_WIDTH * 2;
		int box4digit = BASE_WIDTH + DIGIT_WIDTH * 4;
		int excessWidth = (BOX_WIDTH_FM - (box2digit * 2 + box4digit + ITEM_SPACING * 4 + DIGIT_WIDTH / 2 * 2)) / 3;
		box2digit += excessWidth;
		ImGui::PushItemWidth(box2digit);
		static int day = 1;
		static int month = 1;
		static int year = 2024;
		int maxDayInMonth[12];
		const int minDay = 1;
		const int minMonth = 1;
		const int maxMonth = 12;
		const int minYear = 2010;
		const int maxYear = 2099;
		find_max_day_in_month(maxDayInMonth, month, year);

		if (ImGui::InputInt("##day", &day, 0, 0, ImGuiInputTextFlags_CharsDecimal))
		{

			if (day < minDay)
				day = minDay;
			else if (day > maxDayInMonth[month - 1])
				day = maxDayInMonth[month - 1];
		}
		ImGui::SameLine();
		ImGui::Text("/");
		ImGui::SameLine();
		if (ImGui::InputInt("##month", &month, 0, 0, ImGuiInputTextFlags_CharsDecimal))
		{
			if (month < minMonth)
				month = minMonth;
			else if (month > maxMonth)
				month = maxMonth;
		}

		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Text("/");
		ImGui::SameLine();
		box4digit += (excessWidth - 2);
		ImGui::PushItemWidth(box4digit);
		if (ImGui::InputInt("##year", &year, 0, 0, ImGuiInputTextFlags_CharsDecimal))
		{
			if (year < minYear)
				year = minYear;
			else if (year > maxYear)
				year = maxYear;
		}
		ImGui::PopItemWidth();

		ImGui::Spacing();

		ImGui::Text("Departure Time");
		ImGui::SameLine();
		HelpMarker("HH:MM");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_FM);
		ImGui::Text(":  ");
		ImGui::SameLine();
		int timeBox = (BOX_WIDTH_FM - (ITEM_SPACING * 2 + DIGIT_WIDTH / 2)) / 2;
		ImGui::PushItemWidth(timeBox);
		static int departureHour = 0;
		const int minHour = 0;
		const int maxHour = 23;
		if (ImGui::InputInt("##departure_hour", &departureHour, 0, 0, ImGuiInputTextFlags_CharsDecimal))
		{
			if (departureHour < minHour)
				departureHour = minHour;
			else if (departureHour > maxHour)
				departureHour = maxHour;
		}
		ImGui::SameLine();
		ImGui::Text(":");
		ImGui::SameLine();
		static int departureMinute = 0;
		const int minMinute = 0;
		const int maxMinute = 59;
		if (ImGui::InputInt("##departure_minute", &departureMinute, 0, 0, ImGuiInputTextFlags_CharsDecimal))
		{
			if (departureMinute < minMinute)
				departureMinute = minMinute;
			if (departureMinute > maxMinute)
				departureMinute = maxMinute;
		}
		ImGui::PopItemWidth();

		ImGui::Spacing();

		ImGui::Text("Destination Airport");
		ImGui::SameLine();
		HelpMarker("SGN: Tan Son Nhat International Airport\nHAN: Noi Bai International Airport\nDAD: Da Nang International Airport\nHUI: Phu Bai International Airport\nPQC: Phu Quoc International Airport\nVCA: Can Tho International Airport\nVCS: Con Dao International Airport\nDIN: Dien Bien Phu Airport\nVKG: Rach Gia Airport\nBMV: Buon Ma Thuot Airport\nVII: Vinh International Airport\nUIH: Phu Cat Airport\nPXU: Pleiku Airport\nTHD: Thanh Hoa Airport\nTBB: Tuy Hoa Airport\nVCL: Chu Lai Airport\nVDO: Van Don International Airport\nVDH: Dong Hoi Airport\nVKG: Rach Gia Airport\nVII: Vinh International Airport\nUIH: Phu Cat Airport\nPXU: Pleiku Airport\nTHD: Thanh Hoa Airport\nTBB: Tuy Hoa Airport\nVCL: Chu Lai Airport\nVDO: Van Don International Airport\nVDH: Dong Hoi Airport\nVKG: Rach Gia Airport\nVII: Vinh International Airport\nUIH: Phu Cat Airport\nPXU: Pleiku Airport\nTHD: Thanh Hoa Airport\nTBB: Tuy Hoa Airport\nVCL: Chu Lai Airport\nVDO: Van Don International Airport\nVDH: Dong Hoi Airport");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_FM);
		ImGui::Text(":  ");
		ImGui::SameLine();
		static int current_destinationAirport_index = 0;
		static const char *current_destinationAirport = airports[current_destinationAirport_index];
		const int Airports_length = combo_length(airports);

		ImGui::PushItemWidth(BOX_WIDTH_FM);
		draw_combo("##destination_airport", airports, Airports_length, current_destinationAirport_index, current_destinationAirport);

		ImGui::Spacing();

		ImGui::Text("Aircraft Registration");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_FM);
		ImGui::Text(":  ");
		ImGui::SameLine();

		static int current_aircraftRegistration_index = 0;
		static const char *current_aircraftRegistration = planeList.nodes[current_aircraftRegistration_index]->planeID.c_str();
		// ImGui::PushItemWidth(BOX_WIDTH_AM);

		draw_combo("##aircraft_registration", planeList, current_aircraftRegistration_index, current_aircraftRegistration, current_flightNumber_index);
		ImGui::PopItemWidth();

		ImGui::Separator();

		int windowWidth = ImGui::GetWindowWidth();
		int saveButtonX = (windowWidth - 2 * cmdButtonSize.x - ITEM_SPACING) / 2;
		ImGui::SetCursorPosX(saveButtonX);

		if (ImGui::Button("Save", cmdButtonSize))
		{
			bool rightFormat = true;
			for (int i = 0; i < 4; i++)
				if (!is_decimal(flight_number[i]))
				{
					show_noti("Please enter in the right format!");
					rightFormat = false;
					break;
				}

			if (rightFormat)
			{
				if (insert_flight(pFirstFlight, current_flightNumber, flight_number, current_destinationAirport, planeList, current_aircraftRegistration_index, day, month, year, departureHour, departureMinute))
				{
					int totalFlights = count_flights(pFirstFlight);
					int index = 0;
					for (FlightNodePTR p = pFirstFlight; p != NULL; p = p->next)
					{
						std::string tempFlightNumber = current_flightNumber + std::string(flight_number);
						if (p->flight.flightNumber == tempFlightNumber)
						{
							selectedFlight = index % 30;
							currentPage = index / 30;

							break;
						}
						index++;
					}
					memset(flight_number, '\0', 5);
					day = 1;
					month = 1;
					year = 2024;
					departureHour = 0;
					departureMinute = 0;
					current_flightNumber_index = 0;
					current_flightNumber = flightNumbers[current_flightNumber_index];
					current_destinationAirport_index = 0;
					current_destinationAirport = airports[current_destinationAirport_index];
					current_aircraftRegistration_index = 0;
					current_aircraftRegistration = planeList.nodes[current_aircraftRegistration_index]->planeID.c_str();

					show_add_flight_popup = false;
				}
			}

			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", cmdButtonSize))
		{
			memset(flight_number, '\0', 5);
			day = 1;
			month = 1;
			year = 2024;
			departureHour = 0;
			departureMinute = 0;
			current_flightNumber_index = 0;
			current_flightNumber = flightNumbers[current_flightNumber_index];
			current_destinationAirport_index = 0;
			current_destinationAirport = airports[current_destinationAirport_index];
			current_aircraftRegistration_index = 0;
			current_aircraftRegistration = planeList.nodes[current_aircraftRegistration_index]->planeID.c_str();

			show_add_flight_popup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void show_FM_action_buttons(FlightNodePTR &pFirstFlight, PlaneList &planeList, PassengerNodesPTR &root, int &selected, bool &isInTable, int &currentPage, ImVec2 &tableCursorPos)
{
	ImVec2 viewportSize = ImGui::GetIO().DisplaySize;

	ImVec2 actionButtonsCursorPos = ImGui::GetCursorPos();
	button changeTimeButton("CHANGE TIME", actionButtonSize, actionButtonsCursorPos, 4);

	int widthSpacing = (viewportSize.x - 4 * actionButtonSize.x - 2 * changeTimeButton.x_pos) / 3;

	button addButton("ADD", changeTimeButton, widthSpacing, 4);
	ImGui::SetCursorPos(ImVec2(addButton.x_pos, addButton.y_pos));
	static bool show_add_flight_popup = false;
	if (ImGui::Button(addButton.name, actionButtonSize))
		show_add_flight_popup = true;
	if (show_add_flight_popup)
		add_flight_popup(pFirstFlight, planeList, show_add_flight_popup, selected, currentPage);

	button saveAndExitButton("SAVE & EXIT", addButton, widthSpacing, 4);
	ImGui::SetCursorPos(ImVec2(saveAndExitButton.x_pos, saveAndExitButton.y_pos));
	if (ImGui::Button(saveAndExitButton.name, actionButtonSize))
	{
		save_flight(pFirstFlight, flightFile);
		current_screen = MAIN_MENU;
		open_state[FLIGHT_MANAGEMENT] = false;
	}

	button bookTicketButton("BOOK TICKET", saveAndExitButton, widthSpacing, 4);

	ImVec2 controlButtonSize = ImGui::CalcTextSize("TICKET & PASSENGER CONTROL");
	controlButtonSize.x += 2 * ITEM_SPACING;
	controlButtonSize.y += 2 * ITEM_SPACING;
	button passengerAndTicketButton("TICKET & PASSENGER CONTROL", controlButtonSize.x, controlButtonSize.y, viewportSize.x - controlButtonSize.x - BASE_WIDTH, tableCursorPos.y - controlButtonSize.y - HEIGHT_SPACING * 2);

	static bool isInEditButton = false;
	if (ImGui::IsMouseHoveringRect(ImVec2(changeTimeButton.x_pos, changeTimeButton.y_pos), ImVec2(changeTimeButton.x_pos + changeTimeButton.width, changeTimeButton.y_pos + changeTimeButton.height)))
		isInEditButton = true;
	else
		isInEditButton = false;

	static bool isInDeleteButton = false;
	if (ImGui::IsMouseHoveringRect(ImVec2(bookTicketButton.x_pos, bookTicketButton.y_pos), ImVec2(bookTicketButton.x_pos + bookTicketButton.width, bookTicketButton.y_pos + bookTicketButton.height)))
		isInDeleteButton = true;
	else
		isInDeleteButton = false;

	static bool isInPassengerAndTicketButton = false;
	if (ImGui::IsMouseHoveringRect(ImVec2(passengerAndTicketButton.x_pos, passengerAndTicketButton.y_pos), ImVec2(passengerAndTicketButton.x_pos + passengerAndTicketButton.width, passengerAndTicketButton.y_pos + passengerAndTicketButton.height)))
		isInPassengerAndTicketButton = true;
	else
		isInPassengerAndTicketButton = false;

	if (!(isInTable) && !(isInEditButton) && !(isInDeleteButton) && !(isInPassengerAndTicketButton))
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			selected = -1;

	if (selected != -1)
	{
		ImGui::SetCursorPos(ImVec2(changeTimeButton.x_pos, changeTimeButton.y_pos));
		static bool show_edit_flight_popup = false;
		if (ImGui::Button(changeTimeButton.name, actionButtonSize))
			show_edit_flight_popup = true;
		if (show_edit_flight_popup)
			edit_flight_popup(pFirstFlight, currentPage, selected, show_edit_flight_popup);

		ImGui::SetCursorPos(ImVec2(bookTicketButton.x_pos, bookTicketButton.y_pos));
		static bool show_book_ticket_popup = false;
		if (ImGui::Button(bookTicketButton.name, actionButtonSize))
			show_book_ticket_popup = true;
		if (show_book_ticket_popup)
			book_ticket_popup(pFirstFlight, planeList, root, currentPage, selected, show_book_ticket_popup);

		ImGui::SetCursorPos(ImVec2(passengerAndTicketButton.x_pos, passengerAndTicketButton.y_pos));
		static bool show_ticket_and_passenger_popup = false;
		if (ImGui::Button(passengerAndTicketButton.name, ImVec2(passengerAndTicketButton.width, passengerAndTicketButton.height)))
			show_ticket_and_passenger_popup = true;
		if (show_ticket_and_passenger_popup)
			ticket_and_passenger_popup(pFirstFlight, root, currentPage, selected, show_ticket_and_passenger_popup);
	}
}

void edit_flight_popup(FlightNodePTR &pFirstFlight, int &currentPage, int &row, bool &show_edit_flight_popup)
{
	FlightNodePTR p = pFirstFlight;
	for (int i = 0; i < currentPage * 30 + row; i++)
		p = p->next;
	ImGui::OpenPopup("Want to edit this flight?");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Want to edit this flight?", NULL, popupModalFlags))
	{
		ImGui::Separator();
		ImGui::Text("Flight Number");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_FM);
		ImGui::Text(":  ");
		ImGui::SameLine();
		ImGui::Text((p->flight.flightNumber).c_str());

		ImGui::Spacing();

		ImGui::Text("Departure Date");
		ImGui::SameLine();
		HelpMarker("DD/MM/YYYY");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_FM);
		ImGui::Text(":  ");
		ImGui::SameLine();
		int box2digit = BASE_WIDTH + DIGIT_WIDTH * 2;
		int box4digit = BASE_WIDTH + DIGIT_WIDTH * 4;
		int excessWidth = (BOX_WIDTH_FM - (box2digit * 2 + box4digit + ITEM_SPACING * 4 + DIGIT_WIDTH / 2 * 2)) / 3;
		box2digit += excessWidth;
		ImGui::PushItemWidth(box2digit);
		static int day = p->flight.departureTime.day;
		static int month = p->flight.departureTime.month;
		static int year = p->flight.departureTime.year;
		int maxDayInMonth[12];
		const int minDay = 1;
		const int minMonth = 1;
		const int maxMonth = 12;
		const int minYear = 2024;
		const int maxYear = 2099;
		find_max_day_in_month(maxDayInMonth, month, year);

		if (ImGui::InputInt("##day", &day, 0, 0, ImGuiInputTextFlags_CharsDecimal))
		{

			if (day < minDay)
				day = minDay;
			else if (day > maxDayInMonth[month - 1])
				day = maxDayInMonth[month - 1];
		}
		ImGui::SameLine();
		ImGui::Text("/");
		ImGui::SameLine();
		if (ImGui::InputInt("##month", &month, 0, 0, ImGuiInputTextFlags_CharsDecimal))
		{
			if (month < minMonth)
				month = minMonth;
			else if (month > maxMonth)
				month = maxMonth;
		}

		ImGui::PopItemWidth();
		ImGui::SameLine();
		ImGui::Text("/");
		ImGui::SameLine();
		box4digit += (excessWidth - 2);
		ImGui::PushItemWidth(box4digit);
		if (ImGui::InputInt("##year", &year, 0, 0, ImGuiInputTextFlags_CharsDecimal))
		{
			if (year < minYear)
				year = minYear;
			else if (year > maxYear)
				year = maxYear;
		}
		ImGui::PopItemWidth();

		ImGui::Spacing();

		ImGui::Text("Departure Time");
		ImGui::SameLine();
		HelpMarker("HH:MM");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_FM);
		ImGui::Text(":  ");
		ImGui::SameLine();
		int timeBox = (BOX_WIDTH_FM - (ITEM_SPACING * 2 + DIGIT_WIDTH / 2)) / 2;
		ImGui::PushItemWidth(timeBox);
		static int departureHour = p->flight.departureTime.hour;
		int minHour = 0;
		int maxHour = 23;
		if (ImGui::InputInt("##departure_hour", &departureHour, 0, 0, ImGuiInputTextFlags_CharsDecimal))
		{
			if (departureHour < minHour)
				departureHour = minHour;
			else if (departureHour > maxHour)
				departureHour = maxHour;
		}
		ImGui::SameLine();
		ImGui::Text(":");
		ImGui::SameLine();
		static int departureMinute = p->flight.departureTime.minute;
		int minMinute = 0;
		int maxMinute = 59;
		if (ImGui::InputInt("##departure_minute", &departureMinute, 0, 0, ImGuiInputTextFlags_CharsDecimal))
		{
			if (departureMinute < minMinute)
				departureMinute = minMinute;
			if (departureMinute > maxMinute)
				departureMinute = maxMinute;
		}
		ImGui::PopItemWidth();

		ImGui::Spacing();

		ImGui::Text("Destination Airport");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_FM);
		ImGui::Text(":  ");
		ImGui::SameLine();

		ImGui::Text((p->flight.desAirport).c_str());

		ImGui::Spacing();

		ImGui::Text("Aircraft Registration");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_FM);
		ImGui::Text(":  ");
		ImGui::SameLine();
		ImGui::Text((p->flight.planeID).c_str());
		ImGui::PopItemWidth();

		ImGui::Separator();

		int windowWidth = ImGui::GetWindowWidth();
		int saveButtonX = (windowWidth - 2 * cmdButtonSize.x - ITEM_SPACING) / 2;
		ImGui::SetCursorPosX(saveButtonX);

		if (ImGui::Button("Save", cmdButtonSize))
		{
			p->flight.departureTime.day = day;
			p->flight.departureTime.month = month;
			p->flight.departureTime.year = year;
			p->flight.departureTime.hour = departureHour;
			p->flight.departureTime.minute = departureMinute;
			show_edit_flight_popup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", cmdButtonSize))
		{
			show_edit_flight_popup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void book_ticket_popup(FlightNodePTR &pFirstFlight, PlaneList &planeList, PassengerNodesPTR &root, int &currentPage, int &selectedRow, bool &show_book_ticket_popup)
{
	FlightNodePTR p = pFirstFlight;
	for (int i = 0; i < currentPage * 30 + selectedRow; i++)
		p = p->next;

	int seatNum, rowNum;
	for (int i = 0; i < planeList.totalPlane; i++)
	{
		if (planeList.nodes[i]->planeID == p->flight.planeID)
		{
			seatNum = planeList.nodes[i]->seatNum;
			rowNum = planeList.nodes[i]->rowNum;
			break;
		}
	}
	static int indexInFlight = 0;

	ImGui::OpenPopup("Book Ticket");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Book Ticket", NULL, popupModalFlags | ImGuiWindowFlags_NoTitleBar))
	{
		ImGui::Text("Please select seat(s) you want to book");

		ImGui::Separator();

		const int columns_count = seatNum + 1;
		static bool isSeatBooked[100][11] = {};
		static bool isSeatSelected[100][11] = {};
		static bool isFirstTime = true;
		if (isFirstTime)
		{
			int index = 0;
			for (int column = 0; column < seatNum; column++)
				for (int row = 0; row < rowNum; row++)
				{
					isSeatSelected[row][column] = false;
					if (p->flight.ticketList[index].inUse)
						isSeatBooked[row][column] = true;
					else
						isSeatBooked[row][column] = false;
					index++;
				}
			isFirstTime = false;
		}

		const int frozen_cols = 1;
		const int frozen_rows = 1;
		float TABLE_WIDTH = 388;
		TABLE_WIDTH = 32 + 30 * seatNum + 14;
		const float TABLE_HEIGHT = 307;
		static bool selectedSeat = false;
		bool moreThanOne = false;

		ImVec2 OuterSize = ImVec2(TABLE_WIDTH, TABLE_HEIGHT);
		ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders;
		if (ImGui::BeginTable("Tickets", columns_count, flags, OuterSize))
		{

			ImGui::TableSetupColumn("");
			for (int n = 1; n < columns_count; n++)
				ImGui::TableSetupColumn(seats[n - 1], ImGuiTableColumnFlags_None);
			ImGui::TableSetupScrollFreeze(frozen_cols, frozen_rows);

			ImGui::TableHeadersRow(); // Draw remaining headers and allow access to context-menu and other functions.
			for (int row = 0; row < rowNum; row++)
			{
				ImGui::PushID(row);
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGui::Text(rows[row].c_str());
				for (int column = 1; column < columns_count; column++)
					if (ImGui::TableSetColumnIndex(column))
					{
						ImGui::PushID(column);
						if (isSeatBooked[row][column - 1])
							ImGui::Text("  B");
						else if (ImGui::Checkbox("", &isSeatSelected[row][column - 1]))
							if (isSeatSelected[row][column - 1] == false)
							{
								selectedSeat = false;
							}
							else if (selectedSeat)
							{
								isSeatSelected[row][column - 1] = false;
							}
							else if (isSeatSelected[row][column - 1])
							{
								indexInFlight = (column - 1) * rowNum + row;
								std::cout << indexInFlight << std::endl;
								selectedSeat = true;
							}

						ImGui::PopID();
					}
				ImGui::PopID();
			}
			ImGui::EndTable();
		}

		if (selectedSeat)
		{
			ImGui::SameLine();
			ImGui::BeginGroup();
			ImGui::Text("Tickets Available");
			ImGui::SameLine(0, 0);
			int posX = ImGui::GetCursorPosX();
			ImGui::Text(": ");
			ImGui::SameLine();
			int countTicketsAvailable = rowNum * seatNum;
			for (int row = 0; row < rowNum; row++)
				for (int column = 0; column < seatNum; column++)
				{
					if (isSeatBooked[row][column] || isSeatSelected[row][column])
						countTicketsAvailable--;
				}
			ImGui::Text("%d", countTicketsAvailable);

			ImGui::Text("Selected Seats");
			ImGui::SameLine(0, 0);
			ImGui::SetCursorPosX(posX);
			ImGui::Text(": ");
			ImGui::SameLine();

			std::string tempSeat = p->flight.ticketList[indexInFlight].ticketID;
			ImGui::Text("%s", tempSeat.c_str());

			ImGui::Separator();

			ImGui::EndGroup();
		}

		ImGui::Text("(\"B\" means this seat is already booked)");
		ImGui::Text(" by another passenger)");
		ImGui::Separator();

		int windowWidth = ImGui::GetWindowWidth();
		int saveButtonX = (windowWidth - 2 * cmdButtonSize.x - ITEM_SPACING) / 2;
		ImGui::SetCursorPosX(saveButtonX);

		static bool show_input_passengerID_popup = false;
		if (show_input_passengerID_popup)
		{
			input_passengerID_popup(root, p, indexInFlight, show_input_passengerID_popup, show_book_ticket_popup, isFirstTime, selectedSeat);
		}

		if (ImGui::Button("Book", cmdButtonSize))
		{
			show_input_passengerID_popup = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Back", cmdButtonSize))
		{
			isFirstTime = true;
			selectedSeat = false;
			show_book_ticket_popup = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void input_passengerID_popup(PassengerNodesPTR &root, FlightNodePTR &p, int &ticketIndex, bool &show_input_passengerID_popup, bool &show_book_ticket_popup, bool &isFirstTime, bool &selectedSeat)
{
	ImGui::OpenPopup("Enter Passenger's ID");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Enter Passenger's ID", NULL, popupModalFlags))
	{
		ImGui::Text("Please enter passenger's ID to book this seat!");

		ImGui::Separator();

		ImGui::Text("Passenger's ID:");
		ImGui::SameLine();
		static char passengerIDBuf[13] = "";
		ImGui::InputText("##passenger_ID", passengerIDBuf, 13, ImGuiInputTextFlags_CharsHexadecimal);

		ImGui::Separator();

		ImGui::Text("By \"Confirm\", you agreed to book this seat for the passenger with ID above!");

		int windowWidth = ImGui::GetWindowWidth();
		int saveButtonX = (windowWidth - 2 * cmdButtonSize.x - ITEM_SPACING) / 2;
		ImGui::SetCursorPosX(saveButtonX);

		static bool show_add_passenger_popup = false;
		if (show_add_passenger_popup)
		{
			add_passenger_popup(root, p, ticketIndex, passengerIDBuf, show_add_passenger_popup, show_input_passengerID_popup, show_book_ticket_popup, isFirstTime, selectedSeat);
		}

		static bool show_confirm_id_popup = false;
		static bool confirmation = false;
		if (show_confirm_id_popup)
		{
			confirm_id(passengerIDBuf, show_confirm_id_popup, confirmation);
		}

		if (isPassengerExist(root, passengerIDBuf) && confirmation)
		{
			book_ticket(p, passengerIDBuf, ticketIndex);
			memset(passengerIDBuf, '\0', 13);
			isFirstTime = true;
			selectedSeat = false;
			confirmation = false;
			show_input_passengerID_popup = false;
			show_book_ticket_popup = false;
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::Button("Confirm", cmdButtonSize))
		{
			bool rightFormat = true;
			if (passengerIDBuf[11] == '\0')
			{
				rightFormat = false;
				show_noti("Please enter 12 hexadecimal characters!");
			}
			if (rightFormat)
			{
				if (isPassengerExist(root, passengerIDBuf) && confirmation == false)
				{
					bool isNotInThisFlight = true;
					for (int i = 0; i < p->flight.maxTicket; i++)
						if (p->flight.ticketList[i].passengerID == passengerIDBuf && p->flight.ticketList[i].inUse)
						{
							isNotInThisFlight = false;
							break;
						}
					if (isNotInThisFlight)
						show_confirm_id_popup = true;
					else
						show_noti("This passenger is already in this flight!");
				}
				else
					show_add_passenger_popup = true;
			}
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Back", cmdButtonSize))
		{
			memset(passengerIDBuf, '\0', 13);
			show_input_passengerID_popup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void add_passenger_popup(PassengerNodesPTR &root, FlightNodePTR &p, int &indexInFlight, char (&passengerIDBuf)[13], bool &show_add_passenger_popup, bool &show_input_passengerID_popup, bool &show_book_ticket_popup, bool &isFirstTime, bool &selectedSeat)
{
	ImGui::OpenPopup("It seems the first time the passenger book a ticket");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("It seems the first time the passenger book a ticket", NULL, popupModalFlags))
	{
		ImGui::Text("Please enter the information!");

		ImGui::Separator();

		ImGui::Text("Passenger's ID");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_PM);
		ImGui::Text(":  ");
		ImGui::SameLine();
		// static char passengerIDBuf[13] = "";
		ImGui::PushItemWidth(BOX_WIDTH_PM);
		ImGui::InputText("##passenger_ID", passengerIDBuf, 13, ImGuiInputTextFlags_CharsHexadecimal);
		ImGui::PopItemWidth();

		ImGui::Spacing();

		ImGui::Text("First Name");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_PM);
		ImGui::Text(":  ");
		ImGui::SameLine();
		static char firstNameBuf[51] = "";
		ImGui::PushItemWidth(BOX_WIDTH_PM);
		ImGui::InputText("##first_name", firstNameBuf, 51, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CharsUppercase);
		ImGui::PopItemWidth();

		ImGui::Spacing();

		ImGui::Text("Last Name");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_PM);
		ImGui::Text(":  ");
		ImGui::SameLine();
		static char lastNameBuf[51] = "";
		ImGui::PushItemWidth(BOX_WIDTH_PM);
		ImGui::InputText("##last_name", lastNameBuf, 51, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CharsUppercase);
		ImGui::PopItemWidth();

		ImGui::Spacing();

		ImGui::Text("Gender");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_PM);
		ImGui::Text(":  ");
		ImGui::SameLine();
		static bool isMale = false;
		ImGui::Checkbox("(Check if you are Male)", &isMale);

		ImGui::Separator();

		ImGui::Text("By \"Save\", This passenger will be added to the database");
		ImGui::Text("        and the ticket will be booked for this passenger!");

		int windowWidth = ImGui::GetWindowWidth();
		int saveButtonX = (windowWidth - 2 * cmdButtonSize.x - ITEM_SPACING) / 2;
		ImGui::SetCursorPosX(saveButtonX);

		if (ImGui::Button("Save", cmdButtonSize))
		{
			bool rightFormat = true;
			if (passengerIDBuf[11] == '\0' || firstNameBuf[0] == '\0' || lastNameBuf[0] == '\0')
			{
				rightFormat = false;
				show_noti("Please enter the right format!");
			}
			if (rightFormat)
			{
				insert_passenger(root, passengerIDBuf, firstNameBuf, lastNameBuf, isMale);
				book_ticket(p, passengerIDBuf, indexInFlight);
				memset(passengerIDBuf, '\0', 13);
				memset(firstNameBuf, '\0', 51);
				memset(lastNameBuf, '\0', 51);
				isMale = false;
				isFirstTime = true;
				selectedSeat = false;
				show_add_passenger_popup = false;
				show_input_passengerID_popup = false;
				show_book_ticket_popup = false;
			}
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", cmdButtonSize))
		{
			memset(passengerIDBuf, '\0', 13);
			memset(firstNameBuf, '\0', 51);
			memset(lastNameBuf, '\0', 51);
			isMale = false;
			isMale = false;
			isFirstTime = true;
			show_add_passenger_popup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void confirm_id(char (&passengerIDBuf)[13], bool &show_confirm_id_popup, bool &confirmation)
{
	ImGui::OpenPopup("Confirm to book this seat?");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Confirm to book this seat?", NULL, popupModalFlags))
	{
		ImGui::Text("It seems the passenger's ID is already in the database");

		ImGui::Separator();

		ImGui::Text("Passenger's ID:");
		ImGui::SameLine();
		ImGui::Text(passengerIDBuf);

		ImGui::Separator();

		ImGui::Text("By \"Confirm\", you agreed to book this seat after checking the passenger's ID!");

		int windowWidth = ImGui::GetWindowWidth();
		int saveButtonX = (windowWidth - 2 * cmdButtonSize.x - ITEM_SPACING) / 2;
		ImGui::SetCursorPosX(saveButtonX);

		if (ImGui::Button("Confirm", cmdButtonSize))
		{
			confirmation = true;
			show_confirm_id_popup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Back", cmdButtonSize))
		{
			show_confirm_id_popup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void ticket_and_passenger_popup(FlightNodePTR &pFirstFlight, PassengerNodesPTR &root, int &currentPage, int &selectedRow, bool &show_ticket_and_passenger_popup)
{
	FlightNodePTR p = pFirstFlight;
	for (int i = 0; i < currentPage * 30 + selectedRow; i++)
		p = p->next;

	ImGui::OpenPopup("Ticket & Passenger Information");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	static int selectedTicket = -1;
	static int newStatus = -1;
	static bool show_passenger_information = false;
	static bool show_ticket_information = true;
	static bool showButton = false;

	if (ImGui::BeginPopupModal("Ticket & Passenger Information", NULL, popupModalFlags))
	{
		ImGui::Spacing();
		if (ImGui::Button("Passenger", ImVec2(0.0f, 0.0f)))
		{
			show_passenger_information = true;
			show_ticket_information = false;
			showButton = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Ticket", ImVec2(0.0f, 0.0f)))
		{
			show_passenger_information = false;
			show_ticket_information = true;
			showButton = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Close", ImVec2(0.0f, 0.0f)))
		{
			show_passenger_information = false;
			show_ticket_information = false;
			showButton = true;
		}
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (show_passenger_information)
		{
			float popupWitdh = ImGui::GetWindowWidth();
			ImGui::PushFont(popup_header_font);
			std::string header = "PASSENGER OF FLIGHT " + p->flight.flightNumber;
			ImVec2 headerSize = ImGui::CalcTextSize(header.c_str());
			float currentCursorPos = ImGui::GetCursorPosX() + (popupWitdh - headerSize.x) / 2 - BASE_WIDTH;
			ImGui::SetCursorPosX(currentCursorPos);
			ImGui::Text(header.c_str());
			ImGui::PopFont();

			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Text("Departure: ");
			ImGui::SameLine();
			if (p->flight.departureTime.day < 10)
				ImGui::Text("0%d", p->flight.departureTime.day);
			else
				ImGui::Text("%d", p->flight.departureTime.day);
			ImGui::SameLine(0, 0);
			ImGui::Text("/");
			ImGui::SameLine(0, 0);
			if (p->flight.departureTime.month < 10)
				ImGui::Text("0%d", p->flight.departureTime.month);
			else
				ImGui::Text("%d", p->flight.departureTime.month);
			ImGui::SameLine(0, 0);
			ImGui::Text("/");
			ImGui::SameLine(0, 0);
			ImGui::Text("%d", p->flight.departureTime.year);

			ImGui::SameLine();

			if (p->flight.departureTime.hour < 10)
				ImGui::Text("0%d", p->flight.departureTime.hour);
			else
				ImGui::Text("%d", p->flight.departureTime.hour);
			ImGui::SameLine(0, 0);
			ImGui::Text(":");
			ImGui::SameLine(0, 0);
			if (p->flight.departureTime.minute < 10)
				ImGui::Text("0%d", p->flight.departureTime.minute);
			else
				ImGui::Text("%d", p->flight.departureTime.minute);

			ImGui::Text("Destination Airport: ");
			ImGui::SameLine();
			ImGui::Text(p->flight.desAirport.c_str());

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImVec2 OuterSize = ImVec2(0.0f, 520.7f);
			if (ImGui::BeginTable("Passenger Information", 5, tableFlags | ImGuiTableFlags_ScrollY | ImGuiTableFlags_NoHostExtendY, OuterSize))
			{
				float width_size = 100.0f;
				ImGui::TableSetupColumn("NO.", ImGuiTableColumnFlags_WidthFixed, width_size / 4);
				ImGui::TableSetupColumn("Ticket ID", ImGuiTableColumnFlags_WidthFixed, width_size);
				ImGui::TableSetupColumn("Passenger ID", ImGuiTableColumnFlags_WidthFixed, width_size);
				ImGui::TableSetupColumn("Full Name", ImGuiTableColumnFlags_WidthFixed, width_size);
				ImGui::TableSetupColumn("Gender", ImGuiTableColumnFlags_WidthFixed, width_size / 3 + 14.0f);
				ImGui::TableHeadersRow();

				ImGui::TableNextRow();
				int No_ = 0;
				for (int i = 0; i < p->flight.maxTicket; i++)
				{
					if (p->flight.ticketList[i].inUse)
					{
						PassengerNodesPTR pTemp = search_passenger(root, p->flight.ticketList[i].passengerID);

						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%d", ++No_);
						ImGui::TableSetColumnIndex(1);
						ImGui::Text(p->flight.ticketList[i].ticketID.c_str());
						ImGui::TableSetColumnIndex(2);
						ImGui::Text(p->flight.ticketList[i].passengerID.c_str());
						ImGui::TableSetColumnIndex(3);
						std::string fullName = pTemp->passenger.firstName + " " + pTemp->passenger.lastName;
						ImGui::Text(fullName.c_str());
						ImGui::TableSetColumnIndex(4);
						if (pTemp->passenger.gender)
							ImGui::Text("Mr");
						else
							ImGui::Text("Ms");
					}
				}
				if (No_ == 0)
				{
					ImGui::TableNextRow();
					ImGui::Text("No passenger booked ticket for this flight!");
				}
				ImGui::EndTable();
			}

			ImGui::Spacing();
			ImGui::Separator();
		}
		else if (show_ticket_information)
		{
			static bool showAvailableTicket = false;
			static bool showBookedTicket = false;
			ImGui::Text("Filter: ");
			ImGui::SameLine();
			float posX = ImGui::GetCursorPosX();
			ImGui::Checkbox("Show booked tickets", &showBookedTicket);
			ImGui::SetCursorPosX(posX);
			ImGui::Checkbox("Show available tickets", &showAvailableTicket);

			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Text("Ticket Information");
			ImGui::Spacing();

			ImVec2 OuterSize = ImVec2(0.0f, 520.7f);
			if (ImGui::BeginTable("Ticket Information", 3, tableFlags | ImGuiTableFlags_ScrollY | ImGuiTableFlags_NoHostExtendY, OuterSize))
			{
				float width_size = 100.0f;
				ImGui::TableSetupColumn("TicketID", ImGuiTableColumnFlags_WidthFixed, width_size);
				ImGui::TableSetupColumn("PassengerID", ImGuiTableColumnFlags_WidthFixed, width_size);
				ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, width_size + 14.0f);
				ImGui::TableHeadersRow();

				ImGui::TableNextRow();

				for (int i = 0; i < p->flight.maxTicket; i++)
				{
					if (showBookedTicket && p->flight.ticketList[i].inUse == false)
						continue;
					if (showAvailableTicket && p->flight.ticketList[i].inUse == true)
						continue;
					std::string id = "##" + std::to_string(TICKET_ID * i + i);
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					if (ImGui::Selectable(p->flight.ticketList[i].ticketID.c_str(), selectedTicket == i, ImGuiSelectableFlags_SpanAllColumns))
						selectedTicket = i;
					ImGui::TableSetColumnIndex(1);
					ImGui::Text(p->flight.ticketList[i].passengerID.c_str());
					ImGui::TableSetColumnIndex(2);
					if (p->flight.ticketList[i].inUse)
					{
						ImGui::Text("Booked");
						if (ImGui::BeginPopupContextItem(id.c_str()))
						{
							ImGui::Text("You can cancel booking ticket for this seat!");
							ImGui::Text("By click the button: ");
							ImGui::SameLine(0, 0);
							if (ImGui::Button("Cancel Booking"))
								newStatus = 0;
							ImGui::EndPopup();
						}
					}
					else
					{
						ImGui::Text("Available");
					}

					if (ImGui::IsItemHovered())
					{
						if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
						{
							selectedTicket = i;
						}
					}
				}
				ImGui::EndTable();
			}

			if (newStatus != -1)
			{
				confirm_cancel_ticket(p, selectedTicket, newStatus);
			}
		}
		else if (showButton)
		{
			show_passenger_information = false;
			show_ticket_information = true;
			showButton = false;
			show_ticket_and_passenger_popup = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void confirm_cancel_ticket(FlightNodePTR &p, int &selectedTicket, int &newStatus)
{
	ImGui::OpenPopup("Confirm to cancel booking?");

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Confirm to cancel booking?", NULL, popupModalFlags))
	{
		ImGui::Text("Are you sure you want to cancel booking this ticket?");

		ImGui::Separator();

		ImGui::Text("Ticket ID:");
		ImGui::SameLine();
		ImGui::Text(p->flight.ticketList[selectedTicket].ticketID.c_str());

		ImGui::Text("Passenger ID:");
		ImGui::SameLine();
		ImGui::Text(p->flight.ticketList[selectedTicket].passengerID.c_str());

		ImGui::Text("Status:");
		ImGui::SameLine();
		ImGui::Text("Booked");

		ImGui::Separator();

		ImGui::Text("By \"Confirm\", you agreed to cancel booking this ticket!");

		int windowWidth = ImGui::GetWindowWidth();
		int saveButtonX = (windowWidth - 2 * cmdButtonSize.x - ITEM_SPACING) / 2;
		ImGui::SetCursorPosX(saveButtonX);

		if (ImGui::Button("Confirm", cmdButtonSize))
		{
			if (newStatus == 0)
			{
				p->flight.ticketList[selectedTicket].inUse = false;
				p->flight.totalTicket--;
				newStatus = -1;
			}
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Back", cmdButtonSize))
		{
			newStatus = -1;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void ranking_table_popup(PlaneList &planeList, FlightNodePTR &pFirstFlight, bool &show_ranking_table_popup)
{
	ImGui::OpenPopup("Ranking Table");

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Ranking Table", NULL, popupModalFlags | ImGuiWindowFlags_NoTitleBar))
	{
		float popupWitdh = ImGui::GetWindowWidth();
		ImGui::PushFont(popup_header_font);
		const char *header = "RAKING TABLE";
		ImVec2 headerSize = ImGui::CalcTextSize(header);
		float currentCursorPos = ImGui::GetCursorPosX() + (popupWitdh - headerSize.x) / 2 - BASE_WIDTH;
		ImGui::SetCursorPosX(currentCursorPos);
		ImGui::Text(header);
		ImGui::PopFont();

		for (int i = 0; i < 7; i++)
			ImGui::Spacing();

		int flightCount[MAXPLANE] = {};
		int descendingSortedIndex[MAXPLANE] = {};
		int maxFlight = 0;
		for (int i = 0; i < planeList.totalPlane; i++)
		{
			flightCount[i] = 0;
			FlightNodePTR p = pFirstFlight;
			while (p != NULL)
			{
				if (p->flight.planeID == planeList.nodes[i]->planeID && p->flight.stt == 3)
					flightCount[i]++;
				if (flightCount[i] > maxFlight)
					maxFlight = flightCount[i];
				p = p->next;
			}
		}
		for (int i = planeList.totalPlane; i < MAXPLANE; i++)
		{
			flightCount[i] = -1;
			descendingSortedIndex[i] = -1;
		}

		int currentIndex = 0;
		while (maxFlight > -1)
		{
			for (int i = 0; i < planeList.totalPlane; i++)
			{
				if (flightCount[i] == maxFlight)
				{
					descendingSortedIndex[currentIndex] = i;
					currentIndex++;
				}
			}
			maxFlight--;
		}

		static int selected_row = -1;
		static int currentPage = 0;
		int totalPages = planeList.totalPlane / 20;
		if (planeList.totalPlane % 20)
			totalPages++;

		if (ImGui::BeginTable("Ranking Table", 2, tableFlags))
		{
			ImGui::TableSetupColumn("Plane ID", ImGuiTableColumnFlags_WidthFixed, 200.0f);
			ImGui::TableSetupColumn("Completed Flight(s)", ImGuiTableColumnFlags_WidthFixed, 150.0f);
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			int countRows = 0;
			for (; countRows < 20 && ((currentPage * 20 + countRows) < planeList.totalPlane); countRows++)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(planeList.nodes[descendingSortedIndex[currentPage * 20 + countRows]]->planeID.c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%d", flightCount[descendingSortedIndex[currentPage * 20 + countRows]]);
			}

			ImGui::EndTable();
		}

		ImVec2 pageNavButton = ImVec2(30, 30);

		std::string pageInfo = std::to_string(currentPage + 1) + " / " + std::to_string(totalPages);
		int cursorX = popupWitdh / 2 - pageNavButton.x - ITEM_SPACING - ImGui::CalcTextSize(pageInfo.c_str()).x / 2;
		ImGui::SetCursorPosX(cursorX);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 17);

		if (ImGui::Button("<|", pageNavButton))
		{
			if (currentPage < totalPages)
				currentPage--;
			if (currentPage < 0)
				currentPage = 0;
		}
		ImGui::SameLine();
		ImGui::Text(pageInfo.c_str());
		ImGui::SameLine();
		if (ImGui::Button("|>", pageNavButton))
		{
			if (currentPage >= 0)
				currentPage++;
			if (currentPage >= totalPages)
				currentPage = totalPages - 1;
		}

		for (int i = 0; i < 7; i++)
			ImGui::Spacing();

		int windowWidth = ImGui::GetWindowWidth();
		int saveButtonX = (windowWidth - cmdButtonSize.x) / 2;
		ImGui::SetCursorPosX(saveButtonX);

		if (ImGui::Button("Close", cmdButtonSize))
		{
			show_ranking_table_popup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();

		ImGui::EndPopup();
	}
}

void draw_passenger_management_screen(PassengerNodesPTR &treeRoot, ImFont *&headerFont)
{
	open_state[PASSENGER_MANAGEMENT] = true;
	ImVec2 viewportSize = ImGui::GetIO().DisplaySize;

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(viewportSize);
	ImGui::Begin("Passenger Management", &open_state[PASSENGER_MANAGEMENT], fixed_full_screen);

	ImGui::PushFont(headerFont);
	const char *header = "PASSENGER MANAGEMENT";
	ImVec2 headerSize = ImGui::CalcTextSize(header);
	ImGui::SetCursorPosX((viewportSize.x - headerSize.x) / 2);
	ImGui::Text(header);
	ImGui::PopFont();

	ImGui::Text("");
	ImGui::Text("");

	static int selectedRow = -1;
	static std::string selectedID = "";
	PassengerNodesPTR p = treeRoot;
	int countRows = 0;
	ImVec2 OuterSize = ImVec2(0.0f, 666.0f);
	if (ImGui::BeginTable("Passenger Information", 3, tableFlags | ImGuiTableFlags_ScrollY | ImGuiTableFlags_NoHostExtendY, OuterSize))
	{
		ImGui::TableSetupColumn("Passenger ID");
		ImGui::TableSetupColumn("Full Name");
		ImGui::TableSetupColumn("Gender");
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();

		print_inoder(treeRoot, selectedRow, countRows, selectedID);

		ImGui::EndTable();
	}

	ImGui::Spacing();

	show_PM_action_buttons(treeRoot, selectedID, selectedRow, countRows);
	ImGui::End();
}

void show_PM_action_buttons(PassengerNodesPTR &treeRoot, std::string &passengerID, int &selectedRow, int &countRows)
{
	ImVec2 viewportSize = ImGui::GetIO().DisplaySize;

	ImVec2 actionButtonsCursorPos = ImGui::GetCursorPos();
	button firstButton("FIRST BUTTON", actionButtonSize, actionButtonsCursorPos, 4);

	int widthSpacing = (viewportSize.x - 4 * actionButtonSize.x - 2 * firstButton.x_pos) / 3;

	button addButton("ADD", firstButton, widthSpacing, 4);
	ImGui::SetCursorPos(ImVec2(addButton.x_pos, addButton.y_pos));
	static bool show_add_passenger_popup = false;
	if (ImGui::Button(addButton.name, actionButtonSize))
		show_add_passenger_popup = true;
	if (show_add_passenger_popup)
		add_passenger_popup(treeRoot, passengerID, selectedRow, show_add_passenger_popup);

	button deleteButton("DELETE", addButton, widthSpacing, 4);
	ImGui::SetCursorPos(ImVec2(deleteButton.x_pos, deleteButton.y_pos));
	static bool show_delete_passenger_popup = false;
	if (ImGui::Button(deleteButton.name, actionButtonSize))
		show_delete_passenger_popup = true;
	if (show_delete_passenger_popup)
		delete_passenger_popup(treeRoot, passengerID, selectedRow, show_delete_passenger_popup);
}

void add_passenger_popup(PassengerNodesPTR &tree, std::string &passengerID, int &selectedRow, bool &show_add_passenger_popup)
{
	PassengerNodesPTR p = search_passenger(tree, passengerID);

	static char passengerIDBuf[13] = "";
	static char firstNameBuf[51] = "";
	static char lastNameBuf[51] = "";
	static bool isMale = false;
	

	ImGui::OpenPopup("Want to add a passenger?");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Want to add a passenger?", NULL, popupModalFlags))
	{
		ImGui::Separator();

		ImGui::Text("Passenger's ID:");
		ImGui::SameLine();

		ImGui::Text("Please enter the information!");

		ImGui::Separator();

		ImGui::Text("Passenger's ID");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_PM);
		ImGui::Text(":  ");
		ImGui::SameLine();
		ImGui::PushItemWidth(BOX_WIDTH_PM);
		ImGui::InputText("##passenger_ID", passengerIDBuf, 13, ImGuiInputTextFlags_CharsHexadecimal);
		ImGui::PopItemWidth();

		ImGui::Spacing();

		ImGui::Text("First Name");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_PM);
		ImGui::Text(":  ");
		ImGui::SameLine();
		ImGui::PushItemWidth(BOX_WIDTH_PM);
		ImGui::InputText("##first_name", firstNameBuf, 51, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CharsUppercase);
		ImGui::PopItemWidth();

		ImGui::Spacing();

		ImGui::Text("Last Name");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_PM);
		ImGui::Text(":  ");
		ImGui::SameLine();
		ImGui::PushItemWidth(BOX_WIDTH_PM);
		ImGui::InputText("##last_name", lastNameBuf, 51, ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CharsUppercase);
		ImGui::PopItemWidth();

		ImGui::Spacing();

		ImGui::Text("Gender");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_PM);
		ImGui::Text(":  ");
		ImGui::SameLine();
		ImGui::Checkbox("(Check if you are Male)", &isMale);

		ImGui::Separator();

		ImGui::Separator();

		int windowWidth = ImGui::GetWindowWidth();
		int saveButtonX = (windowWidth - 2 * cmdButtonSize.x - ITEM_SPACING) / 2;
		ImGui::SetCursorPosX(saveButtonX);

		if (ImGui::Button("Save", cmdButtonSize))
		{
			bool rightFormat = true;
			if (passengerIDBuf[11] == '\0' || firstNameBuf[0] == '\0' || lastNameBuf[0] == '\0')
			{
				rightFormat = false;
				show_noti("Please enter the right format!");
			}
			if (rightFormat)
			{
				if (isPassengerExist(tree, passengerIDBuf))
					show_noti("This passenger ID is already in the database!");
				else
				{
					insert_passenger(tree, passengerIDBuf, firstNameBuf, lastNameBuf, isMale);
					show_noti("Add passenger successfully!");
					memset(passengerIDBuf, '\0', 13);
					memset(firstNameBuf, '\0', 51);
					memset(lastNameBuf, '\0', 51);
					isMale = false;
					show_add_passenger_popup = false;
				}
			}
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", cmdButtonSize))
		{
			memset(passengerIDBuf, '\0', 13);
			memset(firstNameBuf, '\0', 51);
			memset(lastNameBuf, '\0', 51);
			isMale = false;
			show_add_passenger_popup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void delete_passenger_popup(PassengerNodesPTR &treeRoot, std::string &passengerID, int &selectedRow, bool &show_delete_passenger_popup)
{
	ImGui::OpenPopup("Want to delete a plane?");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Want to delete a plane?", NULL, popupModalFlags))
	{
		ImGui::Separator();

		ImGui::Text("Are you sure you want to delete this plane?");

		ImGui::Separator();

		int windowWidth = ImGui::GetWindowWidth();
		int saveButtonX = (windowWidth - 2 * cmdButtonSize.x - ITEM_SPACING) / 2;
		ImGui::SetCursorPosX(saveButtonX);

		if (ImGui::Button("Yes", cmdButtonSize))
		{
			delete_passenger(treeRoot, passengerID);
			selectedRow = -1;
			show_delete_passenger_popup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("No", cmdButtonSize))
		{
			selectedRow = -1;
			show_delete_passenger_popup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}
