#ifndef _TEA_H_
#define _TEA_H_

int GetKeyFromPhrase(void * key128, const char * password, int passlen=-1);

bool EncryptData_TEA(const void * src, int n, const char * szPass);
bool DecryptData_TEA(const void * src, int n, const char * pass);

#endif// _TEA_H_
