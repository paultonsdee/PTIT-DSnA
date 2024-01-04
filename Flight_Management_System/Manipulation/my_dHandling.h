#include "Declaration/my_fProt.h"

std::string aircraftFile = "./Media/aircraft.txt";
std::string flightFile = "./Media/flight.txt";
std::string ticketFile = "./Media/ticket.txt";
std::string passengerFile = "./Media/passenger.txt";

void string_uppercase(std::string &);
bool check_valid_planeID(std::string &);
void string_uppercase(std::string &str)
{
	for (int i = 0; i < 2; i++)
		if (str[i] >= 'a' && str[i] <= 'z')
			str[i] -= 32;
}

bool is_decimal(char &c)
{
	return (c >= '0' && c <= '9');
}

bool is_alpha(char &c)
{
	return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
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
		show_noti("The aircraft list is full");
		return false;
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
	for (int i = 0; i < planeList.totalPlane; i++)
		if (plane->planeID == planeList.nodes[i]->planeID)
		{
			show_noti("The aircraft ID already exists");
			return false;
		}
	plane->planeType = pCurrent_aircraftType;
	plane->seatNum = total_seats;
	plane->rowNum = total_rows;

	planeList.totalPlane++;

	show_noti("Aircraft added successfully!");

	return true;
}

bool edit_plane(PlaneList &planeList, FlightNodePTR &pFirstFlight, int &pCurrent_airline_index, char aircraft_registration_buf[], const char *&pCurrent_aircraftType, int &total_seats, int &total_rows, int &planeIndex)
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

	std::string planeID = aircraftRegistration + aircraft_registration_buf;
	for (int i = 0; i < planeList.totalPlane; i++)
		if ((planeID == planeList.nodes[i]->planeID) && (i != planeIndex))
		{
			show_noti("The aircraft ID already exists");
			return false;
		}

	if ((plane->rowNum != total_rows) || (plane->seatNum != total_seats))
	{
		FlightNodePTR p = pFirstFlight;
		while (p != NULL)
		{
			if (p->flight.planeID == plane->planeID && (p->flight.stt == 1 || p->flight.stt == 2))
			{
				show_noti("The aircraft is in use, cannot be edited");
				return false;
			}
			p = p->next;
		}

		p = pFirstFlight;
		while (p != NULL)
		{
			if (p->flight.planeID == plane->planeID)
			{
				delete[] p->flight.ticketList;
				p->flight.maxTicket = total_rows * total_seats;
				p->flight.ticketList = new Ticket[p->flight.maxTicket];

				std::string seats[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K"};
				for (int i = 0; i < p->flight.maxTicket; i++)
				{
					int seatIndex = i / total_rows;
					int rowIndex = i % total_rows + 1;
					p->flight.ticketList[i].ticketID = seats[seatIndex];
					if (total_rows == 100)
					{
						if (rowIndex < 10)
							p->flight.ticketList[i].ticketID += "00";
						else if (rowIndex < 100)
							p->flight.ticketList[i].ticketID += "0";
					}
					else if (total_rows > 9)
					{
						if (rowIndex < 10)
							p->flight.ticketList[i].ticketID += "0";
					}
					p->flight.ticketList[i].ticketID += std::to_string(rowIndex);

					p->flight.ticketList[i].passengerID = "";
					p->flight.ticketList[i].inUse = 0;
				}
			}
			p = p->next;
		}
	}

	plane->planeID = planeID;
	plane->planeType = pCurrent_aircraftType;
	plane->seatNum = total_seats;
	plane->rowNum = total_rows;

	show_noti("Aircraft edited successfully!");

	return true;
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

	show_noti("Aircraft deleted successfully!");
}

bool is_six_hour_dif_day(int &hour1, int &min1, int &hour2, int &min2)
{
	if (hour1 < 6 && hour2 >= 18)
	{
		int time1 = (hour1 + 24) * 60 + min1;
		int time2 = hour2 * 60 + min2;
		if (abs(time1 - time2) < 360)
		{
			show_noti("Cannot use the same plane for two flights within 6 hours");
			return false;
		}
	}
	else if (hour1 >= 18 && hour2 < 6)
	{
		int time1 = (hour2 + 24) * 60 + min2;
		int time2 = hour1 * 60 + min1;
		if (abs(time1 - time2) < 360)
			return false;
	}
	return true;
}

