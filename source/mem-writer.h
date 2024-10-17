#ifndef MEM_WRITER_H
#define MEM_WRITER_H


struct sMemWriter {
    char* filename;
};


void init_mem_writer(struct sMemWriter *mw, char* filename);




#endif //MEM_WRITER_H
