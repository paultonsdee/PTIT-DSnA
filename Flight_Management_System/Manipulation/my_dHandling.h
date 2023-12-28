#include "Declaration/my_fProt.h"

std::string aircraftFile = "./Media/aircraft.txt";
std::string flightFile = "./Media/flight.txt";

void string_uppercase(std::string &);
bool check_valid_planeID(std::string &);
void string_uppercase(std::string &str)
{
	for (int i = 0; i < 2; i++)
		if (str[i] >= 'a' && str[i] <= 'z')
			str[i] -= 32;
}

int string_length(std::string &str)
{
	int length = 0;
	while (str[length] != '\0')
		length++;
	return length;
}

bool is_planeList_empty(PlaneList &planeList)
{
	return planeList.totalPlane == 0;
}

bool is_planeList_full(PlaneList &planeList)
{
	return planeList.totalPlane == MAXPLANE;
}

void display_error(std::string error)
{
	std::cout << std::endl
			  << error << std::endl;
}

void find_max_day_in_month(int *maxDayInMonth, int &month, int &year)
{
	for (int i = 0; i < 12; i++)
	{
		int month = i + 1;
		if (month < 8)
		{
			if (month % 2)
				maxDayInMonth[i] = 31;
			else
				maxDayInMonth[i] = 30;
		}
		else if (month % 2)
			maxDayInMonth[i] = 30;
		else
			maxDayInMonth[i] = 31;
	}
	if ((year % 4 == 0) && (year % 100) ||
		(year % 4 == 0) && (year % 100) && (year % 400))
		maxDayInMonth[1] = 29;
	else
		maxDayInMonth[1] = 28;
}

bool insert_plane(PlaneList &planeList, int &pCurrent_airline_index, char aircraft_registration_buf[], const char *&pCurrent_aircraftType, int &total_seats, int &total_rows)
{
	if (is_planeList_full(planeList))
	{
		std::cout << "The aircraft list is full";
		return 0;
	}

	planeList.nodes[planeList.totalPlane] = new Plane;
	Plane *plane = planeList.nodes[planeList.totalPlane];

	std::string aircraftRegistration;
	switch (pCurrent_airline_index)
	{
	case 0:
	case 1:
	case 2:
		aircraftRegistration = "VN-";
		break;
	case 3:
		aircraftRegistration = "A6-";
		break;
	case 4:
		aircraftRegistration = "B-";
		break;
	}

	plane->planeID = aircraftRegistration + aircraft_registration_buf;
	plane->planeType = pCurrent_aircraftType;
	plane->seatNum = total_seats;
	plane->rowNum = total_rows;

	std::cout << "aircraft created successfully!" << std::endl;

	planeList.totalPlane++;

	return 1;
}

void edit_plane(PlaneList &planeList, int &pCurrent_airline_index, char aircraft_registration_buf[], const char *&pCurrent_aircraftType, int &total_seats, int &total_rows, int &planeIndex)
{
	Plane *plane = planeList.nodes[planeIndex];

	std::string aircraftRegistration;
	switch (pCurrent_airline_index)
	{
	case 0:
	case 1:
	case 2:
		aircraftRegistration = "VN-";
		break;
	case 3:
		aircraftRegistration = "A6-";
		break;
	case 4:
		aircraftRegistration = "B-";
		break;
	}

	plane->planeID = aircraftRegistration + aircraft_registration_buf;
	plane->planeType = pCurrent_aircraftType;
	plane->seatNum = total_seats;
	plane->rowNum = total_rows;

	std::cout << "aircraft edited successfully!" << std::endl;
}

// void save_aircraft(PlaneList &planeList, std::string filename) //binary file
// {
// 	std::ofstream f(filename, std::ios::binary);
// 	if (f.is_open())
// 		for (int i = 0; i < planeList.totalPlane; i++)
// 		{
// 			f.write((char *)&planeList.nodes[i], sizeof(Plane));
// 		}
// 	else
// 	{
// 		std::cout << "An error occurred while opening file" << std::endl;
// 		return;
// 	}
// 	f.close();
// 	std::cout << "Saved to aircraft file successfully!" << std::endl;
// }

