#pragma once
#include <sqlite3.h>
#include <iostream>
#include <string>

using namespace std;

namespace Database {
	/// <summary>
	/// Creates the database if not existent
	/// </summary>
	/// <param name="c"> DB path </param>
	void createDB(const char* c)
	{
		sqlite3* DB;
		int exit = 0;

		exit = sqlite3_open(c, &DB);

		sqlite3_close(DB);
	}

	/// <summary>
	/// Create algorithm, symmetric, asymmetric, file tables
	/// </summary>
	/// <param name="c"> DB path </param>
	void createTable(const char* c)
	{
		sqlite3* DB;

		string alg_table = "CREATE TABLE IF NOT EXISTS ALGORITHM("
			"ID INTEGER PRIMARY KEY AUTOINCREMENT, "
			"ALGORITHM_NAME VARCHAR(255));";

		string key_table = "CREATE TABLE IF NOT EXISTS KEY("
			"ID INTEGER PRIMARY KEY AUTOINCREMENT, "
			"ALGORITHM_ID INTEGER, "
			"KEY_PATH VARCHAR(255),"
			"FOREIGN KEY(ALGORITHM_ID) REFERENCES ALGORITHM(ID));";

		string file_table = "CREATE TABLE IF NOT EXISTS FILE("
			"ID INTEGER PRIMARY KEY AUTOINCREMENT, "
			"KEY_ID INTEGER, "
			"FILE_PATH VARCHAR(255), "
			"ENCR_TIME DOUBLE, "
			"DECR_TIME DOUBLE, "
			"STATUS BOOLEAN,"
			"FOREIGN KEY(KEY_ID) REFERENCES KEY(ID));";

		try {
			int exit = 0;
			exit = sqlite3_open(c, &DB);

			if (exit)
			{
				cerr << "Can't open DB" << endl;
				return;
			}
			else
			{
				cout << "DB opened successfully" << endl;
			}

			char* messageError;
			exit = sqlite3_exec(DB, alg_table.c_str(), NULL, 0, &messageError);

			if (exit != SQLITE_OK)
			{
				cerr << "Error Create Table Algorithm" << endl;
				sqlite3_free(messageError);
			}
			else
			{
				cout << "Table created successfully" << endl;
			}

			if (exit != SQLITE_OK)
			{
				cerr << "Error Create Table Asymmetric" << endl;
				sqlite3_free(messageError);
			}
			else
			{
				cout << "Table created successfully" << endl;
			}

			exit = sqlite3_exec(DB, key_table.c_str(), NULL, 0, &messageError);

			if (exit != SQLITE_OK)
			{
				cerr << "Error Create Table Key" << endl;
				sqlite3_free(messageError);
			}
			else
			{
				cout << "Table created successfully" << endl;
			}

			exit = sqlite3_exec(DB, file_table.c_str(), NULL, 0, &messageError);

			if (exit != SQLITE_OK)
			{
				cerr << "Error Create Table File" << endl;
				sqlite3_free(messageError);
			}
			else
			{
				cout << "Table created successfully" << endl;
			}

			sqlite3_close(DB);
		}
		catch (const exception& e)
		{
			cerr << e.what();
		}
	}

	/// <summary>
	/// Callback to print data from select query
	/// </summary>
	/// <param name="data"></param>
	/// <param name="argc"></param>
	/// <param name="argv"></param>
	/// <param name="azColName"></param>
	/// <returns></returns>
	static int callback(void* data, int argc, char** argv, char** azColName)
	{
		for (int i = 0; i < argc; i++)
			printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");

		cout << endl;
		return 0;
	}

	/// <summary>
	/// Prints out data
	/// </summary>
	/// <param name="c">DB path</param>
	/// <param name="table">
	/// 1 - Algorithm table
	///	2 - Symmetric table
	///	3 - Asymmetric table
	///	4 - File table
	/// </param>
	void selectData(const char* c, int table)
	{
		sqlite3* DB;
		char* messageError;
		const char* data = "Callback function called";

		int exit = sqlite3_open(c, &DB);

		if (exit)
		{
			cerr << "Can't open DB";
			return;
		}
		string sql;

		switch (table)
		{
		case 1:
			sql = "SELECT * FROM ALGORITHM";
			break;
		case 2:
			sql = "SELECT * FROM KEY";
			break;
		case 3:
			sql = "SELECT * FROM FILE";
			break;
		}

		exit = sqlite3_exec(DB, sql.c_str(), callback, (void*)data, &messageError);

		if (exit != SQLITE_OK)
		{
			cerr << "Error selecting data" << endl;
			sqlite3_free(messageError);
		}

		sqlite3_close(DB);
	}

