#pragma once

#include "common.h"

class Room
{
    public:
        Room();
        Room(udsNetworkStruct network);
        ~Room();

        void join();

        std::string getName() const;
        u8 getPlayerCount() const;

        Result receiveData(void* data, size_t size, size_t* actualSize);
        Result sendData(void* data, size_t size);

    private:
        udsNetworkStruct network;
        udsBindContext bindctx;

        bool server;
        bool connected;
};

extern std::shared_ptr<Room> currentRoom;
std::vector<std::shared_ptr<Room>> getRoomList();