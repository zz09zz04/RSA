#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define	Print	printf
#define	PRIME_TABLE_SIZE		80000

#define	MESSAGE	"Hello World!!!"

int PrimeCount = 0;
int PrimeTable[PRIME_TABLE_SIZE];

int GetRandomNumber (int RandomMax)
{
	srand (time(NULL));

	return rand() % RandomMax + 1;
}

int GcdAlgorithm (int Dividend ,int Divisor)
{
	int Remainder;
	if (Divisor == 0) {
		return -1;
	}

	Remainder = Dividend % Divisor;

	if (Remainder == 0) {
		return Divisor;
	}
	return GcdAlgorithm (Divisor, Remainder);
}

int isPrime (int Number)
{
	int i;
	for (i = 2 ; i * i <= Number ; i++) {
		if (Number % i == 0) {
			return 0;
		}
	}
	return 1;
}

void GeneratePrimesTable (void)
{
	int i;

	Print ("Test Prime algorithm:\n");
	for (i = 1 ; i < PRIME_TABLE_SIZE ; i++) {
		if (isPrime (i)) {
			if (PrimeCount < PRIME_TABLE_SIZE) {
				PrimeTable[PrimeCount] = i;
				PrimeCount++;
			}
			Print ("%d %s prime!!!\n", i, isPrime (i)?"is":"isn't");
		}
//		Print ("%d %s prime!!!\n", i, isPrime (i)?"is":"isn't");
	}
	Print ("Print Primes Table:\n");
	for (i = 0 ; i < PrimeCount ; i++) {
		Print ("[%d] %d\n", i, PrimeTable[i]);
	}
}

void GetTwoPrimes (int *PrimeA, int *PrimeB)
{
	int A,B;

	do {
		A = GetRandomNumber (PrimeCount - 1);
	} while (A < 8);
	do {
		B = GetRandomNumber (PrimeCount - 1);
	} while (A == B || B < 8);

	*PrimeA = PrimeTable[A];
	*PrimeB = PrimeTable[B];
}

long GetInverseModular (long e, long r)
{
	long d;
	for (d = 0 ; d < r ; d++) {
		if (e * d % r == 1) {
			return d;
		}
	}
	return -1;
}

int RSAGeneratePublicAndPrivateKeys (long PrimeA, long PrimeB, long *N, long *e, long *d)
{

	*N = PrimeA * PrimeB;
	Print ("N = %ld * %ld = %ld\n", PrimeA, PrimeB, *N);

	long r;
	r = (PrimeA - 1) * (PrimeB - 1);
	Print ("r = (%ld - 1) * (%ld - 1) = %ld \n", (PrimeA), (PrimeB), r);

//	long e;
//	long d;
	do {
		do {
			*e = GetRandomNumber (r);

		} while (GcdAlgorithm (r, *e) != 1);
//	*e = 7;
		Print ("e = %d\n", *e);

		*d = GetInverseModular (*e, r);
		Print ("d = %ld\n", *d);
		Print ("%ld \n", (*e) * (*d));
	} while (*d == -1);
	Print ("e * d % r = %d\n", ((*e) * (*d)) % r);
	Print ("The Public Key is [%ld,%ld]\n", *N, *e);
	Print ("The Private Key is [%ld,%ld]\n", *N, *d);
}

void Encrypt (char *Message, long **Encryption, int *Length, long N, long e)
{
	long i, j;
	long temp;

	*Encryption = malloc (strlen (Message) * sizeof (long));

//	Print ("Encryption = %p\n", *Encryption);

	Print ("The Public Key is [%lld , %lld]\n", N, e);
	Print ("Encrypt the message: %s\n", Message);
	for (i = 0 ; i < strlen (Message) ; i++) {
		temp = 1;
//		Print ("[%d] %d\n", i, Message[i]);
		for (j = 0 ; j < e ; j++) {
			temp = temp * Message[i] % N;
			if (temp < 0) {
				Print ("Error! long int overflow!\n");
				break;
			}
		}
//		temp = temp % N;
		Print ("[%d] %lld\n", i, temp);
		(*Encryption)[i] = temp;
	}
	*Length = strlen (Message);
}

void Decrypt (long *Encryption, char **Message, int Length, long N, long d)
{
	long i, j;
	long temp;

	*Message = malloc ((Length + 1) * sizeof (char));

	Print ("The Private Key is [%lld , %lld]\n", N, d);
//	Print ("Encryption = %p\n", Encryption);

	for (i = 0 ; i < Length ; i++) {
		temp = 1;
//		Print ("[%d] %d\n", i, Encryption[i]);
		for (j = 0 ; j < d ; j++) {
			temp = temp * Encryption[i] % N;
			if (temp < 0) {
				Print ("Error! long int overflow!\n");
				break;
			}
		}
//		temp = temp % N;
		(*Message)[i] = temp;
		Print ("[%03d] 0x%02x ", i, temp);
		Print ("%c", temp);
		Print ("\n");

	}
	(*Message)[Length] = 0;
	Print ("\n");
}


int main(void)
{
	//
	// Generate RSA Public and Private keys
	//
	int PrimeA,PrimeB;
	long N;
	long e,d;

	//
	// Encrypt and decrypt message
	//
	long *Encryption;
	int Length;
	char *Message;

	time_t start;
	time_t end;

	GeneratePrimesTable ();


	GetTwoPrimes (&PrimeA, &PrimeB);
	Print ("Prime A: %d\n", PrimeA);
	Print ("Prime B: %d\n", PrimeB);

	start = clock();
	RSAGeneratePublicAndPrivateKeys (PrimeA, PrimeB, &N, &e, &d);
	end = clock();
	Print ("RSA Keys generating time: %f sec.\n", (double)(end - start) / CLOCKS_PER_SEC);

	Print ("=========== Start Encrypting ===========\n");
	start = clock();
	Encrypt (MESSAGE, &Encryption, &Length, N, e);
	end = clock();
	Print ("Encrypting time: %f sec.\n", (double)(end - start) / CLOCKS_PER_SEC);

	Print ("=========== Start Decrypting ===========\n");
	start = clock();
	Decrypt (Encryption, &Message, Length, N, d);
	end = clock();
	Print ("Decrypting time: %f sec.\n", (double)(end - start) / CLOCKS_PER_SEC);

	Print ("Print the decrypting message: %s\n", Message);

	free (Encryption);
	free (Message);
}






