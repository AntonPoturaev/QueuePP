///////////////////////////////////////////////////////////////////////////////
///
/// Project          MIB3 Radio2/Connectivity
/// Copyright (c)    2017
/// Company          Luxoft
///                  All rights reserved
///
////////////////////////////////////////////////////////////////////////////////
/// @file            DataBaseProccessor_Test.cpp
/// @authors         kgmalakhov
/// @date            3/9/17
///
/// @brief           Implementation of DataBaseProccessor_Test
///
////////////////////////////////////////////////////////////////////////////////


//#include <CL/DataBaseProcessor/DataReader.hpp>
#include <thread>
#include <iostream>
#include <ctime>
#include <sys/time.h>


#include <CL/DataBaseProcessor/Interfaces.hpp>
#include <CL/vCardParser/MicrocardParser.hpp>
#include <SharedMemory/SharedMemoryDriver.hpp>
#include <Phone/Types/Types.hpp>
#include <CL/Logger.hpp>
#include <CL/Logging/Logger.hpp>
#include <Types.hpp>


#define SHMEM_NAME  "Arrakis"
#define SHMEM_SIZE   4092

using namespace Bluetooth::Organizer::DataBaseProcessor;

using namespace Bluetooth::CL;

void _InitLogger() {
  SET_LOGGING_INFORMATION_SETTINGS("BTPlatform", "bluetooth_launcher");
  Logging::Logger::SetFilter("DBProcessor::");
}

static const std::string uri_db = "333.444.555.667";//getExamplePath() + "base/phonebook.sqlite3";
static const std::string vcard_test = "./vcards/contacts2.vcf";

void printContact(Contact& contact) {
  LOG_INFORMATION(boost::format("We got next %1% %2% %3%") % contact.GetID() %
                  reinterpret_cast<wchar_t*>(contact.GetFirstName().firstName) %
                  static_cast<wchar_t*>(contact.GetLastName().lastName));

}

void printCallHistory(CallHistoryItem& contact) {
  LOG_INFORMATION(boost::format("We got next %1% %2% %3%") % contact.GetID() %
                  reinterpret_cast<char*>(contact.GetPhoneNumber().phoneNumber) %
                  static_cast<uint32_t>(contact.GetStatus()));

}

void print(uint32_t test) {
  cout << test << std::endl;
}

std::string epochTimeToStr(const std::time_t& t) {
  std::tm* ptm = std::localtime(&t);
  const int buff_size = 32;
  char buffer[buff_size];
  std::strftime(buffer, buff_size, "%F %T", ptm);
  return std::string(buffer);
}

void getAllContactById() {

  tCallbackFunctionForContact callbackFunction = printContact;
  std::vector<ContactRaw*> contacts;
//  GetAllConactById(uri_db, contacts, callbackFunction);

}

void writeToDB(std::string& vcards) {

  uint32_t start_time = clock();
  tCallbackFunctionForvCard callbackFunction = print;
  //SendVCards(uri_db, vcards, "SIM", callbackFunction);
  uint32_t stop_time = clock();

}

void writeToCallHistory() {

  SharedMemoryDriver shDriver(SHMEM_NAME, sizeof(ContactRaw));
  SharedMemoryContext<CallHistoryItemRaw> shmemContext;
  shmemContext.m_pObject = reinterpret_cast<CallHistoryItemRaw*>(shDriver.GetShmemAdress());
  CallHistoryItem contact(shmemContext);
  contact.SetID(-1);
  contact.SetContactID(-1);
  BtIf::Interface::Phone::tPhoneNumber phone = {"222-2222-4444"};
  // phone
  contact.SetPhoneNumber(phone);
  contact.SetStatus(eCallHistoryItemStatus::CALLSTACKELEMENT_DIALED);
  using std::chrono::system_clock;
  std::time_t tt = system_clock::to_time_t(system_clock::now());

  contact.SetDateTime(epochTimeToStr(tt).c_str());
  tCallbackFunctionForCallHistory callbackFunction = printCallHistory;
//    WriteCallHistory(uri_db,contact,callbackFunction);

}

int main() {
  //SharedMemoryDriver shmemHandler(SHMEM_NAME, SHMEM_SIZE);
  LOG_INFORMATION("Start proccesses");
  std::ifstream in;
  LOG_INFORMATION("Open file");
  in.open(vcard_test);
  std::string vcards = std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  uint32_t start_time = clock();
  Database::InitMultiThread();
  writeToDB(vcards);
  //std::thread fourd(writeToDB,vcards);
//  std::thread first(getAllContactById);
//  std::thread second(getAllContactById);
//  std::thread third(getAllContactById);
//  first.join();
//  second.join();
// third.join();
  //fourd.join();
  uint32_t stop_time = clock();
  LOG_INFORMATION(boost::format("The time of all processes are: %1% s.") %
                  (static_cast<float>((stop_time - start_time)) / CLOCKS_PER_SEC));
  return 1;
}

