#pragma once

#include <iostream>
#include <string>
#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>
#include "my_fProt.h"
#include <fstream>

const int MAXPLANE = 300;

struct Plane
{
    std::string planeID;
    std::string planeType;
    int seatNum;
    int rowNum;
};

struct PlaneList
{
    int totalPlane = 0;
    Plane* nodes[MAXPLANE];
};

struct departureTime
{
    int day;
    int month;
    int year;
    int hour;
    int minute;
};
typedef struct departureTime DT;

struct Ticket
{
    std::string ticketID;
    std::string passengerID;
    bool inUse;
};

struct Flight
{
    std::string flightNumber;
    DT departureTime;
    std::string desAirport;
    int stt = 1;
    std::string planeID;
    Ticket* ticketList;
    int totalTicket = 0;
    int maxTicket;
};

struct FlightList
{
    Flight flight;
    FlightList* next;
};
typedef struct FlightList* FlightListPTR;

struct Passenger
{
    std::string passengerID;
    std::string lastName;
    std::string firstName;
    bool gender;
};

struct PassengerNodes
{
    Passenger passenger;
    PassengerNodes* left;
    PassengerNodes* right;
};
typedef struct nodeHK* PassengerNodesPTR;