/* Program to generate term-biased snippets for paragraphs of text.

   Skeleton program written by Alistair Moffat, ammoffat@unimelb.edu.au,
   August 2022, with the intention that it be modified by students
   to add functionality, as required by the assignment specification.

   Student Authorship Declaration:

   (1) I certify that except for the code provided in the initial skeleton
   file, the  program contained in this submission is completely my own
   individual work, except where explicitly noted by further comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students, or by
   non-students as a result of request, solicitation, or payment, may not be
   submitted for assessment in this subject.  I understand that submitting for
   assessment work developed by or in collaboration with other students or
   non-students constitutes Academic Misconduct, and may be penalized by mark
   deductions, or by other penalties determined via the University of
   Melbourne Academic Honesty Policy, as described at
   https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will I
   do so until after the marks are released. I understand that providing my
   work to other students, regardless of my intention or any undertakings made
   to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring service,
   or drawing the attention of others to such services and code that may have
   been made available via such a service, may be regarded as Student General
   Misconduct (interfering with the teaching activities of the University
   and/or inciting others to commit Academic Misconduct).  I understand that
   an allegation of Student General Misconduct may arise regardless of whether
   or not I personally make use of such solutions or sought benefit from such
   actions.

   Signed by: [Anh Phi Vu ; Student ID: 1266276]
   Dated:     [07/09/2022]

*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

	/* you may need to delete this next line on some platforms
	   in order to get it to compile; if you end up doing that
	   make sure you put it back before you submit to the
	   Gradescope system, because it is required there */
#include <strings.h>

	/* maximum number of characters per word */
#define MAX_WORD_LEN 23
	/* maximum number of words per paragraph */
#define MAX_PARA_LEN 10000

	/* return code from get_word if end of paragraph found */
#define PARA_END 1
	/* return code from get_word if a word successfully found */
#define WORD_FND 2

	/* terminating punctuation that may follow a word */
#define TERM_PUNCT ".,;:!?"
	/* terminating punctuation that needs dots added */
#define NEEDS_DOTS ",;:"
	/* the string the separates paragraphs */
#define PARA_SEPARATOR "\n\n"
	/* insertion characters to indicate "bold" */
#define BBOLD "**"
	/* length of those characters when wrapped around a word */
#define BBOLD_LEN (2*strlen(BBOLD))
	/* insertion characters to indicate omitted text */
#define DDOTS "..."

	/* maximum words in any output snippet */
#define MAX_SNIPPET_LEN 30
	/* minimum words in any output snippet */
#define MIN_SNIPPET_LEN 20
	/* maximum length of output lines */
#define MAX_OUTPUT_LINE 72

	/* maximum terms allowed on command line */
#define MAX_TERMS 50

typedef char word_t[MAX_WORD_LEN + 1];

int     get_word(word_t w, int limit);

/* If you wish to add further #defines, put them below this comment,
   then prototypes for the functions that you add

   The only thing you should alter above this line is to complete the
   Authorship Declaration 
*/

typedef word_t para_t[MAX_PARA_LEN + 1];

int  	get_paragraph(para_t para, int limit);
int     word_in(word_t word, char **list_of_word, int length);
int     get_repeated(char *word, para_t para, int start, int length);
char    get_punct(word_t word);
char    check_punct(word_t word);
char    need_dot(word_t word);

/****************************************************************/


