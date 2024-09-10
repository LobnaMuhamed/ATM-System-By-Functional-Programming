#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>
#include <iomanip>

using namespace std;

namespace MyLib {

	const string ClientFileName = "Clients.txt";
	void ShowATM_MainMenuScreen();
	void ShowQuickWithdrawScreen();
	void Login();

	enum enATM_MainMenuOptions {
		eQuickWithdraw = 1,
		eNormalWithdraw = 2,
		eDeposit = 3,
		eCheckBalance = 4,
		eLogout = 5,
	};
	struct stClient {
		string AccountNumber;
		string PinCode;
		string Name;
		string Phone;
		double AccountBalance;
		bool MarkForDelete = false;
	};
	stClient CurrentClient;
	vector <string> SplitString(string String, string Separator = "#//#")
	{
		vector <string> vString;
		short pos = 0;
		string word;
		while ((pos = String.find(Separator)) != std::string::npos)
		{
			word = String.substr(0, pos);
			if (word != "")
				vString.push_back(word);
			String.erase(0, pos + Separator.length());
		}
		if (String != "")
			vString.push_back(String);
		return vString;
	}

	stClient ConvertClientLineToRecord(string Line)
	{
		vector <string> vClient;
		stClient ClientData;
		vClient = SplitString(Line);
		ClientData.AccountNumber = vClient.at(0);
		ClientData.PinCode = vClient.at(1);
		ClientData.Name = vClient.at(2);
		ClientData.Phone = vClient.at(3);
		ClientData.AccountBalance = stod(vClient.at(4));
		return ClientData;
	}
	string ConvertClientRecordToLine(stClient ClientData, string Separator = "#//#")
	{
		string Record = "";
		Record += ClientData.AccountNumber + Separator;
		Record += ClientData.PinCode + Separator;
		Record += ClientData.Name + Separator;
		Record += ClientData.Phone + Separator;
		Record += to_string(ClientData.AccountBalance);
		return Record;
	}
	vector <stClient> LoadClientsDataFromFile(string FileName)
	{
		fstream MyFile;
		vector <stClient> vClients;
		MyFile.open(FileName, ios::in);
		if (MyFile.is_open())
		{
			string Line;
			stClient Client;
			while (getline(MyFile, Line))
			{
				Client = ConvertClientLineToRecord(Line);
				vClients.push_back(Client);
			}
			MyFile.close();
		}
		return vClients;
	}
	bool FindClientByAccountNumberAndPinCode(string AccountNumber, string PinCode, stClient& Client)
	{
		vector <stClient> vClients;
		vClients = LoadClientsDataFromFile(ClientFileName);
		for (stClient& C : vClients)
		{
			if (C.AccountNumber == AccountNumber && C.PinCode == PinCode)
			{
				Client = C;
				return true;
			}
		}
		return false;
	}
	bool LoadClientData(string AccountNumber, string PinCode)
	{
		if (FindClientByAccountNumberAndPinCode(AccountNumber, PinCode, CurrentClient))
			return true;
		else
			return false;
	}

	vector <stClient> SaveClientsToFile(string FileName, vector <stClient> vClients)
	{
		fstream MyFile;
		string Line;
		MyFile.open(FileName, ios::out);
		if (MyFile.is_open())
		{
			for (stClient& C : vClients)
			{
				if (C.MarkForDelete == false)
				{
					Line = ConvertClientRecordToLine(C);
					MyFile << Line << endl;
				}
			}
			MyFile.close();
		}
		return vClients;
	}
	bool DepositBalanceToClientByAccountNumber(string AccountNumber, double Amount, vector <stClient>& vClients)
	{
		char Answer = 'n';
		cout << "\nAre you sure you want perform this transaction? y/n ? ";
		cin >> Answer;
		if (toupper(Answer) == 'Y')
		{
			for (stClient& C : vClients)
			{
				if (C.AccountNumber == AccountNumber)
				{
					C.AccountBalance += Amount;
					SaveClientsToFile(ClientFileName, vClients);
					cout << "Done Successfully. New balance is: " << C.AccountBalance;
					return true;
				}
			}
		}
		return false;
	}
	short getQuickWithDrawAmount(short QuickWithDrawOption)
	{
		switch (QuickWithDrawOption)
		{
		case 1:
			return 20;
		case 2:
			return 50;
		case 3:
			return 100;
		case 4:
			return 200;
		case 5:
			return 400;
		case 6:
			return 600;
		case 7:
			return 800;
		case 8:
			return 1000;
		default:
			return 0;
		}
	}
	void PerformQuickWithdrawOption(short QuickWithDrawOption)
	{
		if (QuickWithDrawOption == 9)
			return;
		short WithDrawBalance = getQuickWithDrawAmount(QuickWithDrawOption);
		if (WithDrawBalance > CurrentClient.AccountBalance)
		{
			cout << "\nThe Amount Exceeds Your Balance, Make Another Choice.\n";
			cout << "Press Any Key To Continue...";
			system("pause>0");
			ShowQuickWithdrawScreen();
		}
		vector <stClient> vClients = LoadClientsDataFromFile(ClientFileName);
		DepositBalanceToClientByAccountNumber(CurrentClient.AccountNumber,
			WithDrawBalance * -1, vClients);
		CurrentClient.AccountBalance -= WithDrawBalance;
	}
	double ReadDepositAmount()
	{
		double Amount;
		do {
			cout << "\nEnter A Positive Deposit Amount? ";
			cin >> Amount;
		} while (Amount <= 0);
		return Amount;
	}

