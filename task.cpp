#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cctype>
#include <map>
#include <queue>
#include <algorithm>

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
    return (hours * 60) + minutes;
}

// Метод проверки, что строка содержит только цифры
bool isDigits(const string& str) {
    return all_of(str.begin(), str.end(), ::isdigit);
}

//Класс "Стол"
class Table{
	private: 
		int ID;
	public:
		bool isBusy = false;
		int busyStart = 0;
		int busyEnd = 0;
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
			this->busyEnd = 0;
	        this->busyStart = timeStart;
	        isBusy = true;
    	}
		void setBusyEnd(int timeEnd, int hourCost) {
			    this->busyEnd = timeEnd;
			    int busyDuration = this->busyEnd - this->busyStart; // Вычисление продолжительности в минутах
			    this->busySummary += busyDuration; 
			    int hoursUsed = busySummary / 60; // Преобразование в количество часов
			    if (busySummary % 60 != 0) { // Проверка на неполные часы
			        hoursUsed++;
			    }
			    this->earnings = hoursUsed * hourCost;
			
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
		void processEvent(const Event& event, vector<string>& output) {
	        switch (event.eventID) {
	        case 1:
	            arrivalClients(event, output);
	            break;
	        case 2:
	            clientsAtClub(event, output);
	            break;
	        case 3:
	            clientsWaiting(event, output);
	            break;
	        case 4:
	            clientsLeaving(event, output);
	            break;
	        default:
	            std::cerr << "Unknown event ID! (" << event.eventID << ")" << std::endl;
	            break;
	        }
    	}
    	string formatTime(int minutes) {
	        int hours = minutes / 60;
	        int mins = minutes % 60;
	        ostringstream oss;
	        oss << setw(2) << setfill('0') << hours << ":"
	            << setw(2) << setfill('0') << mins;
	        return oss.str();
    	}
	private:
		void arrivalClients(const Event& event, vector<string>& output){
			output.push_back(formatTime(event.time) + " 1 " + event.clientName);
			if (clientTable.find(event.clientName) != clientTable.end()) {
	            generateError(event.time, "YouShallNotPass", output);
	        } else if (event.time < workStart || event.time > workEnd) {
	            generateError(event.time, "NotOpenYet", output);
	        } else {
	            clientTable[event.clientName] = -1; // Клиент в клубе, но не за столом
	        }
		}
		void clientsAtClub(const Event& event, vector<string>& output){
			output.push_back(formatTime(event.time) + " 2 " + event.clientName + " " + to_string(event.tableNumber));
			if (clientTable.find(event.clientName) == clientTable.end()) {
	            generateError(event.time, "ClientUnknown", output);
	        } 
			else {
	        	int currentTable = clientTable[event.clientName];
	            if (currentTable != -1 && tables[currentTable-1].getIsBusy()) {
	                tables[currentTable - 1].setBusyEnd(event.time, hourCost);
	            }
	            if (tables[event.tableNumber-1].getIsBusy()) {
	                generateError(event.time, "PlaceIsBusy", output);
	            } 
				else {
	                tables[event.tableNumber - 1].setBusyStart(event.time);
                	clientTable[event.clientName] = event.tableNumber;
	            }
	        }
		}
		void clientsWaiting(const Event& event, vector<string>& output) {
			output.push_back(formatTime(event.time) + " 3 " + event.clientName);
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
    				clientTable.erase(event.clientName);
	                generateEvent(event.time, 11, event.clientName, output);
	            } 
				else {
	                waitingQueue.push(event.clientName);
	            }
        	}
    	}

	    void clientsLeaving(const Event& event, vector<string>& output) {
	    	output.push_back(formatTime(event.time) + " 4 " + event.clientName);
	        if (clientTable.find(event.clientName) == clientTable.end()) {
	            generateError(event.time, "ClientUnknown", output);
	        } 
			else {
	            int tableNumber = clientTable[event.clientName];
	            if (tableNumber != -1) {
	                tables[tableNumber - 1].setBusyEnd(event.time, hourCost);
	                if (event.time >= workEnd)
					{
						output.pop_back();
	                	generateEvent(event.time,11,event.clientName,output);
					}
					else
					{
						if (!waitingQueue.empty()) {
		                    string nextClient = waitingQueue.front();
	                    	waitingQueue.pop();
	                    	tables[tableNumber - 1].setBusyStart(event.time);
	                    	clientTable[nextClient] = tableNumber;
	                    	generateEvent(event.time, 12, nextClient, output, tableNumber);
	                	}
					}
	            }
	            else{
	            	if (event.time >= workEnd)
					{
						output.pop_back();
	                	generateEvent(event.time,11,event.clientName,output);
					}
				}
	            clientTable.erase(event.clientName);
	        }
	    }
	    void generateError(int time, const string& error, vector<string>& output) {
        	generateEvent(time, 13, error, output);
    	}
    	void generateEvent(int time, int eventID, const string& clientName, vector<string>& output, int tableNumber = -1) {
	        string eventStr = formatTime(time) + " " + to_string(eventID) + " " + clientName;
	        if (tableNumber != -1) {
	            eventStr += " " + to_string(tableNumber);
	        }
	        output.push_back(eventStr);
    	}
};
//Тело программы
int main(int argc, char* argv[]){
	if (argc != 2) {
        cerr << "Error! Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    string filename = argv[1];
    ifstream file(filename.c_str());

    if (!file.is_open()) {
        cerr << "Can't open file: " << filename << endl;
        return 1;
    }

    int tablesCount, hourCost;
    string workStartStr, workEndStr;
    string line;
	
	//чтение первой строки
    if (getline(file, line)) {
        istringstream iss(line);
        if (!(iss >> tablesCount) || !isDigits(line)) {
            cerr << "Error: Incorrect input format in line: " << line << endl;
            return 0;
        }
    } else {
        cerr << "Error: Failed to read the first line" << endl;
        return 0;
    }
	// Чтение второй строки
    if (getline(file, line)) {
        istringstream iss(line);
        if (!(iss >> workStartStr >> workEndStr) || !ValidateTimeFormat(workStartStr) || !ValidateTimeFormat(workEndStr)) {
            cerr << "Error: Incorrect input format in line " << line << endl;
            return 0;
        }
    } else {
        cerr << "Error: Failed to read the second line" << endl;
        return 0;
    }

    int workStart = getTimeMinutes(workStartStr);
    int workEnd = getTimeMinutes(workEndStr);

    //чтение третьей стрки
    if (getline(file, line)) {
        istringstream iss(line);
        if (!(iss >> hourCost) || !isDigits(line)) {
            cerr << "Error: Incorrect input format in line " << line << endl;
            return 0;
        }
    } else {
        cerr << "Error: Failed to read the third line" << endl;
        return 0;
    }

    ComputerClub club(tablesCount, workStart, workEnd, hourCost);
    vector<string> output;

    output.push_back(workStartStr);

    while (getline(file,line)) {
    	istringstream iss(line);
        string timeStr, clientName;
        int eventID, tableNumber;
        
        if (!(iss >> timeStr >> eventID >> clientName)) {
            cerr << "Error: Incorrect input format in line " << line << endl;
            return 0;
        }
        if (!isDigits(to_string(eventID))){
        	cerr << "Error: Incorrect input format in line " << line << endl;
            return 0;
		}
        
        if (!ValidateTimeFormat(timeStr)) {
            cerr << "Error: Incorrect input format in line " << line << endl;
            return 0;
        }
        int time = getTimeMinutes(timeStr);
        switch (eventID) {
        case 1:
        	club.processEvent(Event(time,eventID,clientName), output);
        	break;
        case 3:
        	club.processEvent(Event(time,eventID,clientName),output);
        	break;
        case 4:
            club.processEvent(Event(time, eventID, clientName), output);
            break;
        case 2:
            if (!(iss >> tableNumber) || !isDigits(to_string(tableNumber))) {
                cerr << "Error: Incorrect input format in line " << line << endl;
                return 0;
            }
            club.processEvent(Event(time, eventID, clientName, tableNumber), output);
            break;
        default:
            std::cerr << "Error: Incorrect input format (unknown event ID) in line " << line << std::endl;
            return 0;
        }
    }
    
    while (!club.clientTable.empty()) {
	    auto it = club.clientTable.begin();
	    club.processEvent(Event(workEnd, 4, it->first), output);
	    it->second = -1;
	}
	
    output.push_back(workEndStr);

    for (const auto& line : output) {
        std::cout << line << std::endl;
    }

    for (const auto& table : club.tables) {
        std::cout << table.getID() << " " << table.earnings << " " << club.formatTime(table.busySummary) << std::endl;
    }

    return 0;
}