/* main program controls all the action */
int
main(int argc, char *argv[]) {
	para_t para;
	int word_count = 0;
	int para_count = 0;
	 
	printf("\n");
	while ((word_count = get_paragraph(para, MAX_PARA_LEN)) != -1) {
		para_count++;
		/* STAGE 1 */
		printf("======= Stage 1 [para %d; %d words]\n\n", para_count, word_count);

		/* PRE-STAGE 3
		   calculate the snippet's score, the lenght of snippet,
		   and the start snippet word. doing this before stage 2
		   is to avoid taking into account the word being bolded
		*/
		int start_snippet = 0;
		double max_snippet_score = 0; 
		int length_of_best_snippet = 0;
		for (int snippet_index = 0; snippet_index <= (word_count - MIN_SNIPPET_LEN); 
			snippet_index++) { 

			for (int length_snippet = MIN_SNIPPET_LEN; length_snippet < MAX_SNIPPET_LEN; 
				length_snippet++) {

				if ((snippet_index + length_snippet) > word_count) {
					continue;
				}
				double score_snippet = 0;
				score_snippet = 15.0/(snippet_index + 10.0);
				for (int i = 1; i < argc; i++) {
					int repeat_num = get_repeated(argv[i], para, snippet_index, 
									 length_snippet);

					if (repeat_num > 0) {
						score_snippet += strlen(argv[i])/2.0;
						score_snippet += repeat_num - 1;
					}	
				}
				if (snippet_index == 0 || check_punct(para[snippet_index - 1])) {
					score_snippet += 0.6;
				}
				if (check_punct(para[snippet_index + length_snippet - 1])) {
					score_snippet += 0.3;
				}
				score_snippet -= 0.1*(length_snippet - MIN_SNIPPET_LEN);

				if (score_snippet > max_snippet_score) {
					max_snippet_score = score_snippet;
					length_of_best_snippet = length_snippet;
					start_snippet = snippet_index;
				}
			}
		}

		/* handling the case where the whole paragraph is less than 20, 
		   the minimum snippet length. in this case, just print out 
		   the whole paragraph.
		*/
		if (word_count < MIN_SNIPPET_LEN) {
			/* whole paragraph start word always = 0*/
			start_snippet = 0; 
			/* whole paragraph taken so the length is the word count*/
			length_of_best_snippet = word_count; 
			double score_snippet = 0;
			score_snippet = 15.0/(10.0);
			for (int i = 1; i < argc; i++) {
				int repeat_num = get_repeated(argv[i], para, 0, word_count);
				if (repeat_num > 0) {
					score_snippet += strlen(argv[i])/2.0;
					score_snippet += repeat_num - 1;
				}				
			}
			score_snippet += 0.6;
			if (check_punct(para[word_count - 1])) {
				score_snippet += 0.3;
			}
			max_snippet_score = score_snippet;
		}

		/* STAGE 2*/
		int match_count = 0;
		int current_line_len = 0;
		char punct[2]; 
		for (int i = 0; i < word_count ; i++) {
			punct[0] = get_punct(para[i]);
			punct[1] = 0;
			if (word_in(para[i], argv + 1, argc - 1)) {
				/* Put the bold around the word that exclude the punctuation */
				word_t temp;
				strcpy(temp, para[i]);
				strcpy(para[i], BBOLD);
				strcat(para[i], temp);
				strcat(para[i], BBOLD);
				match_count++;
			} 
			strcat(para[i], punct);

		}

		/* Print out STAGE 2*/
		printf("======= Stage 2 [para %d; %d words; %d matches]\n", para_count, 
			  word_count, match_count);

		for (int i = 0; i < word_count ; i++) {
			if ((current_line_len + strlen(para[i]) + 1) > MAX_OUTPUT_LINE) {
				current_line_len = 0;
				printf("\n");
			} else {
				if (i > 0) {
					printf(" ");
					current_line_len++;
				}	
			}
			printf("%s", para[i]);
			current_line_len += strlen(para[i]);
		}
		printf("\n\n");

		/* Print out STAGE 3 */
		current_line_len = 0;
		printf("======= Stage 3 [para %d; start %d; length %d; score %.2lf]\n", 
			  para_count, start_snippet, length_of_best_snippet, max_snippet_score);

		for (int i = start_snippet; i < length_of_best_snippet + start_snippet; i++) {
			if ((current_line_len + strlen(para[i]) + 1) > MAX_OUTPUT_LINE) {
				current_line_len = 0;
				printf("\n");
			} else {
				if (i > start_snippet) {
					printf(" ");
					current_line_len++;
				}
			}
			printf("%s", para[i]);
			current_line_len += strlen(para[i]);

		}

		/* Check if the snippet end with either ",;:",
		   if yes, print "..." at the end of the snippet. 
		*/
		char *final_word;
		final_word = para[(length_of_best_snippet + start_snippet) - 1];
		if ((!check_punct(final_word)) || (need_dot(final_word))) {
			printf("...");
		}
		printf("\n\n");
	}

	printf("ta daa!\n");

	return 0;
}