	void PerfromDepositOption()
	{
		double DepositAmount = ReadDepositAmount();

		vector <stClient> vClients = LoadClientsDataFromFile(ClientFileName);
		DepositBalanceToClientByAccountNumber(CurrentClient.AccountNumber, DepositAmount, vClients);
		CurrentClient.AccountBalance += DepositAmount;
	}

	int ReadWithDrawAmount()
	{
		int Amount;
		do {
			cout << "\nEnter an amount muliple of 5's ? ";
			cin >> Amount;
		} while (Amount % 5 != 0);
		return Amount;
	}
	void PerformNormalWithdrawOption()
	{
		int WithDrawBalance = ReadWithDrawAmount();
		if (WithDrawBalance > CurrentClient.AccountBalance)
		{
			cout << "\nThe Amount Exceeds Your Balance, Make Another Choice.\n";
			cout << "Press Any Key To Continue...";
			system("pause>0");
			ShowQuickWithdrawScreen();
			return;
		}
		vector <stClient> vClients = LoadClientsDataFromFile(ClientFileName);
		DepositBalanceToClientByAccountNumber(CurrentClient.AccountNumber, WithDrawBalance * -1, vClients);
		CurrentClient.AccountBalance -= WithDrawBalance;
	}
	short ReadQuickWithDrawOption()
	{
		short Choice = 0;
		do {
			cout << "Choose What to do From [1] to [9] ? ";
			cin >> Choice;
		} while (Choice < 1 || Choice > 9);
		return Choice;
	}
	void ShowQuickWithdrawScreen()
	{
		system("cls");
		cout << "=======================================\n";
		cout << "\t\tQuick Withdraw Screen";
		cout << "\n=======================================\n";
		cout << "\t[1] 20" << setw(15) << "[2] 50\n";
		cout << "\t[3] 100" << setw(15) << "[4] 200\n";
		cout << "\t[5] 400" << setw(15) << "[6] 600\n";
		cout << "\t[7] 800" << setw(15) << "[8] 1000\n";
		cout << "\t[9] Exit\n";
		cout << "\n=======================================\n";
		cout << "Your Balance is " << CurrentClient.AccountBalance << endl;
		PerformQuickWithdrawOption(ReadQuickWithDrawOption());

	}
	void ShowNormalWithdrawScreen()
	{
		system("cls");
		short Amount = 0;
		cout << "=======================================\n";
		cout << "\t\tNormal Withdraw Screen";
		cout << "\n=======================================\n";
	}
	void ShowDepositScreen()
	{
		system("cls");
		cout << "=======================================\n";
		cout << "\t\tDeposit Screen";
		cout << "\n=======================================\n";
		PerfromDepositOption();
	}
	void ShowCheckBalanceScreen()
	{
		system("cls");
		cout << "=======================================\n";
		cout << "\t\tCheck Balance Screen";
		cout << "\n=======================================\n";
		cout << "Your Balance is " << CurrentClient.AccountBalance << "\n";
	}
	void GoBackToMainMenu()
	{
		cout << "\n\nPress any key to go back to Main Menu....";
		system("pause>0");
		ShowATM_MainMenuScreen();
	}
	void PerformMainMenuOption(enATM_MainMenuOptions MainMenuOption)
	{
		switch (MainMenuOption)
		{
		case enATM_MainMenuOptions::eQuickWithdraw:
			system("cls");
			ShowQuickWithdrawScreen();
			GoBackToMainMenu();
			break;
		case enATM_MainMenuOptions::eNormalWithdraw:
			system("cls");
			ShowNormalWithdrawScreen();
			GoBackToMainMenu();
			break;
		case enATM_MainMenuOptions::eDeposit:
			system("cls");
			ShowDepositScreen();
			GoBackToMainMenu();
			break;
		case enATM_MainMenuOptions::eCheckBalance:
			system("cls");
			ShowCheckBalanceScreen();
			GoBackToMainMenu();
			break;
		case enATM_MainMenuOptions::eLogout:
			system("cls");
			Login();
			break;

		}
	}
	short ReadMainMenuOption()
	{
		short Num;
		cout << "Choose what do you want to do? [1 to 5]? ";
		cin >> Num;
		return Num;
	}
	void ShowATM_MainMenuScreen()
	{
		system("cls");
		cout << "=======================================\n";
		cout << "\t\tATM Main Menue Screen";
		cout << "\n=======================================\n";
		cout << "\t[1] Quick Withdraw.\n";
		cout << "\t[2] Normal Withdraw.\n";
		cout << "\t[3] Deposit.\n";
		cout << "\t[4] Check Balance.\n";
		cout << "\t[5] Logout.\n";
		cout << "=======================================\n";
		PerformMainMenuOption((enATM_MainMenuOptions)ReadMainMenuOption());
	}
	void Login()
	{
		bool LoginFailed = false;
		string AccountNumber, PinCode;
		do {
			system("cls");
			cout << "\n-------------------------------------------\n";
			cout << "\t\tLogin Screen";
			cout << "\n-------------------------------------------\n";
			if (LoginFailed)
				cout << "Invalid Account Number/PinCode!\n";
			cout << "Enter Account Number? ";
			getline(cin >> ws, AccountNumber);
			cout << "Enter Pin? ";
			getline(cin >> ws, PinCode);
			LoginFailed = !LoadClientData(AccountNumber, PinCode);

		} while (LoginFailed);
		ShowATM_MainMenuScreen();
	}
}