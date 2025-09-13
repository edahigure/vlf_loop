#include <sys/file.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>  // Para ssize_t y read()

/* Función segura para obtener strings con verificación de límites */
int getstrpar(char *name, char *buffer, int bufsize)
{
    char temp_buffer[1024];
    int found;
    
    if (bufsize <= 0) return 0;
    
    found = getpar(name, "s", temp_buffer);
    
    if (found) {
        strncpy(buffer, temp_buffer, bufsize - 1);
        buffer[bufsize - 1] = '\0';
        return 1;
    } else {
        buffer[0] = '\0';
        return 0;
    }
}

char **char_matrix(long nrl, long nrh, long ncl, long nch)
{
    long i, nrow = nrh - nrl + 1, ncol = nch - ncl + 1;
    char **m;
    int NR_END = 1;
    
    m = (char **)malloc((size_t)((nrow + NR_END) * sizeof(char*)));
    if (!m) {
        fprintf(stderr, "allocation failure 1 in matrix()\n");
        exit(EXIT_FAILURE);
    }
    m += NR_END;
    m -= nrl;

    m[nrl] = (char *)malloc((size_t)((nrow * ncol + NR_END) * sizeof(char)));
    if (!m[nrl]) {
        fprintf(stderr, "allocation failure 2 in matrix()\n");
        exit(EXIT_FAILURE);
    }
    m[nrl] += NR_END;
    m[nrl] -= ncl;

    for(i = nrl + 1; i <= nrh; i++) m[i] = m[i-1] + ncol;

    return m;
}

int main(int ac, char **av)
{
    int i, npts, year, jday, hour, min, sec, msec, ihd[40];
    float dt, B = 0.0, E, fhd[70], *tr, evla, evlo, stla, stlo, dist;
    float azi, bazi, cmpaz, cmpinc;
    char **chd, ename[17], sname[9];

    chd = char_matrix(0, 7, 0, 23);

    /* Initialize Header */
    for(i = 0; i < 40; i++) ihd[i] = -12345;
    for(i = 0; i < 70; i++) fhd[i] = -12345.00;
    for(i = 0; i < 8; i++) snprintf(chd[i], 24, "-12345  -12345  -12345");

    strncpy(ename, "-12345  -12345", sizeof(ename) - 1);
    ename[sizeof(ename) - 1] = '\0';
    
    strncpy(sname, "-12345", sizeof(sname) - 1);
    sname[sizeof(sname) - 1] = '\0';

    /* Set Essential Parameters */
    ihd[35] = 1;
    ihd[15] = 1;
    ihd[6] = 6;

    setpar(ac, av);
    mstpar("npts", "d", &npts);
    ihd[9] = npts;
    
    mstpar("dt", "f", &dt);
    fhd[0] = dt;
    
    if(getpar("stime", "f", &B))
        fhd[5] = B;
    if(getpar("year", "d", &year))
        ihd[0] = year;
    if(getpar("jday", "d", &jday))
        ihd[1] = jday;
    if(getpar("hour", "d", &hour))
        ihd[2] = hour;
    if(getpar("min", "d", &min))
        ihd[3] = min;
    if(getpar("sec", "d", &sec))
        ihd[4] = sec;
    if(getpar("msec", "d", &msec))
        ihd[5] = msec;

    getstrpar("ename", ename, sizeof(ename));
    getstrpar("sname", sname, sizeof(sname));

    if(getpar("cmpaz", "f", &cmpaz))
        fhd[57] = cmpaz;
    if(getpar("cmpinc", "f", &cmpinc))
        fhd[58] = cmpinc;
    if(getpar("stla", "f", &stla)) {
        fhd[31] = stla;
        fprintf(stderr, "%f\n", fhd[31]);
    }
    if(getpar("stlo", "f", &stlo))
        fhd[32] = stlo;
    if(getpar("evla", "f", &evla))
        fhd[35] = evla;
    if(getpar("evlo", "f", &evlo))
        fhd[36] = evlo;
    if(getpar("dist", "f", &dist))
        fhd[50] = dist;
    if(getpar("azi", "f", &azi))
        fhd[51] = azi;
    if(getpar("bazi", "f", &bazi))
        fhd[52] = bazi;
    
    endpar();

    snprintf(chd[0], 24, "%-8s%-16s", sname, ename);
    fprintf(stderr, "bin2sac: %s  %s   chd[0]: %s \n", ename, sname, chd[0]);
    
    fhd[6] = B + fhd[0] * (ihd[9] - 1);
    
    tr = (float *)malloc(sizeof(float) * npts);
    if (!tr) {
        fprintf(stderr, "Memory allocation failed for trace data\n");
        exit(EXIT_FAILURE);
    }

    // CORRECCIÓN: usar int en lugar de ssize_t
    int bytes_read = read(0, tr, npts * sizeof(float));
    if (bytes_read != npts * sizeof(float)) {
        fprintf(stderr, "Error reading trace data: expected %d bytes, got %d bytes\n", 
                npts * sizeof(float), bytes_read);
        free(tr);
        exit(EXIT_FAILURE);
    }

    for(int k = 0; k < (npts < 10 ? npts : 10); k++)
        fprintf(stderr, "%d %e \n", k, tr[k]);

    write(1, fhd, 70 * sizeof(float));
    write(1, ihd, 40 * sizeof(int));
    write(1, chd[0], 24 * 8);
    write(1, tr, npts * sizeof(float));

 
 
    free(tr);
    return 0;
}
