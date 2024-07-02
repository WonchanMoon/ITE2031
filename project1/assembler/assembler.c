/* Assembler code fragment for LC-2K */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000
#define MAXMEM 65536
#define REGNUM 8

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int addLabel(char *, int);
int findLabel(char *);
int Rtype(char *, char *, char *, char *);
int Itype(int, char *, char *, char *, char *);
int Jtype(char *, char *, char *);
int Otype(char *);
int fillValue(char *);
int checkRange(int, int, int);

struct {
    char label[7];
    int busy;
    int address;
} labels[MAXMEM];

int main(int argc, char *argv[]) 
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr;
	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], 
			 arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

	if (argc != 3) {
		printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
				argv[0]);
		exit(1);
	}

	inFileString = argv[1];
	outFileString = argv[2];

	inFilePtr = fopen(inFileString, "r");
	if (inFilePtr == NULL) {
		printf("error in opening %s\n", inFileString);
		exit(1);
	}
	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	/* TODO: Phase-1 label calculation */
	int pc = -1;
	while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
        pc++;

        // Is the label exist?
        if(!strlen(label)){
            continue;
        }
        // Is the label started with a letter?
        if(label[0]>='0' && label[0]<='9'){
            printf("error : Label is not started with a leter\n");
            exit(1); // File is closed automatically
        }
        // Is the legth of the label > 6 ?
        if(strlen(label)>6){
            printf("error : The length of the label is more then 6\n");
            exit(1); // File is closed automatically
        }
        // Is the label duplicated?
        if(findLabel(label) != -1){
            printf("error : The label is duplicated\n");
            exit(1); // File is closed automatically
        }
        // Is the memory full?
        if(!addLabel(label, pc)){
            printf("error : memory is full\n");
            exit(1);
        }
    }
	/* this is how to rewind the file ptr so that you start reading from the
		 beginning of the file */
	rewind(inFilePtr);

	/* TODO: Phase-2 generate machine codes to outfile */
	pc = -1;
	while(readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
        pc++;
        int output;

        // Step 2-1 : Split by opcode
        if(!strcmp(opcode, "add") || !strcmp(opcode, "nor")) output = Rtype(opcode, arg0, arg1, arg2);
        else if(!strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, "beq")) output = Itype(pc, opcode, arg0, arg1, arg2);
        else if(!strcmp(opcode, "jalr")) output = Jtype(opcode, arg0, arg1);
        else if(!strcmp(opcode, "halt") || !strcmp(opcode, "noop")) output = Otype(opcode);
        else if(!strcmp(opcode, ".fill")) output = fillValue(arg0);
        else {
            printf("error : Unrecognized opcodes\n");
            printf("%s\n", opcode);
            exit(1);
        }
    
        // Step 2-2 : Write output
        fprintf(outFilePtr, "%d\n", output);
    }

	if (inFilePtr) {
		fclose(inFilePtr);
	}
	if (outFilePtr) {
		fclose(outFilePtr);
	}
	return(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
		char *arg1, char *arg2)
{
	char line[MAXLINELENGTH];
	char *ptr = line;

	/* delete prior values */
	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

	/* read the line from the assembly-language file */
	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
		/* reached end of file */
		return(0);
	}

	/* check for line too long (by looking for a \n) */
	if (strchr(line, '\n') == NULL) {
		/* line too long */
		printf("error: line too long\n");
		exit(1);
	}

	/* is there a label? */
	ptr = line;
	if (sscanf(ptr, "%[^\t\n\r ]", label)) {
		/* successfully read label; advance pointer over the label */
		ptr += strlen(label);
	}

	/*
	 * Parse the rest of the line.  Would be nice to have real regular
	 * expressions, but scanf will suffice.
	 */
	sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%"
			"[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", opcode, arg0, arg1, arg2);
	return(1);
}

int isNumber(char *string)
{
	/* return 1 if string is a number */
	int i;
	return( (sscanf(string, "%d", &i)) == 1);
}

int addLabel(char *label, int pc){
    int i;
    for(i = 0 ; i < MAXMEM ; i++){
        if(!labels[i].busy){
            strcpy(labels[i].label, label);
            labels[i].busy = 1;
            labels[i].address = pc;
            break;
        }
    }
    if(i==MAXMEM) return 0;
    return 1;
}

