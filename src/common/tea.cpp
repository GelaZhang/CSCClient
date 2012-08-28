
#include <string.h>
#define PASSPHRASE_CHARSET " _0123456789abcdefghijklmnopqrstuvwxyz"

#ifndef BYTE
typedef  unsigned char BYTE;
#endif

bool IsWhiteSpace(unsigned int c)
{
	return c && c <= ' ';
}

unsigned int ToLowerCase(unsigned int c)
{
	if (c >= 'A' && c <= 'Z')
		return c - 'A' + 'a';
	
	return c; // return unchanged... unlike stupid _tolower() function
}

void MemcpyXor(void * dest, const void * src, int n)
{
	for (int i = 0; i < n; i++)
		((BYTE*)dest)[i] ^= ((BYTE*)src)[i];
}

void EncipherChunk_TEA(const void * src64, void * dst64, const void *key128, int aux = 0)
{
	const unsigned long * src = (const unsigned long *)src64;
	unsigned long * dst = (unsigned long *)dst64;
	const unsigned long * key = (const unsigned long *)key128;
	register unsigned long y=src[0], z=src[1], sum=0, delta=0x9E3779B9, n=32;
	y ^= aux;
	
	while(n-- > 0)
	{
		y += (z << 4 ^ z >> 5) + z ^ sum + key[sum&3];
		sum += delta;
		z += (y << 4 ^ y >> 5) + y ^ sum + key[sum>>11 & 3];
	}
	dst[0]=y;
	dst[1]=z;
}

void DecipherChunk_TEA(const void * src64, void * dst64, const void *key128, int aux = 0)
{
	const unsigned long * src = (const unsigned long *)src64;
	unsigned long * dst = (unsigned long *)dst64;
	const unsigned long * key = (const unsigned long *)key128;
	
	register unsigned long y=src[0], z=src[1], sum=0xC6EF3720, delta=0x9E3779B9, n=32;
	
	// sum = delta<<5, in general sum = delta * n
	
	while(n-- > 0)
	{
		z -= (y << 4 ^ y >> 5) + y ^ sum + key[sum>>11 & 3];
		sum -= delta;
		y -= (z << 4 ^ z >> 5) + z ^ sum + key[sum&3];
	}
	
	y ^= aux;
	
	dst[0]=y;
	dst[1]=z;
}

int GetKeyFromPhrase(void * key128, const char * pass, int pLen)
{
	if (pLen <= 0) // get the length of the pass phrase if we haven't got it already
		pLen = strlen(pass);
	
	const char * a_set = PASSPHRASE_CHARSET;
	
	memset(key128, 0, 16); // initialize the key to zero
	int a_validCharacters = 0; // counts characters that actually contribute to the key
	
	// normalize passphrase
	for (int i = 0; i < pLen; i++)
	{
		int c = ToLowerCase(pass[i]);
		
		if (IsWhiteSpace(c)) // collapse whitespace to single space
		{
			c = ' ';
			while (i+1 < pLen && IsWhiteSpace(pass[i+1])) // skip extras
				i++;
		}
		
		// now check to see if it's a valid pass character
		//char * p = strchr(a_set, c);
		
		char * p = 0;
		// quick and dirty strchr equiv caus we're not linking with the c run time
		for(int j=0; j<(int)strlen(a_set); j++)
		{
			if(c == a_set[j])
			{
				p = (char*)&a_set[j];
				break;
			}
		}
		
		if (p) // let's hash it into the key then
		{
			BYTE& a_b = ((BYTE*)key128)[a_validCharacters & 0xF];
			a_b = (a_b << 1) ^ (a_b >> 1) ^ (BYTE)(p - a_set);
			a_validCharacters++;
		}
	}
	
	return a_validCharacters;
}

bool EncryptData_TEA(const void * src, int n, const char * szPass)
{
	void * dst = (void*)src;
	int i = 0;

	if (!szPass || !*szPass) // no change for empty string
		return false;
	
	BYTE key[16]; // 128 bit key
	
	if (!GetKeyFromPhrase(key, szPass,-1))
		return false;
	
	for (i = 0; i < (n >> 3); i++)
	{
		// we pass chunk number as aux value to XOR with data (to avoid revealing repetition)
		EncipherChunk_TEA(((const BYTE*)src) + (i << 3), ((BYTE*)dst) + (i << 3), key, i);
	}
	
	if (n & 0x7) // have we got left over bytes? we must pad it out to 8 bytes
	{
		BYTE a_buf[8] = {0,0,0,0,0,0,0,0};
		EncipherChunk_TEA(a_buf, a_buf, key); // is this dangerous?
		MemcpyXor(((BYTE*)dst) + (i << 3), a_buf, n & 0x7);
	}
	
	return true;
}

bool DecryptData_TEA(const void * src, int n, const char * szPass)
{
	void * dst = (void*)src;
	int i = 0;

	if (!szPass || !*szPass) // no change for empty string
		return false;
	
	BYTE key[16]; // 128 bit key
	
	if (!GetKeyFromPhrase(key, szPass,-1))
		return false;
	
	for (i = 0; i < (n >> 3); i++)
	{
		// we pass chunk number as aux value to XOR with data (to avoid revealing repetition)
		DecipherChunk_TEA(((const BYTE*)src) + (i << 3), ((BYTE*)dst) + (i << 3), key, i);
	}
	
	if (n & 0x7) // have we got left over bytes?
	{
		BYTE a_buf[8] = {0,0,0,0,0,0,0,0};
		EncipherChunk_TEA(a_buf, a_buf, key); // is this dangerous?
		MemcpyXor(((BYTE*)dst) + (i << 3), a_buf, n & 0x7);
	}
	
	return true;
}