bool insert_flight(FlightNodePTR &First, const char *&flightNumber1, char (&flightNumber2)[5], const char *&desAirport, PlaneList &planeList, int &selectedPlane, int &day, int &month, int &year, int &hour, int &minute)
{
	FlightNodePTR p = new_flight();

	Flight *flight = &p->flight;
	std::string flightNumber = flightNumber1 + std::string(flightNumber2);

	for (FlightNodePTR pTemp = First; pTemp != NULL; pTemp = pTemp->next)
		if (pTemp->flight.flightNumber == flightNumber)
		{
			show_noti("The flight number already exists");
			return false;
		}
	std::string planeID = planeList.nodes[selectedPlane]->planeID;
	for (FlightNodePTR pTemp = First; pTemp != NULL; pTemp = pTemp->next)
		if (pTemp->flight.planeID == planeID)
		{
			if (pTemp->flight.departureTime.year == year && pTemp->flight.departureTime.month == month && pTemp->flight.departureTime.day == day)
			{
				int maxDayInMonth[12];
				const int minDay = 1;
				const int minMonth = 1;
				const int maxMonth = 12;
				find_max_day_in_month(maxDayInMonth, month, year);

				if ((abs(pTemp->flight.departureTime.year - year)) == 1)
				{
					if ((pTemp->flight.departureTime.month == 12 && month == 1 && pTemp->flight.departureTime.day == 31 && day == 1) || (pTemp->flight.departureTime.month == 1 && month == 12 && pTemp->flight.departureTime.day == 1 && day == 31))
						if (is_six_hour_dif_day(pTemp->flight.departureTime.hour, pTemp->flight.departureTime.minute, hour, minute) == false)
						{
							show_noti("Cannot use the same plane for two flights within 6 hours");
							return false;
						}
				}
				else if (pTemp->flight.departureTime.year == year)
				{
					if ((abs(pTemp->flight.departureTime.month - month)) == 1)
					{
						if ((pTemp->flight.departureTime.day == maxDayInMonth[pTemp->flight.departureTime.month] && day == 1) || (pTemp->flight.departureTime.day == 1 && month == maxDayInMonth[month]))
							if (is_six_hour_dif_day(pTemp->flight.departureTime.hour, pTemp->flight.departureTime.minute, hour, minute) == false)
							{
								show_noti("Cannot use the same plane for two flights within 6 hours");
								return false;
							}
					}
					else if (pTemp->flight.departureTime.month == month)
					{
						if ((abs(pTemp->flight.departureTime.day - day)) == 1)
						{
							if (is_six_hour_dif_day(pTemp->flight.departureTime.hour, pTemp->flight.departureTime.minute, hour, minute) == false)
							{
								show_noti("Cannot use the same plane for two flights within 6 hours");
								return false;
							}
						}
						else if (pTemp->flight.departureTime.day == day)
						{
							int time1 = pTemp->flight.departureTime.hour * 60 + pTemp->flight.departureTime.minute;
							int time2 = hour * 60 + minute;
							if (abs(time1 - time2) < 360)
							{
								show_noti("Cannot use the same plane for two flights within 6 hours");
								return false;
							}
						}
					}
				}
			}
		}

	flight->flightNumber = flightNumber;
	flight->planeID = planeID;
	flight->desAirport = desAirport;
	flight->stt = 1;
	flight->totalTicket = 0;
	flight->maxTicket = planeList.nodes[selectedPlane]->seatNum * planeList.nodes[selectedPlane]->rowNum;
	flight->ticketList = new Ticket[flight->maxTicket];

	std::string seats[] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K"};
	for (int i = 0; i < flight->maxTicket; i++)
	{
		int seatIndex = i / planeList.nodes[selectedPlane]->rowNum;
		int rowIndex = i % planeList.nodes[selectedPlane]->rowNum + 1;
		p->flight.ticketList[i].ticketID = seats[seatIndex];
		if (planeList.nodes[selectedPlane]->rowNum == 100)
		{
			if (rowIndex < 10)
				p->flight.ticketList[i].ticketID += "00";
			else if (rowIndex < 100)
				p->flight.ticketList[i].ticketID += "0";
		}
		else if (planeList.nodes[selectedPlane]->rowNum > 9)
		{
			if (rowIndex < 10)
				p->flight.ticketList[i].ticketID += "0";
		}
		p->flight.ticketList[i].ticketID += std::to_string(rowIndex);

		p->flight.ticketList[i].passengerID = "";
		p->flight.ticketList[i].inUse = 0;
	}

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

	show_noti("Flight added successfully!");

	return true;
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
			p->flight.ticketList = new Ticket[maxTicket];
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

void save_ticket(FlightNodePTR &First, std::string &filename)
{
	std::ofstream f(filename);

	if (f.is_open())
	{
		FlightNodePTR p = First;
		while (p != NULL)
		{
			for (int i = 0; i < p->flight.maxTicket; i++)
			{
				// if (p->flight.ticketList[i].ticketID == "")
				// 	f << p->flight.ticketList[i].ticketID << '\t'
				// 	  << "##EMPTY##" << '\t'
				// 	  << p->flight.ticketList[i].inUse << '\n';
				// else
				// 	f << p->flight.ticketList[i].ticketID << '\t'
				// 	  << p->flight.ticketList[i].passengerID << '\t'
				// 	  << p->flight.ticketList[i].inUse << '\n';

				f << p->flight.ticketList[i].ticketID << '\t';
				if (p->flight.ticketList[i].passengerID == "")
					f << "##EMPTY##" << '\t';
				else
					f << p->flight.ticketList[i].passengerID << '\t';
				f << p->flight.ticketList[i].inUse << '\n';
			}

			p = p->next;
		}
	}
	else
	{
		std::cout << "An error occurred while opening ticket file" << std::endl;
		return;
	}

	f.close();
	std::cout << "Saved to ticket file successfully!" << std::endl;
}

void load_ticket(FlightNodePTR &First, std::string &filename)
{
	std::ifstream f;
	f.open(filename);
	FlightNodePTR p = First;
	std::string ticketID, passengerID;
	bool inUse;

	if (f.is_open())
		while (p != NULL)
		{
			for (int i = 0; i < p->flight.maxTicket; i++)
			{
				f >> ticketID;
				f.ignore();
				f >> passengerID;
				f.ignore();
				f >> inUse;
				f.ignore();

				p->flight.ticketList[i].ticketID = ticketID;
				if (passengerID == "##EMPTY##")
					p->flight.ticketList[i].passengerID = "";
				else
					p->flight.ticketList[i].passengerID = passengerID;
				p->flight.ticketList[i].inUse = inUse;
			}
			p = p->next;
		}
	else
	{
		std::cout << "An error occurred while opening aircraft file" << std::endl;
		return;
	}

	f.close();
	std::cout << "Loaded from aircraft file successfully!" << std::endl;
}

void tree_initialize(PassengerNodesPTR &root)
{
	root = NULL;
}

void insert_passenger(PassengerNodesPTR &p, const std::string &ID, const std::string &firstName, const std::string &lastName, const bool &isMr)
{
	if (p == NULL) // nút p hiện tại sẽ là nút lá
	{
		p = new PassengerNodes;
		p->passenger.passengerID = ID;
		p->passenger.firstName = firstName;
		p->passenger.lastName = lastName;
		p->passenger.gender = isMr;
		p->left = NULL;
		p->right = NULL;
	}
	else if (ID < p->passenger.passengerID) // nếu ID nhỏ hơn ID của nút p hiện tại thì sẽ đi sang trái
		insert_passenger(p->left, ID, firstName, lastName, isMr);
	else if (ID > p->passenger.passengerID)
		insert_passenger(p->right, ID, firstName, lastName, isMr);
}

bool isPassengerExist(PassengerNodesPTR &root, const std::string &ID)
{
	if (root == NULL)
		return false;
	else if (ID < root->passenger.passengerID)
		return isPassengerExist(root->left, ID);
	else if (ID > root->passenger.passengerID)
		return isPassengerExist(root->right, ID);
	else
		return true;
}

void inorder_to_file(PassengerNodesPTR &p, std::ofstream &file)
{
	if (p != NULL)
	{
		inorder_to_file(p->left, file);
		file << p->passenger.passengerID << "\t";
		file << p->passenger.firstName << "\t";
		file << p->passenger.lastName << "\t";
		file << p->passenger.gender << "\n";
		inorder_to_file(p->right, file);
	}
}

void save_passenger(PassengerNodesPTR &root, std::string &filename)
{
	std::ofstream f(filename);

	if (f.is_open())
	{
		inorder_to_file(root, f);
		f.close();
	}
	else
	{
		std::cout << "An error occurred while opening passenger file" << std::endl;
		return;
	}

	std::cout << "Saved to passenger file successfully!" << std::endl;
}

void load_passenger(PassengerNodesPTR &root, std::string &filename)
{
	std::ifstream f(filename);

	if (f.is_open())
	{
		std::string ID, firstName, lastName;
		bool isMr;
		while (f >> ID >> firstName >> lastName >> isMr)
		{
			insert_passenger(root, ID, firstName, lastName, isMr);
		}
		f.close();
	}
	else
	{
		std::cout << "An error occurred while opening passenger file" << std::endl;
		return;
	}

	std::cout << "Loaded from passenger file successfully!" << std::endl;
}

void book_ticket(FlightNodePTR &p, const std::string &ID, int &ticketIndex)
{
	p->flight.ticketList[ticketIndex].passengerID = ID;
	p->flight.ticketList[ticketIndex].inUse = true;
	p->flight.totalTicket++;
	show_noti("Ticket booked successfully!");
}

PassengerNodesPTR search_passenger(PassengerNodesPTR &root, const std::string &key)
{
	PassengerNodesPTR p;
	p = root;
	while (p != NULL && p->passenger.passengerID != key)
		if (key < p->passenger.passengerID)
			p = p->left;
		else
			p = p->right;
	return (p);
}