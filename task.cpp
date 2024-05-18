#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>

using namespace std;

int main(){
	string filename = "test_file.txt";
	std::ifstream file(filename);
	
	if (!file.is_open()){
		std::cerr << "Не удалось открыть файл: " << filename << std::endl;
		return 0;
	}
	string line;
	
	while (getline(file,line)){
		
	}
}

class Table{
	private: 
		boolean isBusy = false;
		int ID;
		int busyStart;
		int busyEnd;
	public:
		Table(const int id) : ID(id);
		int busySummary;
		void setID(const int& id){
			this->ID = id;
		}
		void getID() const{
			return ID;
		}
		void setBusyStart(string timeStart){
			if (ValidateTimeFormat(timeStart)){
				this->busyStart = getTimeMinutes(timeStart);
			}
			else{
				std::cerr << "Ошибка формата времени! " << timeStart << std::endl;
			}
		}
		void setBusyEnd(string timeEnd){
			if (ValidateTimeFormat(timeEnd)){
				this->busyEnd = getTimeMinutes(timeEnd);
				this->busySummary += (this->busyEnd - this->busyStart);
			}
		}
};

class ComputerClub{	
	public:
		int tablesCount;
		int workStart;
		int workEnd;
		int hourCost;
		Vector<Table> tables;
		ComputerClub(int N, string timeStart, string timeEnd, int Cost){
			if (ValidateTimeFormat(timeStart) && ValidateTimeFormat (timeEnd)){
				
			}
			tables = new Vector<Table>();
			for (int i = 0; i<N;i++){
				tables.add(Table table = new Table(i));
			}
		}
};

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
	
	int hours = stoi(time.substr(0,2));
	int minutes = stoi(time.substr(3,2));
	
	// Проверка допустимого диапазона часов и минут
    if (hours < 0 || hours > 23) return false;
    if (minutes < 0 || minutes > 59) return false;
    
    return true;
}

int getTimeMinutes(const string& time){
	int hours = std::stoi(time.substr(0, 2));
    int minutes = std::stoi(time.substr(3, 2));
    return hours * 60 + minutes;
}