int findLabel(char *label){
    for(int i = 0 ; i < MAXMEM ; i++){
        if(labels[i].busy == 0) break;
        if(!strcmp(labels[i].label, label)) return labels[i].address;
    }
    return -1;
}

int checkRange(int num, int lower, int upper){
    if(lower <= num && num <= upper) return 1;
    return 0;
}

int Rtype(char *opcode, char *arg0, char *arg1, char *arg2){
    // Are register args number?
    if(!isNumber(arg0) || !isNumber(arg1) || !isNumber(arg2)){
        printf("error : Non-integer register arguments\n");
        exit(1);
    }
    
    // Are registers out of range?
    if((!checkRange(atoi(arg0), 0, (REGNUM - 1))) || (!checkRange(atoi(arg1), 0, (REGNUM - 1))) || (!checkRange(atoi(arg2), 0, (REGNUM - 1)))){
        printf("error : Registers outside the range [0,7]\n");
        exit(1);
    }
    
    if(!strcmp(opcode, "add")) return (0 << 22 | atoi(arg0) << 19 | atoi(arg1) << 16 | atoi(arg2));
    else return (1 << 22 | atoi(arg0) << 19 | atoi(arg1) << 16 | atoi(arg2));
}

int Itype(int pc, char *opcode, char *arg0, char *arg1, char *arg2){
    // Are register args number?
    if(!isNumber(arg0) || !isNumber(arg1)){
        printf("error : Non-integer register arguments\n");
        exit(1);
    }

    // Are registers out of range?
    if((!checkRange(atoi(arg0), 0, (REGNUM - 1))) || (!checkRange(atoi(arg1), 0, (REGNUM - 1)))){
        printf("error : Registers outside the range [0,7]\n");
        exit(1);
    }

    if(isNumber(arg2)){
        // Does the offset fit in 16 bits?
        int offset = atoi(arg2);
        if(!checkRange(offset, (-1)*(MAXMEM/2 + 1), MAXMEM/2)){
            printf("error : offsetFields that does not fit in 16 bits\n");
            exit(1);
        }
        if(!strcmp(opcode, "lw")) return (2 << 22 | atoi(arg0) << 19 | atoi(arg1) << 16 | (offset & 0xffff));
        if(!strcmp(opcode, "sw")) return (3 << 22 | atoi(arg0) << 19 | atoi(arg1) << 16 | (offset & 0xffff));
        if(!strcmp(opcode, "beq")) return (4 << 22 | atoi(arg0) << 19 | atoi(arg1) << 16 | (offset & 0xffff));
    }
    else{
        // Is the label exist?
        int labelAddress = findLabel(arg2);
        if(labelAddress == -1){
            printf("error : Use of undefined labels\n");
            exit(1);
        }
        if(!strcmp(opcode, "lw")) return (2 << 22 | atoi(arg0) << 19 | atoi(arg1) << 16 | (labelAddress & 0xffff));
        if(!strcmp(opcode, "sw")) return (3 << 22 | atoi(arg0) << 19 | atoi(arg1) << 16 | (labelAddress & 0xffff));
        if(!strcmp(opcode, "beq")) {
            labelAddress = labelAddress - (pc + 1);
            return (4 << 22 | atoi(arg0) << 19 | atoi(arg1) << 16 | (labelAddress & 0xffff));
        }
    }
}

int Jtype(char *opcode, char *arg0, char *arg1){
    // Are registers out of range?
    if((!checkRange(atoi(arg0), 0, (REGNUM - 1))) || (!checkRange(atoi(arg1), 0, (REGNUM - 1)))){
        printf("error : Registers outside the range [0,7]\n");
        exit(1);
    }
    return (5 << 22 | atoi(arg0) << 19 | atoi(arg1) << 16);
}

int Otype(char *opcode){
    if(!strcmp(opcode, "halt")) return (6 << 22);
    else return (7 << 22);
}

int fillValue(char *arg0){
    // Num or Label
    if(isNumber(arg0)) return atoi(arg0);
    else{
        int labelAddress = findLabel(arg0);
        if(labelAddress == -1){
            printf("error : Use of undefined labels(.fill)\n");
            exit(1);
        }
        return labelAddress;
    }
}
