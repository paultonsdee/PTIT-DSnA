#include "my_fProt.h"

int VIEWPORT_WIDTH = 1600;
int VIEWPORT_HEIGHT = 900;
#define BASE_WIDTH 8
#define BASE_WIDTH_DROPDOWN 16
#define DIGIT_WIDTH 8
#define COLON_POSX_AM 165
#define BOX_WIDTH_AM 184
#define ITEM_SPACING 8
#define HEIGHT_SPACING 4
#define HEIGHT_SPACING_PLUS 20
#define BOX_POSX_AM 174
#define combo_length(arr) int(sizeof(arr) / sizeof(arr[0]))
#define COLON_POSX_FM 157
#define BOX_WIDTH_FM 168
#define DROPDOWN_WIDTH 20
#define COLON_POSX_PM 157
#define BOX_WIDTH_PM 168

const int AIRCRAFT_ID = 87;
const int FLIGHT_ID = 178;
const int CONFIRM_ID = 257;
const int TICKET_ID = 111;

bool showNoti = false;
std::string notiMessage = "";
float notiStartTime = 0.0f;
const int timeDisplayNoti = 2507;

bool showPopupNoti = false;
std::string popupNotiMessage = "";
float popupNotiStartTime = 0.0f;

SDL_Window *gWindow = NULL;			// The window we'll be rendering to
SDL_Renderer *gRenderTarget = NULL; // The window renderer
SDL_Texture *gBackGround = NULL;	// The image we will load and show on the screen

ImVec4 clear_color;
ImVec2 cmdButtonSize = ImVec2(70, 30);
ImVec2 actionButtonSize = ImVec2(117, 41);

ImFont *popup_header_font;
ImFont *noti_font;

struct button
{
	const char *name;
	int width;
	int height;
	int x_pos;
	int y_pos;

	button(const char *name, int w, int h, int x, int y)
	{
		this->name = name;
		width = w;
		height = h;
		x_pos = x;
		y_pos = y;
	}

	button(const char *name, int w, int h)
	{
		this->name = name;
		width = w;
		height = h;
		x_pos = (VIEWPORT_WIDTH - width) / 2;
		y_pos = (VIEWPORT_HEIGHT - height) / 2;
	}

	button(const char *name, ImVec2 buttonSize, ImVec2 CursorPos, int totalButton)
	{
		switch (totalButton)
		{
		case 4:
			this->name = name;
			width = buttonSize.x;
			height = buttonSize.y;
			x_pos = CursorPos.x;
			y_pos = CursorPos.y + HEIGHT_SPACING_PLUS;
		}
	}

	button(const char *name, button &beforebutton, int heightSpace)
	{
		this->name = name;
		width = beforebutton.width;
		height = beforebutton.height;
		x_pos = beforebutton.x_pos;
		y_pos = beforebutton.y_pos + beforebutton.height + heightSpace;
	}

	button(const char *name, button &beforebutton, int widthSpace, int totalButton)
	{
		this->name = name;
		width = beforebutton.width;
		height = beforebutton.height;
		x_pos = beforebutton.x_pos + beforebutton.width + widthSpace;
		y_pos = beforebutton.y_pos;
	}
};

enum screen
{
	NONE,
	MAIN_MENU,
	AIRCRAFT_MANAGEMENT,
	FLIGHT_MANAGEMENT,
	TICKET_MANAGEMENT,
	// DEMO_WINDOW,
};
bool open_state[] = {false, false, false, false, false};

#define fixed_full_screen ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
#define notification ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize
#define tableFlags ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter
#define popupModalFlags ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize

screen current_screen = NONE;

const char *airlines[] = {"Vietnam Airlines", "Vietjet Air", "Bamboo Airways", "Emirates Airlines", "Starlux Airlines"};
const char *flightNumbers[] = {"VN", "VJ", "QH", "EK", "JX"};
const char *airports[] = {
		"Dien Bien Phu (DIN)",
		"Tho Xuan (THD)",
		"Dong Hoi (VDH)",
		"Chu Lai (VCL)",
		"Tuy Hoa (TBB)",
		"Pleiku (PXU)",
		"Buon Ma Thuot (BMV)",
		"Rach Gia (VKG)",
		"Ca Mau (CAH)",
		"Con Dao (VCS)",
		"Noi Bai (HAN)",
		"Tan Son Nhat (SGN)",
		"Da Nang (DAD)",
		"Van Don (VDO)",
		"Cat Bi (HPH)",
		"Vinh (VII)",
		"Phu Bai (HUI)",
		"Cam Ranh (CXR)",
		"Lien Khuong (DLI)",
		"Phu Cat (UIH)",
		"Can Tho (VCA)",
		"Phu Quoc (PQC)"};

const char *VN_types[] = {"Airbus A330", "Airbus A321", "Airbus A350", "Boeing 787", "ATR 72-500"};
const char *VJ_types[] = {"Airbus A320", "Airbus A321neo", "Boeing 737 MAX 200"};
const char *QH_types[] = {"Airbus A320neo", "Airbus A321CEO", "Embraer 190", "Boeing 787-9 Dreamliner"};
const char *EK_types[] = {"Airbus A380", "Airbus A340-500", "Boeing 777", "Boeing 777-200ER"};
const char *JX_types[] = {"Airbus A321neo ACF", "Airbus A330-900", "Airbus A350-900"};

const char **aircraftTypes[] = {VN_types, VJ_types, QH_types, EK_types, JX_types};
const int aircraftTypesLength[] = {combo_length(VN_types), combo_length(VJ_types), combo_length(QH_types), combo_length(EK_types), combo_length(JX_types)};

const char *seats[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K"};
std::string rows[100];