/****************************************************************/


/* extract a single word out of the standard input, but not
   more than "limit" characters in total. Also check if we 
   reach the end of paragrpah or not. One character of
   sensible trailing punctuation is retained.  
   argument array W must be limit+1 characters or bigger
*/
int get_word(word_t W, int limit) {
	int c;
	int prev_new = 0;

	/* first, skip over any non alphanumerics */
	while ((c = getchar()) != EOF && !isalnum(c)) {

		// if we potentially in the sperator, check if we still in the seperator //
		if (prev_new == 1 && c == '\n'){
			return PARA_END;
		}
		if (c == '\n'){
			prev_new = 1;
		}else{
			prev_new = 0;
		}
	}

	/* Check if you have read in a blank line */
	if (c==EOF) {
		return EOF;
	}
	
	/* ok, first character of next word has been found */
	*W = c;
	W += 1;
	limit -= 1;
	while (limit>0 && (c=getchar())!=EOF && isalnum(c)) {
		/* another character to be stored */
		*W = c;
		W += 1;
		limit -= 1;
	}
	/* take a look at that next character, is it a sensible trailing
	   punctuation? */
	if (strchr(TERM_PUNCT, c) && (limit>0)) {
		/* yes, it is */
		*W = c;
		W += 1;
		limit -= 1;
	}

	/* now close off the string */
	*W = '\0';
	return WORD_FND;
}

/* extract the paragraph from the standard input 
   ultilising the get_word function. At the end, 
   return the length of the paragraph. 
*/
int get_paragraph(para_t para, int limit) {
	int length_para = 0;
	word_t w;
	int result = get_word(w, MAX_WORD_LEN);
	if (result == EOF) {
		return -1;
	}
	while (result == WORD_FND && length_para < limit) {
		// puts the word into the paragraph //
		strcpy(para[length_para], w);
		length_para++;
		result = get_word(w, MAX_WORD_LEN);
	}
	return length_para;
}

/* check if the query term input from the command
   found in a particular paragraph or not. 
*/
int word_in(word_t word, char **list_of_word, int length) {
	for (int i = 0; i < length; i++) {
		if (strcasecmp(word, list_of_word[i]) == 0) {
			return 1;
		}
		/* Exclude the punctuation if it is at the end of the word */
	}
	return 0;
}

/* extract the word with the punctuation locate
   at the end and take the punctuation out.
*/
char get_punct(word_t word) {
	for (int i = 0; i < strlen(TERM_PUNCT); i++) {
		if (word[strlen(word) - 1] == TERM_PUNCT[i]) {
			// remove the punctuation
			word[strlen(word) - 1] = 0;
			return TERM_PUNCT[i]; // retrun true
		}
	} 
	return 0; // return false
}

/* check the word with the punctuation locate
   at the end. Same as the get_punct function but
   doesn't take the punctuation out.
*/
char check_punct(word_t word) {
	for (int i = 0; i < strlen(TERM_PUNCT); i++) {
		if (word[strlen(word) - 1] == TERM_PUNCT[i]) {
			return TERM_PUNCT[i]; //return true
		}
	} 
	return 0; //return false
}

/* check the word with either ":;," at the end 
   and later use in stage 3 to determine whether
   to print "..." at the end.
*/
char need_dot(word_t word) {
	for (int i = 0; i < strlen(NEEDS_DOTS); i++) {
		if (word[strlen(word) - 1] == NEEDS_DOTS[i]) {
			return NEEDS_DOTS[i]; //return true
		}
	} 
	return 0; //return false
}

/* check how many times the query term input from
   the command prompt is repeated. Mainly use in 
   stage 3 to calculate the snippet score.
*/
int get_repeated(char *word, para_t para, int start, int length) {
	int num_repeated = 0;
	word_t temp_word;
	for(int i = start; i < (start + length); i++) {
		strcpy(temp_word, para[i]);
		if (check_punct(para[i])) {
			temp_word[strlen(temp_word) - 1] = 0;
		}
		if (strcasecmp(word, temp_word) == 0) {
			num_repeated++;
		}
	} 
	return num_repeated;
}

/* algorithms are fun */