	/// <summary>
	/// Insert into Algorithm table
	/// </summary>
	/// <param name="c"> DB path </param>
	/// <param name="name"> File name </param>
	void insertAlgorithm(const char* c, string name)
	{
		sqlite3* DB;
		char* messageError;

		int exit = sqlite3_open(c, &DB);

		if (exit)
		{
			cerr << "Can't open DB" << endl;
			return;
		}

		string sql = "INSERT INTO ALGORITHM(ALGORITHM_NAME) VALUES('" + name + "');";

		exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
		if (exit != SQLITE_OK)
		{
			cerr << "Error insert Algorithm" << endl;
			sqlite3_free(messageError);
		}
		else
			cout << "Algorithm inserted successfully" << endl;

		sqlite3_close(DB);
	}


	/// <summary>
	/// Insert into Symmetric table
	/// </summary>
	/// <param name="c"> DB path </param>
	/// <param name="alg_id"> ID of algorithm from algorithm table </param>
	/// <param name="key_path"> Path to the used key </param>>
	void insertKey(const char* c, int alg_id, string key_path)
	{
		sqlite3* DB;
		char* messageError;

		int exit = sqlite3_open(c, &DB);

		if (exit)
		{
			cerr << "Can't open DB" << endl;
			return;
		}

		string sql = "INSERT INTO KEY(ALGORITHM_ID, KEY_PATH) VALUES(" + to_string(alg_id) + ", \"" + key_path + "\");";

		exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
		if (exit != SQLITE_OK)
		{
			cerr << "Error insert Key" << endl;
			sqlite3_free(messageError);
		}
		else
			cout << "Key inserted successfully" << endl;

		sqlite3_close(DB);
	}

	/// <summary>
	/// Insert into File table
	/// </summary>
	/// <param name="c"> DB path </param>
	/// <param name="key_id"> ID of key used from Key table </param>
	/// <param name="path"> File path </param>
	/// <param name="encr_time"> Encryption time </param>
	/// <param name="decr_time"> Decryption time </param>
	void insertFile(const char* c, int key_id, string path, double encr_time, double decr_time, bool status)
	{
		sqlite3* DB;
		char* messageError;

		int exit = sqlite3_open(c, &DB);

		if (exit)
		{
			cerr << "Can't open DB" << endl;
			return;
		}

		string sql = "INSERT INTO FILE(KEY_ID, FILE_PATH, ENCR_TIME, DECR_TIME, STATUS) VALUES(" + to_string(key_id) + ", \"" + path + "\", "
			+ to_string(encr_time) + ", " + to_string(decr_time) + ", " + to_string(status) + ");";

		exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
		if (exit != SQLITE_OK)
		{
			cerr << "Error insert File" << endl;
			sqlite3_free(messageError);
		}
		else
			cout << "File inserted successfully" << endl;

		sqlite3_close(DB);
	}

	/// <summary>
	/// Deletes entry with ID from Algorithm table
	/// </summary>
	/// <param name="c"> DB path </param>
	/// <param name="id"> ID of entry </param>
	void deleteAlgorithm(const char* c, int id)
	{
		sqlite3* DB;
		char* messageError;
		int exit = sqlite3_open(c, &DB);

		if (exit)
		{
			cerr << "Can't open DB";
			return;
		}

		string sql = "DELETE FROM ALGORITHM where ID=" + to_string(id) + ";";

		exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
		if (exit != SQLITE_OK)
		{
			cerr << "Error delete Algorithm" << endl;
			sqlite3_free(messageError);
		}
		else
		{
			cout << "Algorithm deleted successfully" << endl;
		}

		sqlite3_close(DB);
	}

	/// <summary>
	/// Deletes entry with ID from Key table
	/// </summary>
	/// <param name="c"> DB path </param>
	/// <param name="id"> ID of entry </param>
	void deleteKey(const char* c, int id)
	{
		sqlite3* DB;
		char* messageError;
		int exit = sqlite3_open(c, &DB);

		if (exit)
		{
			cerr << "Can't open DB";
			return;
		}

		string sql = "DELETE FROM KEY where ID=" + to_string(id) + ";";

		exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
		if (exit != SQLITE_OK)
		{
			cerr << "Error delete Key" << endl;
			sqlite3_free(messageError);
		}
		else
		{
			cout << "Key deleted successfully" << endl;
		}

		sqlite3_close(DB);
	}
	/// <summary>
	/// Deletes entry with ID from File table
	/// </summary>
	/// <param name="c"> DB path </param>
	/// <param name="id"> ID of entry </param>
	void deleteFile(const char* c, int id)
	{
		sqlite3* DB;
		char* messageError;
		int exit = sqlite3_open(c, &DB);

		if (exit)
		{
			cerr << "Can't open DB";
			return;
		}

		string sql = "DELETE FROM FILE where ID=" + to_string(id) + ";";

		exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
		if (exit != SQLITE_OK)
		{
			cerr << "Error delete File" << endl;
			sqlite3_free(messageError);
		}
		else
		{
			cout << "File deleted successfully" << endl;
		}

		sqlite3_close(DB);
	}
}