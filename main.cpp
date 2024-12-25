#include <iostream>
#include <Windows.h>
#include "source/inject.h"


int main()
{
	SetConsoleTitleA("Tuica Teste");

	std::cout << "Esperando o FiveM...\n";
	while (!FindWindowA(("grcWindow"), 0));
	std::cout << "Fivem encontrado, pressione qualquer tecla para injetar\n";
	system("pause >nul");
	Injection::Inject();
	system("pause>nul");
}

