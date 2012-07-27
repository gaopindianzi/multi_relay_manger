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
     * ������Կ���ĳ��Ƚ϶̣������ڳ�ʼ��ʱ��ѭ�����õġ�
     */
    for (i = 0; i < SBOX_LEN; i++)
        kbox[i] = key[i % keylen];
}


/*
 * �ڳ�ʼ���Ĺ����У���Կ����Ҫ�����ǽ�S-box���ң�iȷ��S-box��
 * ÿ��Ԫ�ض��õ�����j��֤S-box�Ľ���������ġ�����ͬ��S-box
 * �ھ���α��������������㷨�Ĵ������Եõ���ͬ������Կ���У�
 * ���ң�������������ģ�
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
        
        target[index] = source[index] ^ sbox[t]; //�������
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

        target[index] = source[index] ^ this->sbox[t]; //�������
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

