#pragma once
#include <sqlite3.h>
#include <iostream>
#include <string>

using namespace std;

/// <summary>
/// Creates the database if not existent
/// </summary>
/// <param name="c"> DB path </param>
void createDB(const char* c);

/// <summary>
/// Create algorithm, symmetric, asymmetric, file tables
/// </summary>
/// <param name="c"> DB path </param>
void createTable(const char* c);

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
void selectData(const char* c, int table);

/// <summary>
/// Insert into Algorithm table
/// </summary>
/// <param name="c"> DB path </param>
/// <param name="name"> File name </param>
void insertAlgorithm(const char* c, const char* name);

/// <summary>
/// Insert into Symmetric table
/// </summary>
/// <param name="c"> DB path </param>
/// <param name="alg_id"> ID of algorithm from algorithm table </param>
/// <param name="public_key"> Public key used </param>
/// <param name="private_key"> Private key used </param>
void insertSymKey(const char* c, int alg_id, unsigned char public_key, unsigned char private_key);

/// <summary>
/// Insert into Asymmetric table
/// </summary>
/// <param name="c"> DB path </param>
/// <param name="alg_id"> ID of algorithm from Algorithm table </param>
/// <param name="public_key"> Public key used </param>
/// <param name="private_key"> Private key used </param>
void insertAsymKey(const char* c, int alg_id, unsigned char public_key, unsigned char private_key);

/// <summary>
/// Insert into File table
/// </summary>
/// <param name="c"> DB path </param>
/// <param name="sym_id"> ID of symmetric algorithm used from Symmetric table </param>
/// <param name="asym_id"> ID of asymmetric algorithm used from Asymmetric table </param>
/// <param name="path"> File path </param>
/// <param name="encr_time"> Encryption time </param>
/// <param name="decr_time"> Decryption time </param>
void insertFile(const char* c, int sym_id, int asym_id, const char* path, double encr_time, double decr_time);

/// <summary>
/// Deletes entry with ID from Algorithm table
/// </summary>
/// <param name="c"> DB path </param>
/// <param name="id"> ID of entry </param>
void deleteAlgorithm(const char* c, int id);

/// <summary>
/// Deletes entry with ID from Symmetric table
/// </summary>
/// <param name="c"> DB path </param>
/// <param name="id"> ID of entry </param>
void deleteSymmetric(const char* c, int id);

/// <summary>
/// Deletes entry with ID from Asymmetric table
/// </summary>
/// <param name="c"> DB path </param>
/// <param name="id"> ID of entry </param>
void deleteAsymmetric(const char* c, int id);

/// <summary>
/// Deletes entry with ID from File table
/// </summary>
/// <param name="c"> DB path </param>
/// <param name="id"> ID of entry </param>
void deleteFile(const char* c, int id);