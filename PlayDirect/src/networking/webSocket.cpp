#include "networking/webSocket.hpp"

webSocket::webSocket(int sessionID) {
    if (sessionID == 0) 
        return;
    readyState = ReadyStates::CLOSED;
    if (ConnectSocket(sessionID)) {
        std::cout << "Websocket is connected to backend API. Ready to stream data" << std::endl;
    }
}

// Initalize up the default websocket (It's ID is 500)
webSocket::webSocket() {
    webSocket(500);
}

webSocket::~webSocket() {
    webSocket::DiscounnectSocket();
}

bool webSocket::ConnectSocket(int sessionID) {
    // First attempt to retrieve a current session associated with that socket
    readyState = ReadyStates::CONNECTING;

    winsock = new Winsock(url, sessionID);
    readyState = ReadyStates::OPEN;

    return true;
}

void webSocket::DiscounnectSocket() {
    // Simply leave the session
    readyState = ReadyStates::CLOSING;
    winsock->DisconnectSocket();
    readyState = ReadyStates::CLOSED;
    
    return;
}

void webSocket::onSendMessage(char data[]) {
    int dataSize = strlen(data);
    webSocket::SendMessages(data, dataSize, 0x01);
}

char* webSocket::onRetrieveMessage() {
    return webSocket::RecieveMessages();
}

void webSocket::SendMessages(char data[], int dataSize, int dataType) {
    if (readyState != ReadyStates::OPEN) {
        std::cout << "Socket is not open to send back data, returning" << std::endl;
        return;
    }
    readyState = ReadyStates::BUSY;
    // Start to send off data
    switch (winsock->SendData(data,dataSize, dataType))
    {
    case 0x00:
        std::cout << "Sent off Data" << std::endl;
        readyState = ReadyStates::OPEN;
        break;
    case 0x01:
        std::cout << "Error Sending off Data" << std::endl;
        readyState = ReadyStates::OPEN;
        break;
    default:
        std::cout << "Unknown Error Occured" << std::endl;
        readyState = ReadyStates::OPEN;
        break;
    }

    return;
}

char* webSocket::RecieveMessages() {
    if (readyState != ReadyStates::OPEN) {
        std::cout << "Socket is not open to recieve data, returning" << std::endl;
        return nullptr;
    }
    readyState = ReadyStates::BUSY;

    //Time to start to retrieve data
    char* dataBuf = winsock->RecieveData();
    
    // For now just display the data out on the console
    std::cout << dataBuf << std::endl;
    readyState = ReadyStates::OPEN;

    return dataBuf;
}