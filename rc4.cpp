#include "mainwindow.h"
#include <QString>
#include "rc4.h"


#include "debug.h"

#define THISINFO              0
#define THISERROR           1
#define THISASSERT          1


#if 0

#define      SBOX_LEN      256
unsigned char sbox[SBOX_LEN];
unsigned char kbox[SBOX_LEN];



void init_kbox(unsigned char * key,unsigned int keylen)
{
    int i;
    /*
     * 由于密钥串的长度较短，所以在初始化时是循环利用的。
     */
    for (i = 0; i < SBOX_LEN; i++)
        kbox[i] = key[i % keylen];
}


/*
 * 在初始化的过程中，密钥的主要功能是将S-box搅乱，i确保S-box的
 * 每个元素都得到处理，j保证S-box的搅乱是随机的。而不同的S-box
 * 在经过伪随机子密码生成算法的处理后可以得到不同的子密钥序列，
 * 并且，该序列是随机的：
 */
void init_sbox(void)
{
    int i, j = 0;
    unsigned char tmp;
    
    for (i = 0; i < SBOX_LEN; i++)
        sbox[i] = i;

    for (i = 0; i < SBOX_LEN; i++)
    {
        j = (j + sbox[i] + kbox[i]) % SBOX_LEN;
        tmp = sbox[i];
        sbox[i] = sbox[j];
        sbox[j] = tmp;
    }
}

void rc4_encrypt(unsigned char *source, unsigned char *target, int len)
{
    int i = 0, j = 0, t, index = 0;
    unsigned char tmp;
    
    if (source == NULL || target == NULL || len <= 0)
        return;
    
    while (index < len)
    {
        i = (i + 1) % SBOX_LEN;
        j = (j + sbox[i]) % SBOX_LEN;
        tmp = sbox[i];
        sbox[i] = sbox[j];
        sbox[j] = tmp;
        t = (sbox[i] + sbox[j]) % SBOX_LEN;
        
        target[index] = source[index] ^ sbox[t]; //异或运算
        index++;
    }
}
#endif

//--------------------------------------

QEncryptRc4::QEncryptRc4()
{

}

void QEncryptRc4::UseKey(QString key)
{
    int len = key.length();
    if(key.isEmpty()) {
        return ;
    }
    for(int i=0;i<sbox_len;i++) {
        this->kbox[i] = key.toAscii().data()[i % len];
    }
}

void QEncryptRc4::Encrypt(const QByteArray & source,QByteArray & target)
{
    int i = 0, j = 0, t, index = 0;
    unsigned char tmp;

    int len = source.size();

    target.resize(len);

    init_sbox();

    while (index < len)
    {
        i = (i + 1) % sbox_len;
        j = (j + this->sbox[i]) % sbox_len;
        tmp = this->sbox[i];
        this->sbox[i] = this->sbox[j];
        this->sbox[j] = tmp;
        t = (this->sbox[i] + this->sbox[j]) % sbox_len;

        target[index] = source[index] ^ this->sbox[t]; //异或运算
        index++;
    }
}

void QEncryptRc4::init_sbox(void)
{
    int i, j = 0;
    unsigned char tmp;

    for (i = 0; i < sbox_len; i++)
        this->sbox[i] = i;

    for (i = 0; i < sbox_len; i++)
    {
        j = (j + this->sbox[i] + this->kbox[i]) % sbox_len;
        tmp = this->sbox[i];
        this->sbox[i] = this->sbox[j];
        this->sbox[j] = tmp;
    }
}