void save_aircraft(PlaneList &planeList, std::string &filename) // txt file
{
	std::ofstream f(filename);

	if (f.is_open())
		for (int i = 0; i < planeList.totalPlane; i++)
		{
			f << planeList.nodes[i]->planeID << '\n'
			  << planeList.nodes[i]->planeType << '\n'
			  << planeList.nodes[i]->seatNum << '\n'
			  << planeList.nodes[i]->rowNum << '\n';
		}
	else
	{
		std::cout << "An error occurred while opening aircraft file" << std::endl;
		return;
	}

	f.close();
	std::cout << "Saved to aircraft file successfully!" << std::endl;
}

// void load_aircraft(PlaneList &planeList, std::string filename) //binary file
// {
// 	std::ifstream f;
// 	f.open(filename, std::ios::binary);
// 	Plane plane;
// 	if (f.is_open())
// 		while (f.read((char *)&plane, sizeof(Plane)))
// 		{
// 			planeList.nodes[planeList.totalPlane] = new Plane;
// 			*planeList.nodes[planeList.totalPlane] = plane;
// 			planeList.totalPlane++;
// 		}
// 	else
// 	{
// 		std::cout << "An error occurred while opening file" << std::endl;
// 		return;
// 	}
// 	f.close();
// 	std::cout << "Loaded from aircraft file successfully!" << std::endl;
// }

void load_aircraft(PlaneList &planeList, std::string filename) // txt file
{
	std::ifstream f;
	f.open(filename);
	Plane plane;

	if (f.is_open())
		while (std::getline(f, plane.planeID) &&
			   std::getline(f, plane.planeType) &&
			   (f >> plane.seatNum) && f.ignore() &&
			   (f >> plane.rowNum) && f.ignore())
		{
			planeList.nodes[planeList.totalPlane] = new Plane;
			*planeList.nodes[planeList.totalPlane] = plane;
			planeList.totalPlane++;
		}
	else
	{
		std::cout << "An error occurred while opening aircraft file" << std::endl;
		return;
	}

	f.close();
	std::cout << "Loaded from aircraft file successfully!" << std::endl;
}

void delete_plane(PlaneList &planeList, int &planeIndex)
{
	// if (planeIndex < 0 || planeIndex >= planeList.totalPlane || is_planeList_empty(planeList))
	// 	return;
	for (int i = planeIndex; i < planeList.totalPlane - 1; i++)
		planeList.nodes[i] = planeList.nodes[i + 1];
	planeList.totalPlane--;
}

void insert_flight(FlightNodePTR &First, const char *&flightNumber1, char (&flightNumber2)[5], const char *&desAirport, PlaneList &planeList, int &selectedPlane, int &day, int &month, int &year, int &hour, int &minute)
{
	FlightNodePTR p = new_flight();

	Flight *flight = &p->flight;
	flight->flightNumber = flightNumber1;
	flight->flightNumber += flightNumber2;
	flight->desAirport = desAirport;
	flight->stt = 1;
	flight->planeID = planeList.nodes[selectedPlane]->planeID;
	flight->totalTicket = 0;
	flight->maxTicket = planeList.nodes[selectedPlane]->seatNum * planeList.nodes[selectedPlane]->rowNum;
	flight->ticketList = new Ticket[flight->maxTicket];

	// Ticket *ticket = &flight->ticketList[flight->totalTicket];
	// ticket->ticketID = ;
	// ticket->passengerID = ;
	// ticket->inUse = 0;

	DT *departureTime = &flight->departureTime;
	departureTime->day = day;
	departureTime->month = month;
	departureTime->year = year;
	departureTime->hour = hour;
	departureTime->minute = minute;

	// p->next = NULL;
	// if (First == NULL)
	// 	First = p;
	// else
	// {
	// 	FlightNodePTR Last = NULL;
	// 	for (Last = First; Last->next != NULL; Last = Last->next)
	// 		;
	// 	Last->next = p;
	// }

	if ((First == NULL) || (First->flight.flightNumber > flight->flightNumber))
	{
		p->next = First;
		First = p;
	}
	else
	{
		FlightNodePTR pCurrent = First;
		while ((pCurrent->next != NULL) && (pCurrent->next->flight.flightNumber < flight->flightNumber))
		{
			pCurrent = pCurrent->next;
		}
		p->next = pCurrent->next;
		pCurrent->next = p;
	}

	std::cout << "flight created successfully!" << std::endl;
}

