#include <iostream>

using namespace std;

int main(void)
{
    int intList[250];
	int divisable[30];
	int notDivisable[250];
	int max = 255;
	int inc = (max-5);
	int divis = 0;
	int notdivis = 0;
	
	for(int i = 0; i<(inc); i++)
	{
		int temp = max - i;
		intList[i] = temp;
	}

	for(int ii = 0; ii<(inc); ii++)
	{
		int temp = intList[ii]%5;
		if(temp != 0){
			notDivisable[notdivis]=intList[ii];
			notdivis++;
		}else{
			divisable[divis]=intList[ii];
			divis++;
		}
	}

	cout << "quantity of numbers divisible by five in total prog" << endl;
	cout << divis << endl;
	cout << "the numbers divisible by 5 are " << endl;
	for(int i = 0; i < divis; i++)
	{
		cout << divisable[i] << ' ' ;
	}
	cout << endl;
	cout << "quantity of numbers not divisible by five in total prog" << endl;
	cout << notdivis << endl;
	cout << "those numbers are" << endl;
	for(int i = 0; i < notdivis; i++)
	{
		cout << notDivisable[i] << ' ' ;
	}
	return 0;
}

