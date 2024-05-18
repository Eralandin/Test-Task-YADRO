#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cctype>
#include <map>
#include <queue>

using namespace std;
//Метод валидации формата времени
bool ValidateTimeFormat(const string& time){
	//Проверка длины строки
	if (time.length() !=5){
		return false;
	}
	//Проверка позиции двоеточия
	if (time[2] != ':'){
		return false;
	}
	//Проверка символов, кроме двоеточия, на то, что они - цифры
	for (int i = 0; i < time.size();i++){
		if (i!=2 && !isdigit(time[i])) return false;
	}
	
	int hours = std::stoi(time.substr(0,2));
	int minutes = std::stoi(time.substr(3,2));
	
	// Проверка допустимого диапазона часов и минут
    if (hours < 0 || hours > 23) return false;
    if (minutes < 0 || minutes > 59) return false;
    
    return true;
}
//Метод перевода времени в минуты
int getTimeMinutes(const string& time){
	int hours = std::stoi(time.substr(0, 2));
    int minutes = std::stoi(time.substr(3, 2));
    return hours * 60 + minutes;
}
//Класс "Стол"
class Table{
	private: 
		bool isBusy = false;
		int ID;
		int busyStart;
		int busyEnd;
	public:
		Table(const int id){
			this->setID(id);
		}
		int busySummary = 0;
    	int earnings = 0;
		void setID(const int& id){
			this->ID = id;
		}
		int getID() const{
			return ID;
		}
		void setBusyStart(int timeStart) {
	        this->busyStart = timeStart;
	        isBusy = true;
    	}
		void setBusyEnd(int timeEnd, int hourCost) {
	        this->busyEnd = timeEnd;
	        this->busySummary += (this->busyEnd - this->busyStart);
	        this->earnings += ((this->busySummary) / 60) * hourCost;
	        isBusy = false;
    	}
		bool getIsBusy() const {
        	return isBusy;
    	}
};
//Класс "Событие"
class Event {
public:
    int time;
    int eventID;
    string clientName;
    int tableNumber;

    Event(int t, int id, string name, int table = -1)
        : time(t), eventID(id), clientName(name), tableNumber(table) {}
};

//Класс "Компьютерный клуб"
class ComputerClub{	
	public:
		int tablesCount;
		int workStart;
		int workEnd;
		int hourCost;
		std::vector<Table> tables;
		std::queue<string> waitingQueue;
		std::map<string,int> clientTable;
		
		ComputerClub(int N, int timeStart, int timeEnd, int Cost)
        : tablesCount(N), workStart(timeStart), workEnd(timeEnd), hourCost(Cost) {
	        for (int i = 0; i < N; i++) {
	            tables.push_back(Table(i + 1));
	        }
    	}
		void processEvent(const Event& event) {
	        switch (event.eventID) {
	        case 1:
	            arrivalClients(event);
	            break;
	        case 2:
	            clientsAtClub(event);
	            break;
	        case 3:
	            clientsWaiting(event);
	            break;
	        case 4:
	            clientsLeaving(event);
	            break;
	        default:
	            std::cerr << "Unknown event ID!" << std::endl;
	            break;
	        }
    	}
	private:
		void arrivalClients(const Event& event, vector<string>& output){
			if (clientTable.find(event.clientName) != clientTable.end()) {
	            generateError(event.time, "YouShallNotPass", output);
	        } else if (event.time < workStart || event.time > workEnd) {
	            generateError(event.time, "NotOpenYet", output);
	        } else {
	            clientTable[event.clientName] = -1; // Клиент в клубе, но не за столом
	            output.push_back(formatTime(event.time) + " 1 " + event.clientName);
	        }
		}
		void clientsAtClub(const Event& event){
			if (clientTable.find(event.clientName) == clientTable.end()) {
	            generateError(event.time, "ClientUnknown", output);
	        } 
			else {
	        	int currentTable = clientTable[event.clientName];
	            if (currentTable != -1 && tables[currentTable].getIsBusy()) {
	                tables[currentTable - 1].setBusyEnd(event.time, hourCost);
	            }
	            if (tables[event.tableNumber].getIsBusy()) {
	                generateError(event.time, "PlaceIsBusy", output);
	            } 
				else {
	                tables[event.tableNumber - 1].setBusyStart(event.time);
                clientTable[event.clientName] = event.tableNumber;
                output.push_back(formatTime(event.time) + " 2 " + event.clientName + " " + to_string(event.tableNumber));
	            }
	        }
		}
		void clientsWaiting(const Event& event) {
	        if (clientTable.find(event.clientName) == clientTable.end()) {
            generateError(event.time, "ClientUnknown", output);
        	} 
			else {
	            bool hasFreeTable = false;
	            for (const auto& table : tables) {
	                if (!table.getIsBusy()) {
	                    hasFreeTable = true;
	                    break;
	                }
	            }
	            if (hasFreeTable) {
	                generateError(event.time, "ICanWaitNoLonger!", output);
	            } 
				else if (waitingQueue.size() >= tablesCount) {
	                generateEvent(event.time, 11, event.clientName, output);
	            } 
				else {
	                waitingQueue.push(event.clientName);
	                output.push_back(formatTime(event.time) + " 3 " + event.clientName);
	            }
        	}
    	}

	    void clientsLeaving(const Event& event) {
	        if (clientTable.find(event.clientName) == clientTable.end()) {
	            generateError(event.time, "ClientUnknown", output);
	        } 
			else {
	            int tableNumber = clientTable[event.clientName];
	            if (tableNumber != -1) {
	                tables[tableNumber - 1].setBusyEnd(event.time, hourCost);
	                if (!waitingQueue.empty()) {
	                    string nextClient = waitingQueue.front();
                    	waitingQueue.pop();
                    	tables[tableNumber - 1].setBusyStart(event.time);
                    	clientTable[nextClient] = tableNumber;
                    	generateEvent(event.time, 12, nextClient, tableNumber, output);
	                }
	            }
	            clientTable.erase(event.clientName);
            	output.push_back(formatTime(event.time) + " 4 " + event.clientName);
	        }
	    }
	    void generateError(int time, const string& error) {
        	generateEvent(time, 13, error, output);
    	}
    	void generateEvent(int time, int eventID, const string& clientName, vector<string>& output, int tableNumber = -1) {
	        string eventStr = formatTime(time) + " " + to_string(eventID) + " " + clientName;
	        if (tableNumber != -1) {
	            eventStr += " " + to_string(tableNumber);
	        }
	        output.push_back(eventStr);
    	}
    	string formatTime(int minutes) {
	        int hours = minutes / 60;
	        int mins = minutes % 60;
	        ostringstream oss;
	        oss << setw(2) << setfill('0') << hours << ":"
	            << setw(2) << setfill('0') << mins;
	        return oss.str();
    	}	
};
//Тело программы
int main(){
	string filename = "test_file.txt";
	std::ifstream file(filename.c_str());
	
	if (!file.is_open()){
		std::cerr << "Can't open file': " << filename << std::endl;
		return 0;
	}
	string line;

    while (getline(file, line)) {

    }

    return 0;
}

