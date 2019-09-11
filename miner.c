//VF CASH - Standalone Miner - August 2019
//James William Fletcher

#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "ecc.h"
#include "base58.h"

float approx_sqrt(float n)
{
    long i;
    float x2, y;
    const float threehalfs = 1.5F;
    x2 = n * 0.5F;
    y  = n;
    i  = * ( long * ) &y;
    i  = 0x5f3759df - ( i >> 1 );
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );
    y  = y * ( threehalfs - ( x2 * y * y ) );
    return n*y;
}

double toDB(const uint64_t b)
{
    return (double)(b) / 1000;
}

double mfloor(double i)
{
    if(i < 0)
        return (int)i - 1;
    else
        return (int)i;
}

struct vec3
{
    uint16_t x,y,z;
};
typedef struct vec3 vec3;

double gNa(const vec3* a, const vec3* b)
{
    const double dot = ((double)(a->x) * (double)(b->x)) + ((double)(a->y) * (double)(b->y)) + (double)((a->z) * (double)(b->z));
    const double m1 = sqrt((double)((a->x) * (double)(a->x)) + (double)((a->y) * (double)(a->y)) + (double)((a->z) * (double)(a->z)));
    const double m2 = sqrt((double)((b->x) * (double)(b->x)) + (double)((b->y) * (double)(b->y)) + (double)((b->z) * (double)(b->z)));

    if((m1 == 0 && m2 == 0) || dot == 0)
        return 1;

    return dot / (m1*m2);
}

double getMiningDifficulty()
{
    const time_t lt = time(0);
    const struct tm* tmi = gmtime(&lt);
    return (double)(tmi->tm_hour+1) * 0.01;
}

uint64_t isSubGenesisAddress(uint8_t *a)
{
    vec3 v[5];

    uint8_t *ofs = a;
    memcpy(&v[0].x, ofs, sizeof(uint16_t));
    memcpy(&v[0].y, ofs + sizeof(uint16_t), sizeof(uint16_t));
    memcpy(&v[0].z, ofs + (sizeof(uint16_t)*2), sizeof(uint16_t));

    ofs = ofs + (sizeof(uint16_t)*3);
    memcpy(&v[1].x, ofs, sizeof(uint16_t));
    memcpy(&v[1].y, ofs + sizeof(uint16_t), sizeof(uint16_t));
    memcpy(&v[1].z, ofs + (sizeof(uint16_t)*2), sizeof(uint16_t));

    ofs = ofs + (sizeof(uint16_t)*3);
    memcpy(&v[2].x, ofs, sizeof(uint16_t));
    memcpy(&v[2].y, ofs + sizeof(uint16_t), sizeof(uint16_t));
    memcpy(&v[2].z, ofs + (sizeof(uint16_t)*2), sizeof(uint16_t));

    ofs = ofs + (sizeof(uint16_t)*3);
    memcpy(&v[3].x, ofs, sizeof(uint16_t));
    memcpy(&v[3].y, ofs + sizeof(uint16_t), sizeof(uint16_t));
    memcpy(&v[3].z, ofs + (sizeof(uint16_t)*2), sizeof(uint16_t));

    ofs = ofs + (sizeof(uint16_t)*3);
    memcpy(&v[4].x, ofs, sizeof(uint16_t));
    memcpy(&v[4].y, ofs + sizeof(uint16_t), sizeof(uint16_t));
    memcpy(&v[4].z, ofs + (sizeof(uint16_t)*2), sizeof(uint16_t));

    const double a1 = gNa(&v[0], &v[3]);
    const double a2 = gNa(&v[3], &v[2]);
    const double a3 = gNa(&v[2], &v[1]);
    const double a4 = gNa(&v[1], &v[4]);

    const double min = 0.24;
    
    if(a1 < min && a2 < min && a3 < min && a4 < min)
    {
        const double at = (a1+a2+a3+a4);
        if(at <= 0)
            return 0;
        const double ra = at/4;
        const double mn = 4.166666667;
        const uint64_t rv = (uint64_t)mfloor(( 1000 + ( 10000*(1-(ra*mn)) ) )+0.5);
        
        printf("subG: %.8f - %.8f - %.8f - %.8f - %.3f VFC < %.3f\n\n", a1, a2, a3, a4, toDB(rv), ra);
        
        return rv;
    }
    
    const double soft = 0.1;
    if(a1 < min+soft && a2 < min+soft && a3 < min+soft && a4 < min+soft)
        printf("x: %.8f - %.8f - %.8f - %.8f\n", a1, a2, a3, a4);

    return 0;

}

int main()
{
    printf("Please wait, minted keys are saved to minted.txt, difficulty 0.24 ...\n");
    
	//Uncomment BOTH 2 LINES below and set the number of threads manually if needed (e.g. wanna mine & work together)
	//omp_set_dynamic(0); //Override auto thread count
	//omp_set_num_threads(16); //Set the number of threads here

    #pragma omp parallel
    for(int i=0; i < 2048; ++i) 
    {
        i=0;
        int tid = omp_get_thread_num();
        int nthreads;
        if(tid == 0)
        {
            nthreads = omp_get_num_threads();
            printf("Number of threads: %d\n", omp_get_num_threads());
        }
        
        time_t nt = time(0)+16;
        uint64_t c = 0;
        while(1)
        {
            if(time(0) > nt)
            {
                if(tid == 0)
                    printf("HASH/s: %lu\n", (c*nthreads)/16);

                if(c > 0)
                    printf("T-%i: %lu\n", omp_get_thread_num(), c/16);

                c = 0;
                nt = time(0)+16;
            }

            uint8_t priv[ECC_BYTES];
            uint8_t pub[ECC_BYTES+1];
            ecc_make_key(pub, priv);
            uint64_t r = isSubGenesisAddress(pub);
            if(r != 0)
            {
                char bpriv[256];
                memset(bpriv, 0, sizeof(bpriv));
                size_t len = 256;
                b58enc(bpriv, &len, priv, ECC_BYTES);

                const double fr = toDB(r);
                printf("Private Key: %s (%.3f)\n\n", bpriv, fr);
                
                FILE* f = fopen("minted.txt", "a");
                if(f != NULL)
                {
                    fprintf(f, "%s / %.3f\n", bpriv, fr);
                    fclose(f);
                }
            }

            c++;
        }
    }
    
    return 0;
}