void link_list_initialize(FlightNodePTR &First)
{
	First = NULL;
}

FlightNodePTR new_flight(void)
{
	FlightNodePTR p = new FlightNode;
	return p;
}

int count_flights(FlightNodePTR First)
{
	int count = 0;
	FlightNodePTR p = First;
	while (p != NULL)
	{
		count++;
		p = p->next;
	}
	return count;
}

void save_flight(FlightNodePTR &First, std::string &filename)
{
	std::ofstream f(filename);

	if (f.is_open())
	{
		FlightNodePTR p = First;
		while (p != NULL)
		{
			f << p->flight.flightNumber << '\n'
			  << p->flight.planeID << '\n'
			  << p->flight.desAirport << '\n'
			  << p->flight.departureTime.day << '\n'
			  << p->flight.departureTime.month << '\n'
			  << p->flight.departureTime.year << '\n'
			  << p->flight.departureTime.hour << '\n'
			  << p->flight.departureTime.minute << '\n'
			  << p->flight.totalTicket << '\n'
			  << p->flight.maxTicket << '\n'
			  << p->flight.stt << '\n';
			p = p->next;
		}
	}
	else
	{
		std::cout << "An error occurred while opening flight file" << std::endl;
		return;
	}

	f.close();
	std::cout << "Saved to flight file successfully!" << std::endl;
}

void load_flight(FlightNodePTR &First, std::string &filename)
{
	std::ifstream f(filename);
	Flight flight;

	std::string flightNumber, planeID, desAirport;
	int day, month, year, hour, minute, totalTicket, maxTicket, stt;

	if (f.is_open())
		while (std::getline(f, flightNumber) &&
			   std::getline(f, planeID) &&
			   std::getline(f, desAirport) &&
			   (f >> day) && f.ignore() &&
			   (f >> month) && f.ignore() &&
			   (f >> year) && f.ignore() &&
			   (f >> hour) && f.ignore() &&
			   (f >> minute) && f.ignore() &&
			   (f >> totalTicket) && f.ignore() &&
			   (f >> maxTicket) && f.ignore() &&
			   (f >> stt) && f.ignore())
		{
			FlightNodePTR p = new_flight();

			Flight *flight = &p->flight;
			flight->flightNumber = flightNumber;
			flight->desAirport = desAirport;
			flight->planeID = planeID;
			flight->totalTicket = totalTicket;
			flight->maxTicket = maxTicket;
			flight->ticketList = new Ticket[maxTicket];
			flight->stt = stt;

			DT *departureTime = &flight->departureTime;
			departureTime->day = day;
			departureTime->month = month;
			departureTime->year = year;
			departureTime->hour = hour;
			departureTime->minute = minute;

			if ((First == NULL) || (First->flight.flightNumber > flight->flightNumber))
			{
				p->next = First;
				First = p;
			}
			else
			{
				FlightNodePTR pCurrent = First;
				while ((pCurrent->next != NULL) && (pCurrent->next->flight.flightNumber < flight->flightNumber))
				{
					pCurrent = pCurrent->next;
				}
				p->next = pCurrent->next;
				pCurrent->next = p;
			}
		}

	else
	{
		std::cout << "An error occurred while opening flight file" << std::endl;
		return;
	}

	f.close();
	std::cout << "Loaded from flight file successfully!" << std::endl;
}