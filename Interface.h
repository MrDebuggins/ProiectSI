#pragma once
#include <iostream>
#include <string>
#include "DB.h"

using namespace Database;

void optionChoice()
{
	int option;
	cout << "Choose one of the following: " << endl;
	cout << "1. Display Algorithms" << endl;
	cout << "2. Display files" << endl;
	cout << "3. Display file content" << endl;
	cout << "4. Insert file" << endl;
	cout << "5. Delete file" << endl;
	cout << "6. Exit" << endl;
	cin >> option;

	switch (option)
	{
	case 1:
		selectData(db, option);
		break;
	case 2:
		selectData(db, option);
		break;
	case 3:
		selectData(db, option);
		break;
	case 4:
		insertFile();
		break;
	case 5:
		deleteFile();
		break;
	case 6:
		return;
		break;
	default:
		cout << "Option doesn't exist";
		optionChoice();
		break;
	}
}

void insertFile()
{
	int key_id;
	string path;
	double encr_time, decr_time;
	bool status;

	cout << "Key id: " << endl;
	cout << "Available key ids: " << endl;
	selectData(db, 2);
	cin >> key_id;
	cout << "Encrypted? (true/false)" << endl;
	cin >> status;
	if (status == true)
	{
		cout << "Encription time: " << endl;
		cin >> encr_time;
		cout << "Decription time: " << endl;
		cin >> decr_time;
	}
	else
	{
		encr_time = decr_time = 0;
	}
	insertFile(db, key_id, path, encr_time, decr_time, status);
	optionChoice();
}

void deleteFile()
{
	int file_id;

	cout << "Current files: " << endl;
	selectData(db, 1);
	cout << "File id to delete: " << endl;
	cin >> file_id;

	deleteFile(db, file_id);
	optionChoice();
}