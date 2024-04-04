#pragma once
#include <iostream>
#include <string>

#include "bigint.h"


namespace OpenGG
{
	std::string randomIntStr(int digits)
	{
		std::string str = "";
		for(int i = 0; i < digits; ++i)
		{
			str += std::to_string((std::rand() % 10));
		}
		return str;
	}

	void genRSAKey(int size, std::string filePath = nullptr)
	{
		int digits = 151;
		if (size == 256)
			digits = 78;

		std::string num1 = randomIntStr(digits), num2 = randomIntStr(digits);
		bigint a(num1), b(num2);
		int i = 0;
		while (!big_isPrime(a) && !big_isPrime(b))
		{
			std::cout << "Iteration: " << i++;
			num1 = randomIntStr(digits);
			num2 = randomIntStr(digits);
			a = bigint(num1);
			b = bigint(num2);
		}

		std::cout << num1 << std::endl << num2;
	}

	float symmetricEncrypt(std::string keyFilePath, std::string dataFilePath, void* cipher);
}
