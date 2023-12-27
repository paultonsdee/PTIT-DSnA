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

bool is_in_array(std::string &planeType, const char *arr[], const int &aircraftTypesIndex)
{
	for (int i = 0; i < aircraftTypesLength[aircraftTypesIndex]; i++)
	{
		if (planeType == arr[i])
			return true;
	}
	return false;
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

	button DemoWindowButton("Show demo window", FlightManagementButton, 25);
	ImGui::SetCursorPos(ImVec2(DemoWindowButton.x_pos, DemoWindowButton.y_pos));
	if (ImGui::Button("Show demo window", ImVec2(DemoWindowButton.width, DemoWindowButton.height)))
	{
		current_screen = DEMO_WINDOW;
		open_state[MAIN_MENU] = false;
	}

	button CloseWindowButton("Close Window", DemoWindowButton, 25);
	ImGui::SetCursorPos(ImVec2(CloseWindowButton.x_pos, CloseWindowButton.y_pos));
	if (ImGui::Button("Close Window", ImVec2(CloseWindowButton.width, CloseWindowButton.height)))
	{
		current_screen = NONE;
		open_state[MAIN_MENU] = false;
	}

	ImGui::End();
}

void draw_aircraft_management_screen(PlaneList &planeList, ImFont *&headerFont)
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

	ImGui::Text("From here you can control all the aircrafts!");
	HelpMarker(
		"Using TableSetupColumn() to alter resizing policy on a per-column basis.\n\n"
		"When combining Fixed and Stretch columns, generally you only want one, maybe two trailing columns to use _WidthStretch.");
	// static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

	static ImGuiTableFlags flags = ImGuiTableFlags_RowBg |
								   ImGuiTableFlags_BordersOuter;
	//    ImGuiTableFlags_SizingStretchSame|
	//    ImGuiTableFlags_ScrollX
	//    ImGuiTableFlags_SizingFixedFit;

	// ImGuiTableFlags_NoBordersInBody, HighlightHoveredColumn, ImGuiTableFlags_ScrollY

	ImVec2 tableCursorPos = ImGui::GetCursorPos();

	static int selected_row = -1;
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
			for (int row = 0; row < planeList.totalPlane; row++)
			{

				ImGui::TableNextRow();

				for (int column = 0; column < 3; column++)
				{
					ImGui::TableSetColumnIndex(column);
					Plane *plane = planeList.nodes[row];
					switch (column)
					{
					case 0:
						if (ImGui::Selectable((plane->planeID).c_str(), selected_row == row, ImGuiSelectableFlags_SpanAllColumns))
							selected_row = row;
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
			for (int row = planeList.totalPlane; row < 30; row++)
			{
				ImGui::TableNextRow();
				for (int column = 0; column < 3; column++)
				{
					ImGui::TableSetColumnIndex(column);
					ImGui::Text("");
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
	show_AM_action_buttons(planeList, selected_row, isInTable);

	ImGui::End();
}

void show_AM_action_buttons(PlaneList &planeList, int &selected, bool &isInTable)
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
		add_plane_popup(planeList, show_add_plane_popup, selected);

	button saveAndExitButton("SAVE & EXIT", addButton, widthSpacing, 4);
	ImGui::SetCursorPos(ImVec2(saveAndExitButton.x_pos, saveAndExitButton.y_pos));
	if (ImGui::Button(saveAndExitButton.name, actionButtonSize))
	{
		save_aircraft(planeList, aircraftFile);
		current_screen = MAIN_MENU;
		open_state[AIRCRAFT_MANAGEMENT] = false;
	}

	button deleteButton("DELETE", saveAndExitButton, widthSpacing, 4);

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
			edit_plane_popup(planeList, show_edit_plane_popup, selected);

		ImGui::SetCursorPos(ImVec2(deleteButton.x_pos, deleteButton.y_pos));
		static bool show_delete_plane_popup = false;
		if (ImGui::Button(deleteButton.name, actionButtonSize))
			show_delete_plane_popup = true;
		if (show_delete_plane_popup)
		{
			delete_plane(planeList, selected);
			show_delete_plane_popup = false;
		}
	}
}

void add_plane_popup(PlaneList &planeList, bool &show_add_plane_popup, int &currentAircraft)
{
	ImGui::OpenPopup("Want to add a plane?");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Want to add a plane?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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
			if (insert_plane(planeList, current_airline_index, aircraft_registration_buf, current_aircraftType[current_airline_index], total_seats, total_rows))
			{
				for (int i = 0; i < 6; i++)
				{
					aircraft_registration_buf[i] = '\0';
				}
				currentAircraft = planeList.totalPlane - 1;
			}

			show_add_plane_popup = false;
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

void edit_plane_popup(PlaneList &planeList, bool &show_add_plane_popup, int &selectedPlane)
{
	ImGui::OpenPopup("Want to edit a plane?");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Want to edit a plane?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		// ImGui::Text("Dont forget to save data!\nThis operation cannot be undone!");
		ImGui::Separator();

		ImGui::Text("Airlines");
		ImGui::SameLine();
		ImGui::SetCursorPosX(COLON_POSX_AM);
		ImGui::Text(":  ");
		ImGui::SameLine();

		static int current_airline_index = 0;
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
		static int total_seats = planeList.nodes[selectedPlane]->seatNum;
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
		static int total_rows = planeList.nodes[selectedPlane]->rowNum;
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
			edit_plane(planeList, current_airline_index, aircraft_registration_buf, current_aircraftType[current_airline_index], total_seats, total_rows, selectedPlane);
			for (int i = 0; i < 6; i++)
			{
				aircraft_registration_buf[i] = '\0';
			}
			temp_i = 0;
			startFlag = false;
			startIndex = 0;
			successLoadAircraftType = false;
			selectedPlane = -1;
			show_add_plane_popup = false;
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
			temp_i = 0;
			startFlag = false;
			startIndex = 0;
			successLoadAircraftType = false;
			selectedPlane = -1;
			show_add_plane_popup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void show_delete_plane_popup(PlaneList &planeList, int &selectedPlane)
{
	ImGui::OpenPopup("Want to delete a plane?");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Want to delete a plane?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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

void draw_flight_management_screen(FlightNodePTR &pFirstFlight, PlaneList &planeList, ImFont *&headerFont)
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

	ImGui::Text("From here you can control all the flights!");
	HelpMarker(
		"Using TableSetupColumn() to alter resizing policy on a per-column basis.\n\n"
		"When combining Fixed and Stretch columns, generally you only want one, maybe two trailing columns to use _WidthStretch.");
	// static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

	static ImGuiTableFlags flags = ImGuiTableFlags_RowBg |
								   ImGuiTableFlags_BordersOuter;
	//    ImGuiTableFlags_SizingStretchSame|
	//    ImGuiTableFlags_ScrollX
	//    ImGuiTableFlags_SizingFixedFit;

	// Always center this window when appearing
	ImVec2 tableCursorPos = ImGui::GetCursorPos();

	static int selected_row = -1;
	static int currentPage = 0;
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

		ImGui::TableNextRow();
		FlightNodePTR p = pFirstFlight;
		for (int i = 0; i < 30 * currentPage; i++)
			p = p->next;
		int countRows = 0;
		while (countRows < 30 && p != NULL)
		{
			ImGui::TableNextRow();
			for (int column = 0; column < 7; column++)
			{
				ImGui::TableSetColumnIndex(column);
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
						break;
					case 1:
						ImGui::Text("Tickets Available");
						break;
					case 2:
						ImGui::Text("Sold Out");
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
							ImGui::PushID(FLIGHT_ID * countRows + column);
							ImGui::OpenPopup("Change Status");
							ImGui::PopID();
						}
					}
					
					ImGui::PushID(FLIGHT_ID * countRows + column);
					if (ImGui::BeginPopup("Change Status"))
					{
						if (ImGui::Selectable("Canceled"))
							p->flight.stt = 0;
						if (ImGui::Selectable("Tickets Available"))
							p->flight.stt = 1;
						if (ImGui::Selectable("Sold Out"))
							p->flight.stt = 2;
						if (ImGui::Selectable("Completed"))
							p->flight.stt = 3;
						ImGui::EndPopup();
					}
					ImGui::PopID();

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

	ImVec2 tableSize = ImGui::GetItemRectSize();

	ImVec2 currentMousePos = ImGui::GetMousePos();
	static bool isInTable = false;
	if (ImGui::IsMouseHoveringRect(tableCursorPos, ImVec2(tableCursorPos.x + tableSize.x, tableCursorPos.y + tableSize.y))
		&& ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		isInTable = true;
	}
	else
	{
		isInTable = false;
	}

	if(ImGui::Button("<|", ImVec2(30, 30)))
	{
		if(currentPage < totalPages)
			currentPage--;
		if(currentPage < 0)
			currentPage = 0;
	}
	ImGui::SameLine();
	ImGui::Text("%d/%d", currentPage + 1, totalPages);
	ImGui::SameLine();
	if(ImGui::Button("|>", ImVec2(30, 30)))
	{
		if(currentPage >= 0)
			currentPage++;
		if(currentPage >= totalPages)
			currentPage = totalPages - 1;
	}

	show_FM_action_buttons(pFirstFlight, planeList, selected_row);

	ImGui::End();
}

void add_flight_popup(FlightNodePTR &pFirstFlight, PlaneList &planeList, bool &show_add_flight_popup, int &selectedFlight)
{
	ImGui::OpenPopup("Want to add a new flight?");

	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Want to add a new flight?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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
		static int departureHour = 0;
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
		static int departureMinute = 0;
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
			insert_flight(pFirstFlight, current_flightNumber, flight_number, current_destinationAirport, planeList, current_aircraftRegistration_index, day, month, year, departureHour, departureMinute);
			show_add_flight_popup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", cmdButtonSize))
		{
			show_add_flight_popup = false;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void show_FM_action_buttons(FlightNodePTR &pFirstFlight, PlaneList &planeList, int &selectedFlight)
{
	ImVec2 viewportSize = ImGui::GetIO().DisplaySize;

	ImVec2 actionButtonsCursorPos = ImGui::GetCursorPos();
	button editButton("FIRST BUTTON", actionButtonSize, actionButtonsCursorPos, 4);

	int widthSpacing = (viewportSize.x - 4 * actionButtonSize.x - 2 * editButton.x_pos) / 3;

	button addButton("ADD", editButton, widthSpacing, 4);
	ImGui::SetCursorPos(ImVec2(addButton.x_pos, addButton.y_pos));
	static bool show_add_flight_popup = false;
	if (ImGui::Button(addButton.name, actionButtonSize))
		show_add_flight_popup = true;
	if (show_add_flight_popup)
		add_flight_popup(pFirstFlight, planeList, show_add_flight_popup, selectedFlight);

	button saveAndExitButton("SAVE & EXIT", addButton, widthSpacing, 4);
	ImGui::SetCursorPos(ImVec2(saveAndExitButton.x_pos, saveAndExitButton.y_pos));
	if (ImGui::Button(saveAndExitButton.name, actionButtonSize))
	{
		save_flight(pFirstFlight, flightFile);
		current_screen = MAIN_MENU;
		open_state[FLIGHT_MANAGEMENT] = false;
	}
}