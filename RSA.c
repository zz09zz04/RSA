#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define	Print	printf
#define	PRIME_TABLE_SIZE		100000

#define	MESSAGE	"Hello World!!!"

int PrimeCount = 0;
int PrimeTable[PRIME_TABLE_SIZE];

void SetRandomSeed (void)
{
	srand (time(NULL));
}

int GetRandomNumber (long long RandomMax)
{
  Print ("%x\n", RAND_MAX);
  if (RAND_MAX < 0x8000) {
    return (rand() * rand()) % RandomMax + 1;
  }
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

long long GetModularMultiplicativeInverse (long long e, long long r)
{
	long long d;
	for (d = 0 ; d < r ; d++) {
		if (e * d % r == 1) {
			return d;
		}
	}
	return -1;
}

int RSAGeneratePublicAndPrivateKeys (long long PrimeA, long long PrimeB, long long *N, long long *e, long long *d)
{

	*N = PrimeA * PrimeB;
	Print ("N = %lld * %lld = %lld\n", PrimeA, PrimeB, *N);

	long long r;
	r = (PrimeA - 1) * (PrimeB - 1);
	Print ("r = (%lld - 1) * (%lld - 1) = %lld \n", (PrimeA), (PrimeB), r);

	do {
		do {
			*e = GetRandomNumber (r);

		} while (GcdAlgorithm (r, *e) != 1);
		Print ("e = %lld\n", *e);

		*d = GetModularMultiplicativeInverse (*e, r);
		Print ("d = %lld\n", *d);
		Print ("%lld \n", (*e) * (*d));
	} while (*d == -1);
  
	Print ("e * d %% r = %lld\n", ((*e) * (*d)) % r);
	Print ("The Public Key is [%lld,%lld]\n", *N, *e);
	Print ("The Private Key is [%lld,%lld]\n", *N, *d);

  return 0;
}

void Encrypt (char *Message, long long **Encryption, int *Length, long long N, long long e)
{
	long long i, j;
	long long temp;

	*Encryption = malloc (strlen (Message) * sizeof (long long));

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
		Print ("[%03lld] 0x%016llx\n", i, temp);
		(*Encryption)[i] = temp;
	}
	*Length = strlen (Message);
}

void Decrypt (long long *Encryption, char **Message, int Length, long long N, long long d)
{
	long long i, j;
	long long temp;

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
		(*Message)[i] = temp;
		Print ("[%03lld] 0x%02llx ", i, temp);
		Print ("%c", (char)temp);
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
	long long N;
	long long e,d;

	//
	// Encrypt and decrypt message
	//
	long long *Encryption;
	int Length;
	char *Message;

	time_t start;
	time_t end;

  SetRandomSeed ();

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






